#include <Servo.h>
#include <DFRobot_HuskylensV2.h>

// =====================================================
// PINS
// =====================================================

// DRV8871 motor driver
const int MOTOR_IN1 = 5;
const int MOTOR_IN2 = 6;

// Steering servo
const int SERVO_PIN = 8;

// Start button: D34 -> button -> GND
const int START_BUTTON = 34;

// Onboard LED: ON while something is not working (sensor failed or camera not connected)
const int LED_PIN = 13;

// TCS3200 color sensor
const int S0 = 35;
const int S1 = 36;
const int S2 = 37;
const int S3 = 38;
const int COLOR_OUT = 39;

// HuskyLens 2 on Serial1: D/T -> pin 19 (RX1), C/R -> pin 18 (TX1), GND -> GND

// Set to false for competition runs (no printing = faster line detection)
const bool DEBUG = true;

// =====================================================
// ULTRASONIC SENSORS
// =====================================================

const int NUM_US = 5;
const int US_RIGHT = 0;
const int US_LEFT = 1;
const int US_FRONT_RIGHT = 2;
const int US_FRONT_LEFT = 3;
const int US_FRONT = 4;

const char* US_NAMES[NUM_US] = {"RIGHT", "LEFT", "FRONT-RIGHT", "FRONT-LEFT", "FRONT"};

//                            RIGHT LEFT  F-RIGHT F-LEFT FRONT
const int US_TRIG[NUM_US] =     {30,  22,   28,     24,    26};
const int US_ECHO[NUM_US] =     {31,  23,   29,     25,    27};

// Set a sensor to false to never use it
const bool US_ALLOWED[NUM_US] = {true, true, true,  true,  true};

const int PING_STEPS = 8;
const int PING_ORDER[PING_STEPS] = {
  US_RIGHT, US_LEFT, US_FRONT, US_FRONT_RIGHT,
  US_RIGHT, US_LEFT, US_FRONT, US_FRONT_LEFT
};

// =====================================================
// STEERING AND SPEED
// =====================================================

const int STEER_CENTER = 82;
const int STEER_LEFT = 55;
const int STEER_RIGHT = 119;

const int NORMAL_SPEED = 200;
const int TURN_SPEED = 200;
const int FINISH_SPEED = 200;

const int KICK_SPEED = 255;
const unsigned long KICK_TIME_MS = 150;

// =====================================================
// CORNERS (same as the open challenge, tuned at speed 200)
// =====================================================

// Blue (left) turn times
const unsigned long FIRST_TURN_TIME_MS = 2200;
const unsigned long TURN_TIME_MS = 1800;
const unsigned long LAST_TURN_TIME_MS = 2100;

// Orange (right) turn: chosen by distance to the inner wall
const int TURN_CLEARANCE_CM = 30;
const unsigned long WAIT_MS_PER_CM = 20;
const unsigned long MAX_WAIT_MS = 500;
const int FAR_CM = 55;
const unsigned long NORMAL_TURN_MS = 1800;
const unsigned long FAR_TURN_MS = 2200;
const unsigned long ORANGE_LAST_EXTRA_MS = 300;

const unsigned long POST_TURN_SETTLE_MS = 300;
const unsigned long CORNER_LOOKBACK_MS = 500;

// Drive this long into the starting section after 12 corners, then stop
const unsigned long FINISH_DRIVE_MS = 1500;

// Corner counting protection and backups
const unsigned long LINE_MIN_GAP_MS = 600;
const unsigned long OTHER_LINE_GAP_MS = 800;
const int OTHER_CONFIRM_READINGS = 3;
const int FRONT_BACKUP_CM = 35;
const unsigned long MIN_TIME_BETWEEN_TURNS_MS = 1500;
const unsigned long PILLAR_BLOCKS_BACKUP_MS = 700;   // no front backup this soon after seeing a pillar

// =====================================================
// PILLARS (HuskyLens)
// =====================================================

