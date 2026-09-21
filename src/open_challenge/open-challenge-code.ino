#include <Servo.h>

// =====================================================
// PIN SETUP
// =====================================================

// Steering servo
const int SERVO_PIN = 7;

// Drive motor
const int MOTOR_PWM = 5;
const int MOTOR_IN1 = 32;
const int MOTOR_IN2 = 33;

// Start button
// Wired from D34 to GND, so INPUT_PULLUP is used
const int START_BUTTON = 34;

// TCS3200 color sensor
const int S0 = 35;
const int S1 = 36;
const int S2 = 37;
const int S3 = 38;
const int COLOR_OUT = 39;


// =====================================================
// ROBOT SETTINGS
// =====================================================

// We count 12 blue-line corners:
// 4 corners per lap × 3 laps
int cornerCount = 0;

// After the 12th corner, keep driving straight
// for this long before stopping
const unsigned long FINISH_DRIVE_MS = 1250;

// Steering angles
const int STEER_CENTER = 82;
const int STEER_LEFT = 55;

// Motor speeds
const int NORMAL_SPEED = 140;
const int TURN_SPEED = 155;

// How long the robot keeps the steering locked left
// when a blue line is detected
const unsigned long TURN_TIME_MS = 1000;


// =====================================================
// GLOBAL VARIABLES
// =====================================================

Servo steering;

// Prevents the same blue line from triggering
// multiple turns while the sensor is still over it
bool blueArmed = true;

// Counts how many consecutive non-blue readings we get
// before allowing another blue-line trigger
int nonBlueReadings = 0;


// =====================================================
// MOTOR FUNCTIONS
// =====================================================

// Drive the robot forward at the requested PWM speed
void driveForward(int speedValue) {

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);

  analogWrite(MOTOR_PWM, speedValue);
}


// Stop the drive motor completely
void stopMotor() {

  analogWrite(MOTOR_PWM, 0);

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}


// =====================================================
// COLOR SENSOR FUNCTIONS
// =====================================================

// Reads one color channel from the TCS3200.
//
// S2 and S3 select which color filter is active.
// The sensor gives a pulse frequency, so we convert
// the pulse duration into a simple frequency-like value.
unsigned long readColorChannel(bool s2, bool s3) {

  digitalWrite(S2, s2);
  digitalWrite(S3, s3);

  // Small delay so the sensor has time to switch filters
  delay(3);

  unsigned long pulse =
      pulseIn(COLOR_OUT, LOW, 30000);

  // No pulse received within timeout
  if (pulse == 0) {
    return 0;
  }

  return 500000UL / pulse;
}


// Take 3 readings and average them.
// This makes the color detection more stable.
unsigned long averageColor(bool s2, bool s3) {

  unsigned long total = 0;

  for (int i = 0; i < 3; i++) {

    total += readColorChannel(s2, s3);
  }

  return total / 3;
}


// =====================================================
// COLOR DETECTION
//
// Returns:
// 0 = other / white
// 1 = blue
// 2 = orange
// =====================================================

int detectColor() {

  // Read red channel
  unsigned long R =
      averageColor(LOW, LOW);

  // Read green channel
  unsigned long G =
      averageColor(HIGH, HIGH);

  // Read blue channel
  unsigned long B =
      averageColor(LOW, HIGH);


  // Print raw values so we can calibrate the sensor
  Serial.print("R: ");
  Serial.print(R);

  Serial.print("   G: ");
  Serial.print(G);

  Serial.print("   B: ");
  Serial.print(B);

  Serial.print("   ---> ");


  // =================================================
  // BLUE DETECTION
  //
  // These ranges were calibrated from the real field.
  //
  // R: 10000 - 19000
  // G:  9000 - 14500
  // B: 11000 - 20500
  //
  // The last condition helps separate blue from orange.
  // =================================================

  if (
      R >= 10000 &&
      R <= 19000 &&

      G >= 9000 &&
      G <= 14500 &&

      B >= 11000 &&
      B <= 20500 &&

      B + 3000 >= R
     )
  {
    Serial.println("BLUE");

    return 1;
  }


  // =================================================
  // ORANGE DETECTION
  //
  // Orange is only identified for debugging.
  // The robot does not turn when it sees orange.
  // =================================================

  if (
      R >= 17500 &&
      R <= 25500 &&

      G >= 12000 &&
      G <= 16500 &&

      B >= 13000 &&
      B <= 19000 &&

      R > B + 3000
     )
  {
    Serial.println("ORANGE");

    return 2;
  }


  // Anything outside the blue/orange ranges
  Serial.println("OTHER");

  return 0;
}


