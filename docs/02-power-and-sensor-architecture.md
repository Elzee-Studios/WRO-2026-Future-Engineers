# Power and Sensor Architecture

## Electronics overview

An Arduino Mega 2560 controls the vehicle. A DRV8871 drives the rear motor, a servo steers the front wheels, and sensors provide floor-color, distance, and obstacle information.

| Component | Role / connection |
|---|---|
| Arduino Mega 2560 | Main controller |
| DRV8871 motor driver | Controls drive-motor speed and direction |
| Steering servo | Operates the front steering linkage from D8 |
| TCS3200 color sensor | Reads blue and orange floor lines |
| 5 HC-SR04 ultrasonic sensors | Measure distances to nearby walls and objects |
| HuskyLens 2 | Detects red and green obstacles; communicates over UART1 |
| 4S LiPo battery | Main vehicle power source; 14.8 V nominal, 1500 mAh |
| Step-down regulator | Supplies a regulated 12 V rail |
| USB power bank | Powers the HuskyLens 2 through USB-C |
| Start button | Connects D34 to GND when pressed |
| Power switch | Controls the main vehicle power |

### Arduino connections

| Device / signal | Mega connection |
|---|---|
| DRV8871 IN1 / IN2 | D5 / D6 |
| Steering servo signal | D8 |
| Start button | D34 to GND using `INPUT_PULLUP` |
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

All components that exchange signals share a common ground. The HuskyLens 2 communication cable carries TX, RX, and GND; its positive power wire is disconnected because the camera receives power from its own USB power bank.

## Power distribution

The main battery is a 4S LiPo rated at 14.8 V and 1500 mAh. Its calculated nominal energy is:

`14.8 V × 1.5 Ah = 22.2 Wh`

Power follows these routes:

- **4S LiPo → main switch → step-down regulator:** The regulator is set to 12 V.
- **12 V rail → Arduino Mega Vin:** The Mega powers the servo, color sensor, and ultrasonic sensors from its 5 V rail.
- **12 V rail → DRV8871 → drive motor:** The driver controls motor speed and direction.
- **USB power bank → HuskyLens 2 USB-C:** The camera has a separate power supply but shares ground with the Mega for UART communication.

The HuskyLens 2 is rated at approximately 1.5–3 W, or 0.3–0.6 A at 5 V. Supplying that load through the Mega’s onboard 5 V regulator would add substantial heat, especially with the servo and other sensors connected. The separate power bank supplies the camera directly.

The servo remains on the Mega’s 5 V rail. Its current peaks during steering have not been measured, so a controller reset during a turn would be a reason to check that supply. The motor’s actual current and the vehicle’s runtime have not yet been measured.

## Sensor placement and use

| Sensor | Position and purpose |
|---|---|
| TCS3200 color sensor | Beneath the front of the vehicle in a 3D-printed holder, 27.5 mm above the floor; detects blue and orange corner lines |
| Left ultrasonic | Measures distance on the left |
| Front-left ultrasonic | Measures distance diagonally ahead on the left |
| Front ultrasonic | Measures distance directly ahead |
| Front-right ultrasonic | Measures distance diagonally ahead on the right |
| Right ultrasonic | Measures distance on the right |
| HuskyLens 2 | Mounted on the front mast; provides detections of red and green obstacles |

The floor-facing color sensor reaches a corner line before the wheels. Its four LEDs illuminate the surface beneath it. The Open Challenge code reads the color sensor and ultrasonic sensors. The Obstacle Challenge code also processes HuskyLens 2 detections.

## TCS3200 calibration

The TCS3200 produces pulses rather than direct RGB values. The program sets `S0 = HIGH` and `S1 = LOW` for 20% output-frequency scaling, then selects each channel with S2 and S3:

| Channel | S2 | S3 |
|---|---|---|
| Red | LOW | LOW |
| Green | HIGH | HIGH |
| Blue | LOW | HIGH |

After changing the filter, the program waits 300 µs and measures one LOW pulse with a 5,000 µs timeout. For a valid pulse, it calculates a frequency-like value as:

`500000 / pulse duration in microseconds`

The following ranges are the Open Challenge calibration recorded with the sensor 27.5 mm above our mat:

| Channel | BLUE | ORANGE |
|---|---:|---:|
| Red (R) | 15,000–25,000 | 26,500–39,000 |
| Green (G) | 15,000–24,000 | 19,500–25,000 |
| Blue (B) | 22,000–36,000 | 22,500–29,000 |

A reading must fall within all three ranges for a color to be accepted. The red ranges do not overlap, helping separate blue from orange. The program prints the channel readings and classification to the Serial Monitor at 115200 baud so the ranges can be checked under the event lighting.

## Reliability checks

- If a color-channel pulse does not arrive within 5,000 µs, that channel returns zero and the reading is classified as `OTHER`.
- After a line triggers a turn, the Open Challenge code requires 20 consecutive readings outside the selected color before it accepts another line.
- Before each run, check that cables cannot touch the wheels, drive shaft, or steering linkage. An ultrasonic wire was cut by a wheel during testing; the wiring was replaced, rerouted, and secured with zip ties.
- Check that the power bank is on and the HuskyLens 2 has started before an Obstacle Challenge run.
- Recheck blue and orange readings at the venue. Sensor height, surface, and lighting can change the measured values.
