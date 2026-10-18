# Mobility and Mechanical Design

## Chassis overview

The Invader uses the RN AI kit’s metal chassis, front steering assembly, and rear-drive assembly. A lower deck holds the drivetrain and steering linkage. An upper deck carries the Arduino Mega, DRV8871 motor driver, start button, and power switch, with the connections accessible from above.

| Measurement | Value |
|---|---:|
| Overall length | 275 mm |
| Overall width | 195 mm |
| Overall height | 210 mm |
| Wheelbase | 175 mm |
| Rear-wheel diameter | 60 mm |
| Front-wheel diameter | 35 mm |
| Color sensor height above floor | 27.5 mm |
| Vehicle weight | Not yet measured |

The vehicle is within the 300 × 200 × 300 mm size limit. Width has the least clearance: only 5 mm remains before the 200 mm limit. The vehicle still needs to be weighed with all equipment fitted.

## Drive system

One DC gear motor drives the rear wheels through a differential. The front wheels steer but do not provide propulsion.

When the vehicle turns, the outside rear wheel travels farther than the inside rear wheel. The differential allows the wheels to rotate at different speeds, reducing skidding during turns.

A DRV8871 motor driver controls the drive motor through two Arduino Mega pins: IN1 on D5 and IN2 on D6. To drive forward, the program holds IN2 HIGH and applies PWM to IN1 as `255 - speed`. It gives the motor a 150 ms kick at PWM 255 to start moving, then reduces the command to its normal value of 200. At the finish, it briefly brakes the motor before stopping.

## Steering system

A servo on Arduino pin D8 operates the front Ackermann linkage. In a turn, the linkage points the inside front wheel more sharply than the outside front wheel so they can follow different turning radii.

| Program command | Servo value | Use |
|---|---:|---|
| `STEER_CENTER` | 82° | Straight travel |
| `STEER_LEFT` | 55° | Anticlockwise corners |
| `STEER_RIGHT` | 109° | Clockwise corners |

These values are commands sent to the servo; they are not measurements of the front wheels’ angles or the vehicle’s change in heading. The 82° center and 55° left commands were tuned on the track. The 109° right command is a mirrored estimate that still needs full clockwise test runs.

The program holds the steering command for 2,200 ms on the first corner, 1,800 ms on corners 2–11, and 2,100 ms on the final corner.

## Speed and torque reasoning

A 60 mm rear wheel travels approximately 188.5 mm per revolution if it rolls without slipping. Its theoretical ground speed can be estimated with:

`v = π × D × n / 60`

where `D` is wheel diameter in meters and `n` is **wheel** speed in revolutions per minute.

The kit lists a 300 RPM motor. If the rear wheels themselves reached 300 RPM, the calculation would give approximately 0.94 m/s. Actual speed will be lower or higher depending on the gearing, motor load, PWM command, and wheel slip. We have not yet measured the vehicle’s ground speed.

## Testing and mechanical reliability

| Test or observation | Result or action |
|---|---|
| First two logged anticlockwise runs | Used to tune straight steering and speed |
| Next three logged full-course runs | Each completed three laps and stopped in the finish section |
| Ultrasonic sensor wire entered a wheel’s path during the first test | Replaced the damaged wire, rerouted the cables, and secured them with zip ties |
| Arduino power cord broke during the repair | Soldered the connection and insulated it with heat-shrink tubing |

Before each run, we check that no wire can reach a wheel, drive shaft, or steering linkage. Clockwise steering and the vehicle’s final weight still require verification.
