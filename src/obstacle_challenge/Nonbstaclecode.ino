#include <Servo.h>

// =====================================================
// HARDWARE PINS
// =====================================================

Servo steering;

// Steering servo
const int SERVO_PIN = 7;

// Drive motor
const int MOTOR_PWM = 5;
const int MOTOR_IN1 = 32;
const int MOTOR_IN2 = 33;

// Start button
// Connected from D34 to GND
const int START_BUTTON = 34;


// =====================================================
// BASIC MOVEMENT FUNCTIONS
// =====================================================

// -----------------------------------------------------
// FORWARD
//
// Keeps the wheels centered at 82 degrees
// and drives forward at speed 150.
//
// Example:
// FORWARD(1000);
// means drive straight for 1 second.
// -----------------------------------------------------

void FORWARD(unsigned long ms) {

  steering.write(82);

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);

  analogWrite(MOTOR_PWM, 150);

  delay(ms);
}


// -----------------------------------------------------
// LEFT
//
// Gentle left steering.
//
// Used mainly for obstacle avoidance,
// not for full 90-degree field corners.
//
// Steering = 65 degrees
// Speed = 150
// -----------------------------------------------------

void LEFT(unsigned long ms) {

  steering.write(65);

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);

  analogWrite(MOTOR_PWM, 150);

  delay(ms);
}


// -----------------------------------------------------
// RIGHT
//
// Gentle right steering.
//
// Used mainly for obstacle avoidance.
//
// Steering = 92 degrees
// Speed = 150
// -----------------------------------------------------

void RIGHT(unsigned long ms) {

  steering.write(92);

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);

  analogWrite(MOTOR_PWM, 150);

  delay(ms);
}


// -----------------------------------------------------
// CORNER_LEFT
//
// Strong left turn used for the actual
// 90-degree corners of the WRO field.
//
// Steering = 55 degrees
// Speed = 220
//
// After the turn finishes,
// steering automatically returns to center.
// -----------------------------------------------------

void CORNER_LEFT(unsigned long ms) {

  steering.write(55);

  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);

  analogWrite(MOTOR_PWM, 220);

  delay(ms);

  // Straighten wheels after corner
  steering.write(82);
}


// -----------------------------------------------------
// STOP
//
// Completely stops the drive motor
// and centers the steering.
// -----------------------------------------------------

void STOP() {

  analogWrite(MOTOR_PWM, 0);

  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);

  steering.write(82);
}


// =====================================================
// LAP 1
// =====================================================
//
// Robot starts on the RIGHT side of the field.
//
// The movements below are completely time-based.
// =====================================================

void runLap() {

  // =================================================
  // RIGHT SIDE -> TOP-RIGHT CORNER
  // =================================================

  // Drive upward
  FORWARD(1000);

  // Turn onto top side
  CORNER_LEFT(800);


  // =================================================
  // TOP SIDE
  // =================================================
  //
  // GREEN obstacle section
  //
  // Robot should pass on the LEFT side of green.
  // =================================================

  // Approach green
  FORWARD(350);

  // Move back toward the middle after passing green
  RIGHT(320);

  // Continue toward top-left corner
  FORWARD(900);

  // Turn onto left side
  CORNER_LEFT(800);


  // =================================================
  // LEFT SIDE
  // =================================================
  //
  // RED / GREEN obstacle section
  // =================================================

  // Approach obstacle area
  FORWARD(300);


  // -------------------------------------------------
  // RED OBSTACLE
  //
  // Robot should pass to the RIGHT of red.
  // -------------------------------------------------

  // Shift left first
  LEFT(250);

  // Move past obstacle
  FORWARD(350);

  // Correct back toward the right / middle
  RIGHT(500);

  // Continue toward bottom-left corner
  FORWARD(400);

  // Turn onto bottom side
  CORNER_LEFT(800);


  // =================================================
  // BOTTOM SIDE
  // =================================================
  //
  // RED / GREEN obstacle section
  // =================================================

  FORWARD(400);


  // -------------------------------------------------
  // RED OBSTACLE
  // -------------------------------------------------

  LEFT(220);

  // Continue along bottom side
  FORWARD(800);


  // Turn onto right side
  CORNER_LEFT(500);


  // =================================================
  // RIGHT SIDE
  // =================================================
  //
  // Final correction before completing Lap 1
  // =================================================

  // Small right correction
  RIGHT(300);

  // Small left correction
  LEFT(100);


  // Stop at end of Lap 1
  STOP();
}


// =====================================================
// LAP 2
// =====================================================
//
// Second lap has slightly different timings
// from Lap 1.
// =====================================================

void runLap2() {

  // =================================================
  // RIGHT SIDE -> TOP-RIGHT CORNER
  // =================================================

  FORWARD(600);

  CORNER_LEFT(800);


  // =================================================
  // TOP SIDE
  // =================================================
  //
  // GREEN obstacle section
  // =================================================

  // Longer straight approach than Lap 1
  FORWARD(650);

  // Correct toward middle
  RIGHT(320);

  // Continue toward top-left corner
  FORWARD(800);

  // Stronger / longer corner than Lap 1
  CORNER_LEFT(1000);


  // =================================================
  // LEFT SIDE
  // =================================================

  FORWARD(300);


  // -------------------------------------------------
  // RED OBSTACLE
  //
  // Previous LEFT movement is currently disabled.
  // -------------------------------------------------

  // LEFT(350);


  // Continue forward
  FORWARD(600);


  // Previous RIGHT correction is currently disabled.
  // RIGHT(400);


  FORWARD(400);


  // Turn onto bottom side
  CORNER_LEFT(800);


  // =================================================
  // BOTTOM SIDE
  // =================================================

  FORWARD(400);


  // -------------------------------------------------
  // RED OBSTACLE
  // -------------------------------------------------

  LEFT(220);

  // Continue toward next obstacle / corner
  FORWARD(800);


  // Turn onto right side
  CORNER_LEFT(500);


  // =================================================
  // RIGHT SIDE
  // =================================================
  //
  // Final corrections before stopping.
  // =================================================

  // FORWARD(150);

  RIGHT(300);

  LEFT(100);


  STOP();
}


// =====================================================
// SETUP
// =====================================================
//
// Runs once when the Arduino powers on.
//
// 1. Initialize servo
// 2. Initialize motor pins
// 3. Wait for start button
// 4. Run Lap 1
// 5. Run Lap 2
// =====================================================

void setup() {

  // Attach steering servo
  steering.attach(SERVO_PIN);

  // Start centered
  steering.write(82);


  // Motor pins
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);


  // Start button
  pinMode(START_BUTTON, INPUT_PULLUP);


  // Make sure robot is stopped before starting
  STOP();


  // =================================================
  // WAIT FOR BUTTON PRESS
  //
  // INPUT_PULLUP means:
  // HIGH = button not pressed
  // LOW  = button pressed
  // =================================================

  while (digitalRead(START_BUTTON) == HIGH) {

    steering.write(82);
  }


  delay(100);


  // =================================================
  // WAIT FOR BUTTON RELEASE
  // =================================================

  while (digitalRead(START_BUTTON) == LOW) {

    steering.write(82);
  }


  // Small pause before starting
  delay(300);


  // =================================================
  // RUN BOTH LAPS
  // =================================================

  runLap();

  runLap2();
}


// =====================================================
// MAIN LOOP
// =====================================================
//
// After both laps finish,
// keep the robot permanently stopped.
// =====================================================

void loop() {

  STOP();

  delay(100);
}