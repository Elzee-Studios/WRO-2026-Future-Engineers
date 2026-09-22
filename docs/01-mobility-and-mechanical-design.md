# Mobility and Mechanical Design

## Chassis overview

The vehicle uses a metal chassis with an upper electronics platform. The build combines the RN steering and rear-drive assemblies with the metal chassis. The upper platform provides mounting space and access to the controller and electrical connections.

| Measurement | Value |
|---|---:|
| Overall length | 27.5 cm |
| Overall width | 19.5 cm |
| Overall height | 21.0 cm |
| Wheelbase | 17.5 cm |
| Rear-wheel diameter | 6.0 cm |
| Front-wheel diameter | 3.5 cm |
| Color sensor height above floor | 2.75 cm |

## Drive system

The vehicle uses rear-wheel propulsion. One DC motor drives the rear wheels through a differential assembly, while the front wheels are used for steering.

The rear differential allows the outside rear wheel to rotate faster than the inside rear wheel while the vehicle follows a curved path.

The rear wheels have a diameter of 60 mm. The kit reference lists a 300 RPM motor.

## Steering system

A single steering servo operates the front Ackermann linkage. The linkage gives the inside front wheel a larger steering angle than the outside front wheel to account for their different turning radii.

| Program command | Value | Role |
|---|---:|---|
| `STEER_CENTER` | 82° | Servo command for straight travel |
| `STEER_LEFT` | 55° | Servo command for a left turn |
| `TURN_TIME_MS` | 1000 ms | Duration of the commanded turn |

These angles are servo commands and do not directly state the front-wheel angles or the vehicle's heading change.

## Speed and torque reasoning

A 60 mm rear wheel travels approximately 188.5 mm per revolution without slip.

Wheel speed can be estimated using:

`v = π × D × n / 60`

where `D` is wheel diameter in meters and `n` is wheel speed in revolutions per minute.

The kit reference lists a 300 RPM motor. Actual wheel speed depends on the transmission, load and drive command.

## Testing and iterations

| Version/date | Test or problem | Evidence | Change made | Result |
|---|---|---|---|---|
| Attempt 01 | Steering adjustment | Team test record | Steering angles tuned | Used to tune steering angles |
| Attempt 02 | Steering adjustment | Team test record | Steering angles tuned | Used to tune steering angles |
| Attempt 03 | Complete course | Team test record | — | Completed three laps |
| Attempt 04 | Complete course | Team test record | — | Completed three laps |
| Attempt 05 | Complete course | Team test record | — | Completed three laps |
| First test | Ultrasonic sensor wire entered a wheel's moving path and was cut | Physical wiring failure | Damaged wiring replaced and cables secured with zip ties | Wiring kept away from moving parts |
| First test | Arduino power cord broke | Physical wiring failure | Power cord soldered and insulated with heat-shrink tubing | Power connection restored |