// Which HuskyLens IDs are green and which are red (others are ignored)
const int GREEN_IDS[] = {1, 2, 3, 4};
const int RED_IDS[]   = {5, 7};

const int NUM_GREEN_IDS = sizeof(GREEN_IDS) / sizeof(GREEN_IDS[0]);
const int NUM_RED_IDS   = sizeof(RED_IDS) / sizeof(RED_IDS[0]);

const int CAMERA_CENTER_X = 320;       // x value straight ahead of the robot (screen is 640 wide)
const int MIN_BOX_HEIGHT = 15;         // ignore boxes smaller than this (noise)
const int SWITCH_MARGIN = 25;          // other pillar must be this much closer to switch target

const int REACT_BOTTOM = 200;          // pillars whose bottom is higher on screen than this are too far: ignored
const int NEAR_BOTTOM = 440;           // bottom value of a pillar that is very close

const int MIN_OFFSET = 120;            // how far to the side to keep a FAR pillar on screen (pixels)
const int MAX_OFFSET = 280;            // how far to the side to keep a CLOSE pillar on screen (pixels)
const float KX = 0.15;                 // steering degrees per pixel of error

const int PASS_BOTTOM = 360;           // a pillar lost while this close counts as passed
const unsigned long PASS_HOLD_MS = 300;    // drive straight this long after passing a pillar
const unsigned long TARGET_LOST_MS = 150;  // keep following a pillar this long if one frame misses it
const unsigned long CAMERA_INTERVAL_MS = 50;  // ask the HuskyLens this often

// =====================================================
// WALLS
// =====================================================

const int WALL_MAX_CM = 120;           // readings further than this are ignored
const int LANE_SIDE_CM = 40;           // distance to keep when only one wall is visible
const float KC = 0.6;                  // centering steering per cm
const int MAX_CENTER_CORRECTION = 15;

const int SAFETY_CM = 12;
const int DIAG_SAFETY_CM = 18;
const int SAFETY_CORRECTION = 15;

const int DANGER_CM = 8;
const int DIAG_DANGER_CM = 12;
const int DANGER_CORRECTION = 25;

// Ultrasonic timing
const unsigned long US_INTERVAL_MS = 20;
const unsigned long US_TIMEOUT_US = 7000;
const unsigned long US_CHECK_TIMEOUT_US = 30000;
const unsigned long US_VALID_FOR_MS = 300;

// =====================================================
// OTHER
// =====================================================

const unsigned long DEBUG_PRINT_INTERVAL_MS = 200;
const unsigned long START_IGNORE_MS = 500;
const int CONFIRM_READINGS = 2;
const int COUNTER_MAX = 1000;

// =====================================================
// STATE
// =====================================================

Servo steering;
HuskylensV2 huskylens;

// 0 = not selected, 1 = blue first (left turns), 2 = orange first (right turns)
int selectedColor = 0;

int cornerCount = 0;
bool lineArmed = true;
int nonSelectedReadings = 0;

unsigned long startTime = 0;
int lastRawColor = 0;
int sameColorCount = 0;

int currentAngle = STEER_CENTER;

// Ultrasonic state
bool usEnabled[NUM_US];
bool usFailed = false;
long usBuffer[NUM_US][3];
int usIndex[NUM_US];
long lastRaw[NUM_US];
long wallDist[NUM_US];
long lastValidDist[NUM_US];
long wallChange[NUM_US];
unsigned long lastValidTime[NUM_US];
unsigned long lastPingTime = 0;
int pingStep = 0;

unsigned long lastTurnEndTime = 0;
unsigned long lastDebugPrint = 0;

// Camera / pillar state
bool cameraOK = false;
unsigned long lastCameraRead = 0;
unsigned long lastCameraTry = 0;

int targetColor = 0;            // 0 = none, 1 = green, 2 = red
int targetX = 0;
int targetBottom = 0;
unsigned long targetLastSeen = 0;
unsigned long lastPillarSeenTime = 0;

