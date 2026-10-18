# Systems Thinking and Engineering Decisions

## Requirements and constraints

| Requirement | Vehicle or design value | Constraint |
|---|---:|---|
| Length | 275 mm | 300 mm maximum |
| Width | 195 mm | 200 mm maximum |
| Height | 210 mm | 300 mm maximum |
| Weight | Not yet measured | 1.5 kg maximum |
| Wheelbase | 175 mm | — |
| Rear / front wheel diameter | 60 / 35 mm | — |
| Color sensor height | 27.5 mm above the floor | Must remain consistent for calibration |

Width is our tightest dimension, with only 5 mm of clearance. The vehicle must be measured and weighed with the power bank and all mounts fitted.

The Arduino Mega handles motor control and sensor readings. The HuskyLens 2 processes camera images and sends detections to the Mega over UART, avoiding the need for the Mega to process images itself.

## Power decisions

A 14.8 V, 1500 mAh 4S LiPo provides 22.2 Wh of nominal energy. A step-down regulator supplies a 12 V rail for the Mega and drive system. A separate USB power bank supplies the HuskyLens 2.

| Decision | Reason | Tradeoff or check |
|---|---|---|
| DRV8871 motor driver | Controls the rear motor through two inputs and supports the 12 V drive system | Motor wiring and control logic must match the driver |
| 12 V step-down regulator | Holds the supply steady as the LiPo voltage falls | Confirm its current rating is sufficient for the connected loads |
| Separate camera power bank | Supplies the HuskyLens 2 without loading the Mega’s 5 V regulator | Check that the bank is on before each run and remains on |
| Shared camera and Mega ground | Provides a reference for UART communication | Keep the camera cable’s positive power wire disconnected |

Motor current, servo current peaks, and total vehicle current have not been measured. Battery runtime is therefore an estimate, not a tested result. A controller reset during steering would prompt us to check the servo’s 5 V supply.

## Navigation and control decisions

| Decision | Why we chose it | Limitation |
|---|---|---|
| Rear differential | Lets the rear wheels rotate at different speeds in a turn | Adds mechanical parts and possible gear play |
| Front Ackermann steering | Gives the inside and outside wheels appropriate turning angles using one servo | Steering response depends on the linkage and available turning space |
| Downward-facing color sensor | Reads corner lines directly beneath the vehicle | Detects only the surface immediately below it |
| Direction from the first line | Blue selects anticlockwise travel; orange selects clockwise travel | A false first detection could select the wrong direction |
| Timed corner turns | Simple to tune and repeat in code | The physical turn can vary with traction, load, and battery condition |
| 20-reading line re-arm | Reduces repeated counts while crossing one line | Does not independently prove that the next reading comes from a new corner |
| HuskyLens 2 for obstacles | Sends red and green obstacle detections to the Mega | Detection must be checked under field lighting and while moving |
| Ultrasonic distance sensing | Supports wall clearance and backup turn decisions | Readings can be missed or affected by an obstacle in front of a sensor |

The Open Challenge counts four corners per lap for three laps. After the twelfth turn, it drives forward for 1,500 ms, brakes, and stops. Its left turns are timed at 2,200 ms for the first corner, 1,800 ms for corners 2–11, and 2,100 ms for the last corner. Clockwise steering requires full-course testing.

## Risks and responses

| Risk | Effect | Response |
|---|---|---|
| Cable reaches a wheel or steering linkage | Damaged wire or interrupted run | Route cables away from moving parts and check clearance before every run |
| Camera has insufficient power or its power bank turns off | Obstacle detections stop | Power it separately and confirm its screen is on before starting |
| False or missed floor-line reading | Wrong turn or corner count | Check calibration at the venue and inspect serial readings |
| Different lighting or sensor height | Color readings shift outside the calibrated ranges | Keep the sensor at 27.5 mm and recheck readings on the event mat |
| Low LiPo voltage | Reduced supply stability | Check and recharge the pack before a run |
| Servo current spike | Possible Mega reset | Monitor for resets and investigate the 5 V supply if one occurs |
| Vehicle exceeds the width or weight limit | Fails inspection | Measure and weigh the complete vehicle |

An ultrasonic sensor wire was cut when it entered a wheel’s path during testing. We replaced it, rerouted the wiring, and secured the cables with zip ties. The Arduino power cord also broke during that repair; we soldered it and insulated the joint with heat-shrink tubing. These failures made cable clearance and power-connection checks part of our pre-run routine.

## Testing and remaining work

The first two logged anticlockwise Open Challenge runs were used for steering adjustments. The next three logged full-course runs each completed three laps and stopped in the finish section. Those results do not establish reliability for clockwise runs, different starting positions, or the Obstacle Challenge.

Our next tests record laps completed, stopping position, elapsed time, and wall contacts across both directions and different starting sections. We also need to weigh the complete vehicle, measure motor and servo current, and test obstacle detection while driving.
