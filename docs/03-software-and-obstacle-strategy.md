# Software Architecture and Challenge Strategy

## Software overview

The vehicle uses separate Arduino sketches for the Open Challenge and Obstacle Challenge. Both programs run on an Arduino Mega and are uploaded using the Arduino IDE. The Open Challenge sketch uses the Arduino `Servo` library.

### Open Challenge functions

| Function | Responsibility |
|---|---|
| `setup()` | Initialize pins and wait for the start-button sequence |
| `driveForward()` | Set forward direction and apply the PWM command |
| `stopMotor()` | Set PWM to zero and both direction outputs LOW |
| `readColorChannel()` | Select a color filter and measure one pulse |
| `averageColor()` | Average three returned channel measurements |
| `detectColor()` | Classify the color channels and print diagnostics |
| `turnLeft()` | Apply the timed turn, then restore straight commands |
| `finishRun()` | Run the final straight and hold the stopped state |
| `loop()` | Manage classification, re-arming and corner counting |

## Module relationships

In the Open Challenge, the ground color sensor provides red, green and blue measurements. The program averages the readings and classifies the detected surface as BLUE, ORANGE or OTHER.

When BLUE is detected while the system is armed, the program increments the corner counter and performs a timed left turn. Three consecutive non-blue readings re-arm the system for the next BLUE detection.

The control flow is:

`Wait for start → Read color → Classify → BLUE detected? → Count corner → Turn left → Check corner count → Continue or finish`

After 12 accepted BLUE detections, the vehicle performs its final straight movement and stops.

## Open Challenge strategy

The Open Challenge uses the ground color sensor to detect blue floor markings.

A BLUE detection triggers a left turn only when `blueArmed` is true. After a BLUE detection is accepted, the system is disarmed until three consecutive non-blue readings are received.

The vehicle counts accepted blue-line detections as corners:

`4 corners per lap × 3 laps = 12 corners`

After the twelfth accepted BLUE detection, the vehicle completes the normal left turn and enters the finish routine.

The finish routine:

- Centers the steering at 82°
- Drives forward at PWM 140 for 1250 ms
- Stops the motor
- Holds the steering at 82° until reset

## Obstacle Challenge strategy

The Obstacle Challenge uses two predefined sequences of timed movements:

`runLap()` followed by `runLap2()`

The program uses forward movements, gentle steering corrections and stronger left corner commands. Each movement runs for a fixed duration before the next command begins.

The submitted program does not use the HuskyLens, ultrasonic sensors or ground color sensor during the Obstacle Challenge.

### Movement commands

| Function | Servo | PWM | Purpose |
|---|---:|---:|---|
| `FORWARD(ms)` | 82° | 150 | Straight movement |
| `LEFT(ms)` | 65° | 150 | Gentle left correction |
| `RIGHT(ms)` | 92° | 150 | Gentle right correction |
| `CORNER_LEFT(ms)` | 55° | 220 | Stronger left corner command |
| `STOP()` | 82° | 0 | Motor stopped and steering centered |

The first sequence contains 18 timed movements totaling 9.09 seconds of motion delays.

The second sequence contains 16 timed movements totaling 8.59 seconds of motion delays.

After both sequences are completed, the vehicle stops and remains stopped until the controller is reset.

## Control and tuning

### Open Challenge

| Setting | Current value |
|---|---:|
| Straight steering | 82° |
| Left steering | 55° |
| Straight PWM | 140 / 255 |
| Turn PWM | 155 / 255 |
| Turn duration | 1000 ms |
| Finish forward duration | 1250 ms |

The first two Open Challenge attempts were used for steering adjustment. The following three complete-course attempts each completed three laps.

### Color classification

| Condition | BLUE | ORANGE |
|---|---:|---:|
| Red channel R | 10000–19000 | 17500–25500 |
| Green channel G | 9000–14500 | 12000–16500 |
| Blue channel B | 11000–20500 | 13000–19000 |
| Additional test | `B + 3000 >= R` | `R > B + 3000` |

For each color channel, three readings are averaged before classification.

The thresholds were selected from readings collected during testing with the ground color sensor mounted 2.75 cm above the floor.

## Failure handling

If no color-sensor pulse is received within the 30,000 µs timeout, the reading function returns zero.

The three-reading re-arm rule reduces repeated BLUE triggers while the sensor remains over the same blue line.

At the end of either challenge program, the motor is stopped and the steering is centered.

The Obstacle Challenge uses timed movements without sensor feedback, so its path depends on the starting pose, battery condition, traction and configured obstacle layout.