bool passHolding = false;
unsigned long passHoldStart = 0;

// =====================================================
// LED
// =====================================================

void updateLed() {
  digitalWrite(LED_PIN, (usFailed || !cameraOK) ? HIGH : LOW);
}

// =====================================================
// MOTOR CONTROL
// =====================================================

void driveForward(int speedValue) {
  digitalWrite(MOTOR_IN2, HIGH);
  analogWrite(MOTOR_IN1, 255 - speedValue);
}

void stopMotor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
}

void brakeMotor() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, HIGH);
  delay(300);
  stopMotor();
}

void kickStart() {
  driveForward(KICK_SPEED);
  delay(KICK_TIME_MS);
  driveForward(NORMAL_SPEED);
}

// =====================================================
// STEERING CHECK AT STARTUP
// =====================================================

void moveSteeringTo(int target) {
  while (currentAngle != target) {
    if (currentAngle < target) currentAngle++;
    else currentAngle--;

    steering.write(currentAngle);
    delay(10);
  }
  delay(300);
}

void calibrateSteering() {
  Serial.println("STEERING CHECK: LEFT");
  moveSteeringTo(STEER_LEFT);

  Serial.println("STEERING CHECK: RIGHT");
  moveSteeringTo(STEER_RIGHT);

  Serial.println("STEERING CHECK: CENTER");
  moveSteeringTo(STEER_CENTER);

  Serial.println("STEERING READY");
}

// =====================================================
// ULTRASONIC SENSORS
// =====================================================

long pingOnce(int trigPin, int echoPin, unsigned long timeoutUs) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, timeoutUs);

  if (duration == 0) {
    return 999;
  }

  return duration / 58;
}

void resetUltrasonic() {
  for (int s = 0; s < NUM_US; s++) {
    usBuffer[s][0] = 999;
    usBuffer[s][1] = 999;
    usBuffer[s][2] = 999;
    lastRaw[s] = 999;
    wallDist[s] = 999;
    wallChange[s] = 0;
  }
}

void checkUltrasonicSensors() {
  usFailed = false;

  for (int s = 0; s < NUM_US; s++) {
    usIndex[s] = 0;
    lastValidDist[s] = 999;
    lastValidTime[s] = 0;

    if (!US_ALLOWED[s]) {
      usEnabled[s] = false;
      Serial.print("SENSOR ");
      Serial.print(US_NAMES[s]);
      Serial.println(": TURNED OFF IN CODE");
      continue;
    }

    int goodReadings = 0;
    for (int i = 0; i < 5; i++) {
      long d = pingOnce(US_TRIG[s], US_ECHO[s], US_CHECK_TIMEOUT_US);
      if (d >= 2 && d <= 400) {
        goodReadings++;
      }
      delay(60);
    }

    usEnabled[s] = (goodReadings >= 3);

    Serial.print("SENSOR ");
    Serial.print(US_NAMES[s]);
    Serial.print(": ");
    Serial.print(goodReadings);
    Serial.print("/5 good -> ");

    if (usEnabled[s]) {
      Serial.println("OK");
    } else {
      Serial.println("FAILED - NOT USED THIS RUN");
      usFailed = true;
    }
  }

  updateLed();
}

long median3(long a, long b, long c) {
  if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
  if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
  return c;
}