// =====================================================
// BLUE-LINE TURN
// =====================================================

void turnLeft() {

  Serial.println();
  Serial.println(">>> BLUE DETECTED");
  Serial.println(">>> STARTING LEFT TURN");


  // Turn the steering left
  steering.write(STEER_LEFT);


  // Keep the car moving while turning
  driveForward(TURN_SPEED);


  // Hold the turn for the calibrated amount of time
  delay(TURN_TIME_MS);


  // Straighten the wheels again
  steering.write(STEER_CENTER);


  // Return to normal driving speed
  driveForward(NORMAL_SPEED);


  Serial.println(">>> TURN COMPLETE");
  Serial.println();
}


// =====================================================
// FINISH THE RUN
// =====================================================

void finishRun() {

  Serial.println();
  Serial.println("============================");
  Serial.println(">>> 3 LAPS COMPLETE");
  Serial.println(">>> MOVING INTO FINISH AREA");
  Serial.println("============================");


  // Make sure the wheels are straight
  steering.write(STEER_CENTER);


  // Continue forward into the finish section
  driveForward(NORMAL_SPEED);


  // Drive a little farther after the final corner
  delay(FINISH_DRIVE_MS);


  // Stop the robot
  stopMotor();


  // Keep the wheels centered when stopped
  steering.write(STEER_CENTER);


  Serial.println(">>> STOPPED");


  // Stay stopped permanently until the robot is reset
  while (true) {

    steering.write(STEER_CENTER);

    stopMotor();

    delay(100);
  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);


  // -------------------------------------------------
  // Steering servo
  // -------------------------------------------------

  steering.attach(SERVO_PIN);

  // Start with the wheels centered
  steering.write(STEER_CENTER);


  // -------------------------------------------------
  // Motor
  // -------------------------------------------------

  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  // Robot should not move before the start button
  stopMotor();


  // -------------------------------------------------
  // Color sensor
  // -------------------------------------------------

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);

  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(COLOR_OUT, INPUT);


  // Set TCS3200 output frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);


  // -------------------------------------------------
  // Start button
  // -------------------------------------------------

  pinMode(START_BUTTON, INPUT_PULLUP);


  // Reset corner count whenever the robot restarts
  cornerCount = 0;


  Serial.println();
  Serial.println("READY - PRESS BUTTON");


  // Keep the wheels centered while waiting
  steering.write(STEER_CENTER);


  // Wait until the start button is pressed
  while (digitalRead(START_BUTTON) == HIGH) {

    steering.write(STEER_CENTER);
  }


  // Re-center the steering when the button is pressed
  steering.write(STEER_CENTER);

  delay(100);


  // Wait until the button is released
  while (digitalRead(START_BUTTON) == LOW) {

    steering.write(STEER_CENTER);
  }


  // Short delay before movement begins
  delay(300);


  Serial.println("START");
  Serial.println("CORNER 0 / 12");


  // Start driving straight
  steering.write(STEER_CENTER);

  driveForward(NORMAL_SPEED);
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Check the floor color
  int color = detectColor();


  // =================================================
  // BLUE LINE DETECTED
  // =====================================================

  if (color == 1) {

    // We are currently on blue, so reset this counter
    nonBlueReadings = 0;


    // Only react once to each blue line
    if (blueArmed) {

      blueArmed = false;


      // Count this as one corner
      cornerCount++;


      Serial.print("CORNER: ");
      Serial.print(cornerCount);
      Serial.println(" / 12");


      // Make the left turn
      turnLeft();


      // After 12 corners, 3 laps are complete
      if (cornerCount >= 12) {

        finishRun();
      }
    }
  }


  // =================================================
  // NOT BLUE
  // =====================================================

  else {

    nonBlueReadings++;


    // Wait for 3 non-blue readings before allowing
    // the next blue line to trigger another turn.
    //
    // This prevents one wide blue line from being
    // counted as multiple corners.
    if (nonBlueReadings >= 3) {

      blueArmed = true;
    }


    // Normal driving:
    // wheels centered and normal forward speed
    steering.write(STEER_CENTER);

    driveForward(NORMAL_SPEED);
  }
}
