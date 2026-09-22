# Power and Sensor Architecture

## Electronics overview

The vehicle uses an Arduino Mega as the main controller.

| Component | Role / connection |
|---|---|
| Arduino Mega | Main controller |
| L298N motor-driver module | Controls drive-motor direction and PWM |
| Steering servo | Controls the front steering linkage through D7 |
| Ground color sensor | Detects field color beneath the vehicle |
| 5 ultrasonic sensors | Mounted left, front-left, front, front-right and right |
| HuskyLens 1 | Vision sensor connected through RX1 / TX1 |
| Studica battery | 12 V nominal, 3000 mAh |
| Start button | D34 to GND using `INPUT_PULLUP` |
| Power switch | Main power control |

### Arduino connections

| Device / signal | Mega connection |
|---|---|
| Steering servo signal | D7 |
| L298N ENA / drive PWM | D5 |
| L298N IN1 / IN2 | D32 / D33 |
| Start button | D34 to GND |
| Color sensor S0 / S1 | D35 / D36 |
| Color sensor S2 / S3 | D37 / D38 |
| Color sensor OUT | D39 |
| Left ultrasonic TRIG / ECHO | D22 / D23 |
| Front-left ultrasonic TRIG / ECHO | D24 / D25 |
| Front ultrasonic TRIG / ECHO | D26 / D27 |
| Front-right ultrasonic TRIG / ECHO | D28 / D29 |
| Right ultrasonic TRIG / ECHO | D30 / D31 |
| HuskyLens TX to Mega RX1 | D19 / RX1 |
| HuskyLens RX to Mega TX1 | D18 / TX1 |

## Power distribution

The battery has a nominal voltage of 12 V and a rated capacity of 3000 mAh, giving a calculated nominal energy of:

`12 V × 3 Ah = 36 Wh`

The confirmed power routes are:

- Battery to the L298N motor driver
- Battery to the Arduino Mega barrel jack
- Arduino to HuskyLens

With power entering through the Mega barrel jack, the Arduino's onboard regulator supplies the 5 V rail. HuskyLens draws power from this rail.

## Sensor placement

| Sensor | Position / role |
|---|---|
| Ground color sensor | Mounted below the vehicle, 27.5 mm above the floor |
| Left ultrasonic | Left side of the vehicle |
| Front-left ultrasonic | Front-left orientation |
| Front ultrasonic | Front-facing |
| Front-right ultrasonic | Front-right orientation |
| Right ultrasonic | Right side of the vehicle |
| HuskyLens 1 | Onboard vision sensor with RX1 / TX1 connection |

The Open Challenge program reads the ground color sensor.

The ultrasonic sensors and HuskyLens are mounted on the vehicle, but neither submitted challenge sketch reads them.

## Ground color sensor calibration

The ground sensor uses a TCS3200 frequency-output interface.

The program sets:

- `S0 = HIGH`
- `S1 = LOW`

This selects 20% output scaling.

The color filters are selected using S2 and S3:

| Channel | S2 | S3 |
|---|---|---|
| Red | LOW | LOW |
| Green | HIGH | HIGH |
| Blue | LOW | HIGH |

After selecting a filter, the program waits 3 ms and measures a LOW pulse with a 30,000 µs timeout.

A successful sample returns:

`500000 / pulse duration`

For each color channel, three samples are taken and averaged.

### Color thresholds

| Condition | BLUE | ORANGE |
|---|---:|---:|
| Red channel R | 10000–19000 | 17500–25500 |
| Green channel G | 9000–14500 | 12000–16500 |
| Blue channel B | 11000–20500 | 13000–19000 |
| Additional test | `B + 3000 ≥ R` | `R > B + 3000` |

The program checks BLUE first. If the reading does not meet the BLUE condition, it checks ORANGE.

The calibration was performed with the sensor mounted 2.75 cm above the floor using the sensor's LEDs. Thresholds were selected from readings gathered during testing. Serial output displays the measured R, G and B values and the resulting classification.

## Failure points and handling

If no color-sensor pulse arrives before the 30,000 µs timeout, the sample returns zero.

To reduce repeated triggers from one blue line, the program uses an armed-state system. After a BLUE detection triggers a turn, the system does not accept another BLUE trigger until three consecutive non-blue readings have been detected.

A wiring failure also occurred during testing when an ultrasonic sensor wire entered a wheel's moving path and was cut. The damaged wiring was replaced and the cables were secured with zip ties to keep them away from moving parts.

During the same repair, the Arduino power cord broke. The team soldered the power cord and insulated the repaired connection with heat-shrink tubing.