void updateDistances() {
  if (millis() - lastPingTime < US_INTERVAL_MS) {
    return;
  }
  lastPingTime = millis();

  int s = -1;
  for (int tries = 0; tries < PING_STEPS; tries++) {
    int candidate = PING_ORDER[pingStep];
    pingStep = (pingStep + 1) % PING_STEPS;
    if (usEnabled[candidate]) {
      s = candidate;
      break;
    }
  }
  if (s < 0) {
    return;
  }

  long raw = pingOnce(US_TRIG[s], US_ECHO[s], US_TIMEOUT_US);
  long filtered;

  if (s == US_RIGHT || s == US_LEFT) {
    usBuffer[s][usIndex[s]] = raw;
    usIndex[s] = (usIndex[s] + 1) % 3;
    filtered = median3(usBuffer[s][0], usBuffer[s][1], usBuffer[s][2]);
  } else {
    filtered = (raw > lastRaw[s]) ? raw : lastRaw[s];
    lastRaw[s] = raw;
  }

  if (filtered < 999) {
    if (lastValidDist[s] < 999 && millis() - lastValidTime[s] <= US_VALID_FOR_MS) {
      wallChange[s] = filtered - lastValidDist[s];
    } else {
      wallChange[s] = 0;
    }

    wallDist[s] = filtered;
    lastValidDist[s] = filtered;
    lastValidTime[s] = millis();
  } else {
    wallDist[s] = 999;
    wallChange[s] = 0;
  }
}

long openSpace(int s) {
  long d = wallDist[s];
  if (d > 150) d = 150;
  return d;
}

// =====================================================
// CAMERA (HUSKYLENS)
// =====================================================

bool isGreen(int id) {
  for (int i = 0; i < NUM_GREEN_IDS; i++) {
    if (GREEN_IDS[i] == id) return true;
  }
  return false;
}

bool isRed(int id) {
  for (int i = 0; i < NUM_RED_IDS; i++) {
    if (RED_IDS[i] == id) return true;
  }
  return false;
}

void tryConnectCamera() {
  if (huskylens.begin(Serial1)) {
    huskylens.switchAlgorithm(ALGORITHM_COLOR_RECOGNITION);
    cameraOK = true;
    Serial.println("HUSKYLENS CONNECTED");
  } else {
    cameraOK = false;
  }
  updateLed();
}

void clearPillarTarget() {
  targetColor = 0;
  passHolding = false;
}

// Reads the camera and picks the closest red or green pillar
void updateCamera(int rawColor) {
  if (!cameraOK) return;
  if (rawColor != 0) return;   // a line is under the color sensor: don't slow down line reading
  if (millis() - lastCameraRead < CAMERA_INTERVAL_MS) return;
  lastCameraRead = millis();

  int count = huskylens.getResult(ALGORITHM_COLOR_RECOGNITION);

  int greenBottom = -1, greenX = 0;
  int redBottom = -1, redX = 0;

  if (count > 0) {
    while (huskylens.available(ALGORITHM_COLOR_RECOGNITION)) {
      Result *r = static_cast<Result *>(
          huskylens.popCachedResult(ALGORITHM_COLOR_RECOGNITION));

      if (r->height < MIN_BOX_HEIGHT) continue;           // too small: noise

      int bottom = r->yCenter + r->height / 2;            // where the pillar meets the floor
      if (bottom < REACT_BOTTOM) continue;                // too far away: ignore for now

      if (isGreen(r->ID) && bottom > greenBottom) {
        greenBottom = bottom;
        greenX = r->xCenter;
      } else if (isRed(r->ID) && bottom > redBottom) {
        redBottom = bottom;
        redX = r->xCenter;
      }
    }
  }

  // Choose a target (stick with the current one unless the other is clearly closer)
  int newTarget = 0;
  if (greenBottom >= 0 && redBottom < 0) {
    newTarget = 1;
  } else if (redBottom >= 0 && greenBottom < 0) {
    newTarget = 2;
  } else if (greenBottom >= 0 && redBottom >= 0) {
    if (targetColor == 1) {
      newTarget = (redBottom > greenBottom + SWITCH_MARGIN) ? 2 : 1;
    } else if (targetColor == 2) {
      newTarget = (greenBottom > redBottom + SWITCH_MARGIN) ? 1 : 2;
    } else {
      newTarget = (redBottom > greenBottom) ? 2 : 1;
    }
  }

  if (newTarget != 0) {
    targetColor = newTarget;
    targetX = (newTarget == 1) ? greenX : redX;
    targetBottom = (newTarget == 1) ? greenBottom : redBottom;
    targetLastSeen = millis();
    lastPillarSeenTime = millis();
    passHolding = false;
  } else if (targetColor != 0 && millis() - targetLastSeen > TARGET_LOST_MS) {
    // The pillar is gone from view
    if (targetBottom >= PASS_BOTTOM) {
      // It was close, so it's now beside the robot: we passed it
      passHolding = true;
      passHoldStart = millis();
      if (DEBUG) {
        Serial.println();
        Serial.println(targetColor == 2 ? ">>> PASSED RED" : ">>> PASSED GREEN");
      }
    }
    targetColor = 0;
  }
}

