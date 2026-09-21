#include <Servo.h>

// =====================================================
// PINS
// =====================================================

const int SERVO_PIN = 7;

// Motor
const int MOTOR_PWM = 5;
const int MOTOR_IN1 = 32;
const int MOTOR_IN2 = 33;

// Start button: D34 -> button -> GND
const int START_BUTTON = 34;

// Color sensor
const int S0 = 35;
const int S1 = 36;
const int S2 = 37;
const int S3 = 38;
const int COLOR_OUT = 39;


// =====================================================
// SETTINGS
// =====================================================

int cornerCount = 0;

// After 12th corner:
// drive straight for 1.8 seconds then stop
const unsigned long FINISH_DRIVE_MS = 1250;

// Straight
const int STEER_CENTER = 82;

// Left turn
const int STEER_LEFT = 55;

// Speeds
const int NORMAL_SPEED = 140;
const int TURN_SPEED   = 155;

// Locked left turn
const unsigned long TURN_TIME_MS = 1000;


// =====================================================
// GLOBALS
// =====================================================

Servo steering;

bool blueArmed = true;
int nonBlueReadings = 0;


// =====================================================
// MOTOR
// =====================================================

void driveForward(int speedValue) {

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);

  analogWrite(MOTOR_PWM, speedValue);
}


void stopMotor() {

  analogWrite(MOTOR_PWM, 0);

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}


// =====================================================
// COLOR SENSOR
// =====================================================

unsigned long readColorChannel(bool s2, bool s3) {

  digitalWrite(S2, s2);
  digitalWrite(S3, s3);

  delay(3);

  unsigned long pulse =
      pulseIn(COLOR_OUT, LOW, 30000);

  if (pulse == 0) {
    return 0;
  }

  return 500000UL / pulse;
}


// =====================================================
// AVERAGE 3 READINGS
// =====================================================

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
// 0 = OTHER
// 1 = BLUE
// 2 = ORANGE
// =====================================================

int detectColor() {

  unsigned long R =
      averageColor(LOW, LOW);

  unsigned long G =
      averageColor(HIGH, HIGH);

  unsigned long B =
      averageColor(LOW, HIGH);


  Serial.print("R: ");
  Serial.print(R);

  Serial.print("   G: ");
  Serial.print(G);

  Serial.print("   B: ");
  Serial.print(B);

  Serial.print("   ---> ");


  // =================================================
  // BLUE
  //
  // UPDATED RANGE
  //
  // R: 10000 - 19000
  // G:  9000 - 14500
  // B: 11000 - 20500
  // =====================================================

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
  // ORANGE
  //
  // Robot does NOT turn for orange
  // =====================================================

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


  Serial.println("OTHER");

  return 0;
}


// =====================================================
// LEFT TURN
// =====================================================

void turnLeft() {

  Serial.println();
  Serial.println(">>> BLUE DETECTED");
  Serial.println(">>> LOCKING LEFT TURN");


  // Turn left
  steering.write(STEER_LEFT);


  // Keep moving during turn
  driveForward(TURN_SPEED);


  // Stay locked left
  delay(TURN_TIME_MS);


  // Return to center
  steering.write(STEER_CENTER);


  // Continue straight
  driveForward(NORMAL_SPEED);


  Serial.println(">>> TURN COMPLETE");
  Serial.println();
}


// =====================================================
// FINISH RUN
// =====================================================

void finishRun() {

  Serial.println();
  Serial.println("============================");
  Serial.println(">>> 3 LAPS COMPLETE");
  Serial.println(">>> DRIVE STRAIGHT 1800 MS");
  Serial.println("============================");


  // Center steering
  steering.write(STEER_CENTER);


  // Continue straight
  driveForward(NORMAL_SPEED);


  // Drive into finish area
  delay(FINISH_DRIVE_MS);


  // Stop
  stopMotor();


  // Keep servo centered
  steering.write(STEER_CENTER);


  Serial.println(">>> STOPPED");


  // Stay stopped forever
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


  // =================================================
  // SERVO
  // =====================================================

  steering.attach(SERVO_PIN);

  steering.write(STEER_CENTER);


  // =================================================
  // MOTOR
  // =====================================================

  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);

  stopMotor();


  // =================================================
  // COLOR SENSOR
  // =====================================================

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);

  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(COLOR_OUT, INPUT);


  // 20% frequency scaling
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);


  // =================================================
  // START BUTTON
  // =====================================================

  pinMode(
    START_BUTTON,
    INPUT_PULLUP
  );


  cornerCount = 0;


  Serial.println();
  Serial.println("READY - PRESS BUTTON");


  // Keep steering centered while waiting
  steering.write(STEER_CENTER);


  // =================================================
  // WAIT FOR BUTTON PRESS
  // =====================================================

  while (
    digitalRead(START_BUTTON) == HIGH
  ) {

    steering.write(STEER_CENTER);
  }


  // Reset servo when button is pressed
  steering.write(STEER_CENTER);

  delay(100);


  // =================================================
  // WAIT FOR BUTTON RELEASE
  // =====================================================

  while (
    digitalRead(START_BUTTON) == LOW
  ) {

    steering.write(STEER_CENTER);
  }


  delay(300);


  // =================================================
  // START
  // =====================================================

  Serial.println("START");
  Serial.println("CORNER 0 / 12");


  steering.write(STEER_CENTER);

  driveForward(NORMAL_SPEED);
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  int color = detectColor();


  // =================================================
  // BLUE
  // =====================================================

  if (color == 1) {

    nonBlueReadings = 0;


    // React only once to each blue line
    if (blueArmed) {

      blueArmed = false;


      // Count corner
      cornerCount++;


      Serial.print("CORNER: ");
      Serial.print(cornerCount);
      Serial.println(" / 12");


      // Perform turn
      turnLeft();


      // =================================================
      // AFTER 12 CORNERS = 3 LAPS
      // =====================================================

      if (cornerCount >= 12) {

        finishRun();
      }
    }
  }


  // =================================================
  // EVERYTHING ELSE
  // =====================================================

  else {

    nonBlueReadings++;


    // Must leave blue before another blue can trigger
    if (nonBlueReadings >= 3) {

      blueArmed = true;
    }


    // Always center when nothing is happening
    steering.write(STEER_CENTER);


    driveForward(NORMAL_SPEED);
  }
}
