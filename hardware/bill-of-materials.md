# Bill of Materials

| Quantity | Component | Key specification | Purpose |
|---:|---|---|---|
| 1 | Arduino Mega 2560 with screw-terminal shield | Main controller; accessible wiring terminals | Runs the challenge programs and connects the sensors and actuators |
| 1 | DRV8871 motor driver | Two-input motor control; IN1 on D5 and IN2 on D6 | Controls the rear drive motor |
| 1 | DC gear motor | RN AI kit motor; kit reference lists 300 RPM | Drives the rear axle |
| 1 | Rear differential assembly | Allows the rear wheels to rotate at different speeds | Transfers motor power to the rear wheels during straight travel and turns |
| 1 | Steering servo | Signal connected to D8 | Operates the front Ackermann steering linkage |
| 1 | TCS3200 color sensor | Frequency-output sensor with four LEDs | Detects blue and orange floor lines |
| 5 | HC-SR04 ultrasonic sensors | Left, front-left, front, front-right, and right | Measure distances to walls and nearby objects |
| 1 | HuskyLens 2 | UART1 communication with the Mega; separate USB-C power | Detects red and green obstacles |
| 1 | 4S LiPo battery | Zeee, 14.8 V nominal, 1500 mAh, XT60 connector | Main vehicle power source |
| 1 | Step-down regulator | Adjusted to 12 V output | Supplies the regulated 12 V rail |
| 1 | USB power bank | 5 V USB output | Powers the HuskyLens 2 separately |
| 1 | Start button | D34 to GND using `INPUT_PULLUP` | Starts a run when pressed and released |
| 1 | Main power switch | Installed in the main battery supply path | Switches vehicle power on and off |
| 2 | Rear wheels | 60 mm diameter | Driven wheels |
| 2 | Front wheels | 35 mm diameter | Steered wheels |