// Steering angle that keeps the pillar on the correct side of the screen
int pillarSteerAngle() {
  int b = constrain(targetBottom, REACT_BOTTOM, NEAR_BOTTOM);
  long offset = map(b, REACT_BOTTOM, NEAR_BOTTOM, MIN_OFFSET, MAX_OFFSET);

  // Red: keep the pillar on the LEFT of the screen (robot passes on its right)
  // Green: keep the pillar on the RIGHT of the screen (robot passes on its left)
  int goalX = (targetColor == 2) ? (CAMERA_CENTER_X - offset) : (CAMERA_CENTER_X + offset);

  int angle = STEER_CENTER + (int)(KX * (targetX - goalX));
  return constrain(angle, STEER_LEFT, STEER_RIGHT);
}

// =====================================================
// STEERING ON THE STRAIGHTS (priority order)
// =====================================================

void steerOnStraight() {
  long R  = wallDist[US_RIGHT];
  long L  = wallDist[US_LEFT];
  long FR = wallDist[US_FRONT_RIGHT];
  long FL = wallDist[US_FRONT_LEFT];

  // 1. Emergency: about to hit something
  bool dangerRight = (R < 999 && R <= DANGER_CM) || (FR < 999 && FR <= DIAG_DANGER_CM);
  bool dangerLeft  = (L < 999 && L <= DANGER_CM) || (FL < 999 && FL <= DIAG_DANGER_CM);

  if (dangerRight && !dangerLeft) {
    steering.write(STEER_CENTER - DANGER_CORRECTION);
    return;
  }
  if (dangerLeft && !dangerRight) {
    steering.write(STEER_CENTER + DANGER_CORRECTION);
    return;
  }

  // 2. A pillar is being followed: steer around it on the correct side
  if (targetColor != 0) {
    steering.write(pillarSteerAngle());
    return;
  }

  // 3. Just passed a pillar, or just finished a turn: drive straight for a moment
  if (passHolding && millis() - passHoldStart < PASS_HOLD_MS) {
    steering.write(STEER_CENTER);
    return;
  }
  passHolding = false;

  if (millis() - lastTurnEndTime <= POST_TURN_SETTLE_MS) {
    steering.write(STEER_CENTER);
    return;
  }

  // 4. Too close to a wall
  bool closeRight = (R < 999 && R <= SAFETY_CM) || (FR < 999 && FR <= DIAG_SAFETY_CM);
  bool closeLeft  = (L < 999 && L <= SAFETY_CM) || (FL < 999 && FL <= DIAG_SAFETY_CM);

  if (closeRight && !closeLeft) {
    steering.write(STEER_CENTER - SAFETY_CORRECTION);
    return;
  }
  if (closeLeft && !closeRight) {
    steering.write(STEER_CENTER + SAFETY_CORRECTION);
    return;
  }

  // 5. Nothing special: drive in the middle of the corridor
  bool rOk = (R < WALL_MAX_CM);
  bool lOk = (L < WALL_MAX_CM);
  float correction = 0;

  if (rOk && lOk) {
    correction = KC * (R - L) / 2.0;        // R bigger = robot is left of center = steer right
  } else if (rOk) {
    correction = KC * (R - LANE_SIDE_CM);
  } else if (lOk) {
    correction = KC * (LANE_SIDE_CM - L);
  }

  if (correction > MAX_CENTER_CORRECTION) correction = MAX_CENTER_CORRECTION;
  if (correction < -MAX_CENTER_CORRECTION) correction = -MAX_CENTER_CORRECTION;

  steering.write(STEER_CENTER + (int)correction);
}

