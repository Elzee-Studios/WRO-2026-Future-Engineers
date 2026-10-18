# Reproducibility and Setup

## Required parts

See the complete [bill of materials](../hardware/bill-of-materials.md). The main components are an Arduino Mega 2560, RN AI kit chassis and drivetrain, DRV8871 motor driver, steering servo, TCS3200 color sensor, five HC-SR04 ultrasonic sensors, HuskyLens 2, 4S LiPo, 12 V step-down regulator, and USB power bank.

## Build and wiring

Assemble the rear-drive chassis, differential, and front steering linkage. Mount the Mega and motor driver on the upper deck. Fit the color sensor beneath the front of the vehicle, **27.5 mm above the floor**, and mount the HuskyLens 2 on the front mast. Route all wires clear of the wheels, drive shaft, and steering linkage.

Use these Arduino Mega connections:

| Device / signal | Mega connection |
|---|---|
| DRV8871 IN1 / IN2 | D5 / D6 |
| Steering servo signal | D8 |
| Start button | D34 to GND |
| Color sensor S0 / S1 | D35 / D36 |
| Color sensor S2 / S3 | D37 / D38 |
| Color sensor OUT | D39 |
| Left ultrasonic TRIG / ECHO | D22 / D23 |
| Front-left ultrasonic TRIG / ECHO | D24 / D25 |
| Front ultrasonic TRIG / ECHO | D26 / D27 |
| Front-right ultrasonic TRIG / ECHO | D28 / D29 |
| Right ultrasonic TRIG / ECHO | D30 / D31 |
| HuskyLens 2 TX → Mega RX1 | D19 |
| HuskyLens 2 RX ← Mega TX1 | D18 |

The start button uses `INPUT_PULLUP`: D34 reads HIGH normally and LOW while the button connects it to ground.

### Power connections

1. Connect the 4S LiPo through the main switch to the step-down regulator.
2. Set and verify the regulator output at **12 V** before connecting the electronics.
3. Supply the Mega’s Vin and the DRV8871 from the regulated 12 V rail.
4. Supply the servo, TCS3200, and ultrasonic sensors from the Mega’s 5 V rail.
5. Power the HuskyLens 2 through its USB-C port using a separate USB power bank.
6. Connect the HuskyLens 2 TX, RX, and GND to the Mega. Leave the communication cable’s positive power wire disconnected so the power bank and Mega 5 V rails are not joined.
7. Connect all signal-using components to a common ground.

Check polarity with a multimeter before switching on the vehicle.

## Software requirements

- Arduino IDE with **Arduino Mega 2560** selected as the board
- Arduino `Servo` library
- DFRobot HUSKYLENS 2 library for the Obstacle Challenge sketch

The challenges use separate Arduino sketches. Open the required sketch in a matching sketch folder, select the Mega’s port, compile, and upload it. Use the constants in the selected sketch when checking its steering, sensor, and motor settings.

## Open Challenge configuration

| Parameter | Value |
|---|---:|
| Straight steering | 82° |
| Left steering | 55° |
| Right steering | 109° |
| Normal and turn speed command | 200 / 255 |
| Starting kick | 255 / 255 for 150 ms |
| First turn | 2,200 ms |
| Turns 2–11 | 1,800 ms |
| Final turn | 2,100 ms |
| Drive after final turn | 1,500 ms |
| Finish brake | 300 ms |
| Readings outside the selected color needed to re-arm | 20 |

The first accepted blue line selects anticlockwise travel and left turns. The first accepted orange line selects clockwise travel and right turns. The program counts 12 corners, completes the final drive, brakes, and waits for a reset.

## Color-sensor setup and calibration

The TCS3200 uses 20% output-frequency scaling with `S0 = HIGH` and `S1 = LOW`.

| Channel | S2 | S3 |
|---|---|---|
| Red | LOW | LOW |
| Green | HIGH | HIGH |
| Blue | LOW | HIGH |

The Open Challenge sketch waits **300 µs** after changing a filter, then measures a LOW pulse with a **5,000 µs** timeout. It takes one reading per channel and calculates `500000 / pulse duration` for a valid pulse.

The recorded Open Challenge ranges are:

| Channel | BLUE | ORANGE |
|---|---:|---:|
| Red (R) | 15,000–25,000 | 26,500–39,000 |
| Green (G) | 15,000–24,000 | 19,500–25,000 |
| Blue (B) | 22,000–36,000 | 22,500–29,000 |

A reading must fall within all three ranges for a color to be accepted. These values were calibrated with the sensor’s LEDs on and its height fixed at 27.5 mm. Open the Serial Monitor at **115200 baud**, read R, G, B, and the classification over each line and the surrounding floor, and adjust the ranges if the event mat or lighting changes.

The Obstacle Challenge sketch has its own color settings. Check and calibrate the values in that sketch separately rather than copying the Open Challenge ranges into it.

## Obstacle Challenge setup

The Obstacle Challenge sketch reads the HuskyLens 2 over `Serial1`, the five ultrasonic sensors, and the floor color sensor. Its camera color-recognition IDs must match the colors trained on the HuskyLens 2 before a run. Check that the camera is powered, connected, and reporting red and green detections.

The sketch uses different steering and obstacle-navigation settings from the Open Challenge sketch. Keep each sketch’s settings with its own code. Parallel parking is not implemented.

## Before each run

1. Confirm that the complete vehicle meets the size limit and that no cable can touch a moving part.
2. Check the LiPo charge, 12 V regulator output, main switch, and camera power bank.
3. Confirm that the correct challenge sketch is uploaded.
4. Check the steering center and sensor readings.
5. Place the vehicle in the chosen starting section.
6. Press and release the D34 start button. The program waits 300 ms after release before starting its motor kick.
7. Reset the Mega before the next run.
