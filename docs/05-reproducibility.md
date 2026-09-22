# Reproducibility and Setup

## Required parts

See the component list in:

[`../hardware/bill-of-materials.md`](../hardware/bill-of-materials.md)

## Wiring

Use the following Arduino Mega connections:

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

The D34 start button uses `INPUT_PULLUP`. Pressing the button connects D34 to GND and produces a LOW reading.

Confirmed power routes:

- Battery to L298N motor driver
- Battery to Arduino Mega barrel jack
- Arduino to HuskyLens

## Software dependencies

- Arduino IDE
- Arduino Mega
- Arduino `Servo` library

The Open Challenge and Obstacle Challenge are stored as separate Arduino sketches.

## Build and upload

1. Select the required challenge sketch.
2. Place it in a matching Arduino sketch folder.
3. Open the sketch in Arduino IDE.
4. Make sure the `Servo` library is available.
5. Connect the hardware using the documented Mega pin assignments.
6. Connect the D34 start button to GND.
7. Check polarity and supply connections before powering the vehicle.
8. Select the Arduino Mega board and its connected port.
9. Compile and upload the sketch.
10. Position the vehicle for the route represented by the selected sketch.
11. Press and release the start button.
12. The program waits 300 ms after release before beginning forward motion.

For Open Challenge diagnostics, open the Serial Monitor at `115200` baud.

## Configuration

### Open Challenge

| Parameter | Value |
|---|---:|
| Steering center | 82° |
| Left steering | 55° |
| Straight PWM | 140 / 255 |
| Turn PWM | 155 / 255 |
| Turn duration | 1000 ms |
| Finish forward duration | 1250 ms |

### Ground color sensor

The TCS3200 is configured with:

- `S0 = HIGH`
- `S1 = LOW`
- 20% output scaling

Filter selection:

| Channel | S2 | S3 |
|---|---|---|
| Red | LOW | LOW |
| Green | HIGH | HIGH |
| Blue | LOW | HIGH |

Each filter selection is followed by a 3 ms delay.

The program measures a LOW pulse with a `30,000 µs` timeout.

Three readings are averaged for each color channel.

### Color thresholds

| Condition | BLUE | ORANGE |
|---|---:|---:|
| Red channel R | 10000–19000 | 17500–25500 |
| Green channel G | 9000–14500 | 12000–16500 |
| Blue channel B | 11000–20500 | 13000–19000 |
| Additional test | `B + 3000 >= R` | `R > B + 3000` |

## Calibration

The ground color sensor is mounted `2.75 cm` above the floor.

The reported calibration used the sensor's LEDs. Thresholds were selected from readings gathered during testing.

Serial output shows:

- R value
- G value
- B value
- Resulting color classification