// =====================================================
// COLOR SENSOR (corner lines)
// =====================================================

unsigned long readColorChannel(bool s2, bool s3) {
  digitalWrite(S2, s2);
  digitalWrite(S3, s3);

  delayMicroseconds(300);

  unsigned long pulse = pulseIn(COLOR_OUT, LOW, 5000);

  if (pulse == 0) {
    return 0;
  }

  return 500000UL / pulse;
}

// Returns: 0 = other, 1 = blue, 2 = orange
int detectColor() {
  unsigned long R = readColorChannel(LOW, LOW);
  unsigned long G = readColorChannel(HIGH, HIGH);
  unsigned long B = readColorChannel(LOW, HIGH);

  int result = 0;

  // Blue range
  if (
      R >= 15000 && R < 26500 &&
      G >= 15000 && G <= 26000 &&
      B >= 22000 && B <= 45000
     ) {
    result = 1;
  }
  // Orange range
  else if (
      R >= 26500 && R <= 57000 &&
      (
        (G >= 19500 && G <= 26000 &&
         B >= 22500 && B <= 47000)
        ||
        (G > 26000 && G <= 35000 &&
         B >= 22500 && B <= 30000)
      )
     ) {
    result = 2;
  }

  if (DEBUG) {
    if (millis() - lastDebugPrint >= DEBUG_PRINT_INTERVAL_MS) {
      lastDebugPrint = millis();

      Serial.print("L: ");
      Serial.print(wallDist[US_LEFT]);
      Serial.print(" FL: ");
      Serial.print(wallDist[US_FRONT_LEFT]);
      Serial.print(" F: ");
      Serial.print(wallDist[US_FRONT]);
      Serial.print(" FR: ");
      Serial.print(wallDist[US_FRONT_RIGHT]);
      Serial.print(" R: ");
      Serial.print(wallDist[US_RIGHT]);

      Serial.print("  | PILLAR: ");
      if (targetColor == 2) Serial.print("RED");
      else if (targetColor == 1) Serial.print("GREEN");
      else Serial.print("none");
      if (targetColor != 0) {
        Serial.print(" x: ");
        Serial.print(targetX);
        Serial.print(" bottom: ");
        Serial.print(targetBottom);
      }

      Serial.print("  | FLOOR: ");
      if (result == 1) Serial.println("BLUE");
      else if (result == 2) Serial.println("ORANGE");
      else Serial.println("other");

    } else if (result == 1) {
      Serial.print('b');
    } else if (result == 2) {
      Serial.print('o');
    }
  }

  return result;
}

// =====================================================
// TURNS
// =====================================================

void orangeTurn(bool backupTurn) {
  long d = 999;
  if (millis() - lastValidTime[US_RIGHT] <= CORNER_LOOKBACK_MS) {
    d = lastValidDist[US_RIGHT];
  }

  unsigned long waitTime = 0;
  if (!backupTurn && d < TURN_CLEARANCE_CM) {
    waitTime = (TURN_CLEARANCE_CM - d) * WAIT_MS_PER_CM;
    if (waitTime > MAX_WAIT_MS) waitTime = MAX_WAIT_MS;
  }

  unsigned long turnTime = NORMAL_TURN_MS;
  if (d > FAR_CM) turnTime = FAR_TURN_MS;
  if (cornerCount == 12) turnTime += ORANGE_LAST_EXTRA_MS;

  Serial.println();
  Serial.print(">>> WALL BEFORE CORNER: ");
  Serial.print(d);
  Serial.print(" cm   WAIT: ");
  Serial.print(waitTime);
  Serial.print(" ms   TURN: ");
  Serial.print(turnTime);
  Serial.println(" ms");

  if (waitTime > 0) {
    steering.write(STEER_CENTER);
    driveForward(TURN_SPEED);
    delay(waitTime);
  }

  Serial.println(">>> TURNING RIGHT");
  steering.write(STEER_RIGHT);
  driveForward(TURN_SPEED);
  delay(turnTime);

  steering.write(STEER_CENTER);
  driveForward(NORMAL_SPEED);
}

