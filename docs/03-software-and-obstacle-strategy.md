# Software Architecture and Challenge Strategy

## Software overview

The Invader uses a separate Arduino sketch for each challenge. Both run on the Arduino Mega 2560 and control the DRV8871 drive motor and front steering servo. The Open Challenge focuses on completing three laps and stopping in the finish section. The Obstacle Challenge also uses a HuskyLens 2 and ultrasonic sensors to navigate around obstacles.

The first blue or orange corner line determines the driving direction. Blue selects anticlockwise travel and left turns; orange selects clockwise travel and right turns. The vehicle then counts 12 corners, corresponding to four corners on each of three laps.

## Open Challenge strategy

The Open Challenge sketch reads the downward-facing TCS3200 color sensor and the ultrasonic sensors. Color readings identify the corner lines, while distance readings provide information about the surrounding walls.

The program follows this sequence:

`Wait for button → Kick start → Read sensors → Select direction → Count and turn at corners → Drive into finish section → Brake and stop`

### Direction and corner counting

Before the first line, the driving direction is unset. The first accepted blue line selects left turns; the first accepted orange line selects right turns. For the remainder of the run, the program counts lines of the selected color.

After an accepted line, the program temporarily disarms line detection. It requires 20 consecutive readings outside the selected color before another line can trigger a turn. This reduces the chance of counting one wide line more than once.

| Corner | Turn duration |
|---|---:|
| First | 2,200 ms |
| Second through eleventh | 1,800 ms |
| Twelfth | 2,100 ms |

The steering commands are 82° for straight travel, 55° for left turns, and 109° for right turns. These are servo commands, not measured wheel or heading angles.

### Motor and finish control

At the start, the motor receives PWM 255 for 150 ms to get the vehicle moving. It then runs at a normal command of 200. The DRV8871 receives its motor commands through D5 and D6.

After the twelfth turn, the vehicle drives straight for 1,500 ms, brakes for 300 ms, and remains stopped until reset.

### Color reading

For each decision, the TCS3200 provides one red, one green, and one blue reading. The program waits 300 µs after selecting a channel and allows up to 5,000 µs for its pulse. The Open Challenge color ranges and calibration procedure are documented in [Power and Sensor Architecture](02-power-and-sensor-architecture.md).

Serial output at 115200 baud reports sensor readings and corner events for calibration and debugging.

## Obstacle Challenge strategy

The Obstacle Challenge sketch combines three sources of information:

- **HuskyLens 2:** Identifies red and green obstacles and reports their positions in the camera image.
- **Five ultrasonic sensors:** Measure nearby walls and objects for steering corrections and collision avoidance.
- **TCS3200 color sensor:** Identifies corner lines and determines the driving direction.

The vehicle aims to pass **red obstacles on the right** and **green obstacles on the left**. When it detects a relevant obstacle, it selects a target and steers to keep that obstacle on the appropriate side of the camera image. If both colors are visible, the code uses their image positions to choose which obstacle to address first.

On a straight section, the steering logic gives priority to immediate collision avoidance, then obstacle navigation, then wall clearance and lane centering. After passing an obstacle or completing a turn, the vehicle briefly holds a straight heading before resuming normal corrections.

The code uses the selected floor-line color to count corners. It also contains backup turn conditions for cases where the expected line is missed, including detection of the other line color or a nearby wall ahead. Clockwise corner timing can be adjusted using the measured distance to the inner wall.

After 12 corners, the sketch drives into the starting section and stops. **Parallel parking is not implemented in this sketch.**

## Control functions

| Function | Responsibility |
|---|---|
| `setup()` | Initialize the motor, steering, sensors, camera connection, and start button |
| `driveForward()` | Send a speed command to the DRV8871 |
| `stopMotor()` / `brakeMotor()` | Stop or briefly brake the drive motor |
| `readColorChannel()` / `detectColor()` | Read and classify the floor color |
| `updateDistances()` | Refresh ultrasonic distance readings |
| `updateCamera()` | Read HuskyLens 2 detections and select an obstacle target |
| `steerOnStraight()` | Choose a steering correction using obstacles and wall distances |
| `turnForDirection()` | Carry out a left or right corner turn |
| `finishRun()` | Drive into the finish section, brake, and hold the stopped state |
| `loop()` | Repeatedly update sensors, identify corners, and control movement |

The two sketches share the same overall vehicle hardware, but their steering and sensor settings should be checked in the specific sketch being uploaded.

## Known limitations and checks

- A false corner-line reading can cause an early turn, while a completely missed line can leave the corner count short. The Obstacle Challenge sketch includes backup turn conditions, but these do not guarantee recovery in every layout.
- Color thresholds depend on the mat, sensor height, and lighting. Check the serial readings at the venue.
- Timed turns can vary with traction, battery condition, and starting position. Full runs in both directions are needed to evaluate their reliability.
- Camera detections must be checked while the vehicle is moving. A powered camera or a working connection alone does not demonstrate reliable obstacle passes.
- Parking remains a separate development task.