void turnForDirection(bool backupTurn) {
  if (selectedColor == 2) {
    orangeTurn(backupTurn);
  } else {
    Serial.println();
    Serial.println(">>> TURNING LEFT");
    steering.write(STEER_LEFT);
    driveForward(TURN_SPEED);

    if (cornerCount == 1) {
      delay(FIRST_TURN_TIME_MS);
    } else if (cornerCount == 12) {
      delay(LAST_TURN_TIME_MS);
    } else {
      delay(TURN_TIME_MS);
    }

    steering.write(STEER_CENTER);
    driveForward(NORMAL_SPEED);
  }

  lastTurnEndTime = millis();
  resetUltrasonic();
  clearPillarTarget();

  Serial.println(">>> TURN COMPLETE");
}

void finishRun();

void doCorner(bool backupTurn) {
  lineArmed = false;
  nonSelectedReadings = 0;
  cornerCount++;

  Serial.println();
  Serial.print("CORNER: ");
  Serial.print(cornerCount);
  Serial.println(" / 12");

  turnForDirection(backupTurn);

  if (cornerCount >= 12) {
    finishRun();
  }
}

// =====================================================
// FINISH: drive into the starting section and stop
// =====================================================

void finishRun() {
  Serial.println(">>> 12 CORNERS COUNTED");
  Serial.println(">>> DRIVING INTO THE STARTING SECTION");

  unsigned long finishStart = millis();

  while (millis() - finishStart < FINISH_DRIVE_MS) {
    updateDistances();
    updateCamera(0);       // still avoid pillars on the way in
    steerOnStraight();
    driveForward(FINISH_SPEED);
  }

  brakeMotor();
  steering.write(STEER_CENTER);

  Serial.println(">>> STOPPED");

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
  Serial1.begin(115200);   // HuskyLens

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);   // stays on until everything is ready

  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  stopMotor();

  steering.attach(SERVO_PIN);
  steering.write(STEER_CENTER);
  currentAngle = STEER_CENTER;
  delay(500);

  calibrateSteering();

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(COLOR_OUT, INPUT);

  // 20% output-frequency scaling
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  for (int s = 0; s < NUM_US; s++) {
    pinMode(US_TRIG[s], OUTPUT);
    pinMode(US_ECHO[s], INPUT);
    digitalWrite(US_TRIG[s], LOW);
  }

  checkUltrasonicSensors();
  resetUltrasonic();

  // First try to connect to the HuskyLens (it keeps trying while waiting for the button)
  tryConnectCamera();

  pinMode(START_BUTTON, INPUT_PULLUP);

  cornerCount = 0;
  selectedColor = 0;
  lineArmed = true;
  nonSelectedReadings = 0;
  clearPillarTarget();

  Serial.println("READY - PRESS BUTTON");

  // Wait for the button, and keep trying to connect the camera if it isn't connected yet
  while (digitalRead(START_BUTTON) == HIGH) {
    steering.write(STEER_CENTER);

    if (!cameraOK && millis() - lastCameraTry > 500) {
      lastCameraTry = millis();
      tryConnectCamera();
    }
  }

  delay(100);

  while (digitalRead(START_BUTTON) == LOW) {
    steering.write(STEER_CENTER);
  }

  delay(300);

  if (!cameraOK) {
    Serial.println("WARNING: STARTING WITHOUT CAMERA - PILLARS WILL NOT BE AVOIDED");
  }

  Serial.println("START");
  Serial.println("CORNER 0 / 12");

  steering.write(STEER_CENTER);
  kickStart();

  startTime = millis();
  lastTurnEndTime = millis();
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {
  updateDistances();

  int rawColor = detectColor();

  updateCamera(rawColor);

  // Count how many times in a row the same floor color was read
  if (rawColor == lastRawColor) {
    if (sameColorCount < COUNTER_MAX) sameColorCount++;
  } else {
    lastRawColor = rawColor;
    sameColorCount = 1;
  }

  int color = 0;
  if (rawColor != 0 && sameColorCount >= CONFIRM_READINGS) {
    color = rawColor;
  }

  bool startIgnore = (millis() - startTime < START_IGNORE_MS);
  if (startIgnore) {
    color = 0;
  }

  // First line decides the direction
  if (selectedColor == 0 && (color == 1 || color == 2)) {
    selectedColor = color;

    Serial.println();
    if (selectedColor == 1) {
      Serial.println("BLUE FIRST: LEFT TURNS / COUNT BLUE (ORANGE AS BACKUP)");
    } else {
      Serial.println("ORANGE FIRST: RIGHT TURNS / COUNT ORANGE (BLUE AS BACKUP)");
    }
  }

  int otherColor = 0;
  if (selectedColor == 1) otherColor = 2;
  if (selectedColor == 2) otherColor = 1;

  bool lineGapOk = (cornerCount == 0) || (millis() - lastTurnEndTime > LINE_MIN_GAP_MS);

  // 1. Turn line
  if (selectedColor != 0 && color == selectedColor && lineGapOk) {
    nonSelectedReadings = 0;
    if (lineArmed) {
      doCorner(false);
    }
    return;
  }

  // 2. Missed the turn line, but saw the other color of the same corner
  if (otherColor != 0 &&
      !startIgnore &&
      rawColor == otherColor &&
      sameColorCount >= OTHER_CONFIRM_READINGS &&
      millis() - lastTurnEndTime > OTHER_LINE_GAP_MS) {
    Serial.println();
    Serial.println(">>> TURN LINE MISSED, SAW THE OTHER COLOR: BACKUP TURN");
    doCorner(true);
    return;
  }

  // 3. Wall straight ahead and no line seen (but NOT if a pillar was just seen,
  //    because then the front sensor is probably seeing the pillar)
  if (wallDist[US_FRONT] <= FRONT_BACKUP_CM &&
      millis() - lastTurnEndTime > MIN_TIME_BETWEEN_TURNS_MS &&
      millis() - lastPillarSeenTime > PILLAR_BLOCKS_BACKUP_MS &&
      targetColor == 0) {

    if (selectedColor == 0) {
      long rightOpen = openSpace(US_RIGHT) + openSpace(US_FRONT_RIGHT);
      long leftOpen  = openSpace(US_LEFT)  + openSpace(US_FRONT_LEFT);
      selectedColor = (rightOpen > leftOpen) ? 2 : 1;

      Serial.println();
      Serial.println(selectedColor == 2 ?
        "FIRST LINE MISSED: RIGHT SIDE OPEN -> RIGHT TURNS" :
        "FIRST LINE MISSED: LEFT SIDE OPEN -> LEFT TURNS");
    }

    Serial.println(">>> WALL AHEAD, NO LINE SEEN: BACKUP TURN");
    doCorner(true);
    return;
  }

  // 4. Normal driving
  if (nonSelectedReadings < COUNTER_MAX) nonSelectedReadings++;
  if (nonSelectedReadings >= 20) {
    lineArmed = true;
  }

  steerOnStraight();
  driveForward(NORMAL_SPEED);
}
