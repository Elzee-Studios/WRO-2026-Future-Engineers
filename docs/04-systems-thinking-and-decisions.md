# Systems Thinking and Engineering Decisions

## Requirements and constraints

### Vehicle dimensions

| Measurement | Value |
|---|---:|
| Overall length | 27.5 cm |
| Overall width | 19.5 cm |
| Overall height | 21.0 cm |
| Wheelbase | 17.5 cm |
| Rear-wheel diameter | 6.0 cm |
| Front-wheel diameter | 3.5 cm |
| Color sensor height above floor | 2.75 cm |

### Power

The vehicle uses a Studica battery rated at:

- 12 V nominal
- 3000 mAh
- 36 Wh nominal energy

The notebook does not assign a numerical runtime because motor current, total current and peak accessory current were not measured.

### Timing

The Open Challenge uses:

- 1000 ms timed left turns
- 1250 ms final forward movement before stopping

The Obstacle Challenge uses two timed movement sequences:

- First sequence: 9.09 s of motion delays
- Second sequence: 8.59 s of motion delays
- Total motion delays: 17.68 s

### Reliability

Testing showed that wiring could enter a wheel's moving path. An ultrasonic sensor wire was cut during testing, so the damaged wiring was replaced and cables were secured with zip ties.

The Arduino power cord also broke during the same repair. It was soldered and insulated with heat-shrink tubing.

## Major design decisions

| Decision | Evidence or test | Tradeoff | Final choice |
|---|---|---|---|
| Separate propulsion and steering | Vehicle uses a rear motor and front steering servo | Wheel paths during turns depend on both mechanisms | Rear-wheel propulsion with servo-operated front steering |
| Rear differential | Mechanical design of the rear drive assembly | Allows the two rear wheels to follow different paths during turns | Use a differential between the rear wheels |
| Timed left turns | Steering was adjusted during the first two Open Challenge attempts | Simple and repeatable, but actual path still depends on traction, load and starting alignment | 55° steering command for 1000 ms |
| Three-reading BLUE re-arm | A persistent BLUE reading could otherwise repeatedly trigger turns | Reduces repeated triggers but does not independently confirm a new physical line | Require three consecutive non-blue readings before accepting another BLUE trigger |
| 12-corner finish routine | Four corners per lap × three laps | Depends on correct color detections; missed or false detections affect the stopping location | Stop after 12 accepted BLUE detections and a 1250 ms final straight |
| Timed Obstacle Challenge route | Implemented under limited available time | Simple to implement, but depends on starting pose, battery condition, traction and obstacle layout | Two predefined timed movement sequences |
| Cable restraints | An ultrasonic wire was cut after entering a wheel's path | Additional cable-routing and securing work | Replace damaged wiring and secure cables with zip ties |

## Integration lessons

Mechanical, electrical and software behavior affected one another during testing.

A sensor wire entered the path of a moving wheel and was cut. Although that ultrasonic sensor was not used by the active Open Challenge program, its wiring still affected the mechanical reliability of the vehicle. After the failure, wiring was rerouted and secured with zip ties.

The steering commands also interact with mechanical conditions. The same steering angle and turn duration provide a repeatable software command, but the actual vehicle path can still change with traction, load and starting alignment.

The Obstacle Challenge shows a similar interaction. Because the route uses timed movements rather than sensor feedback, its physical path depends on the starting pose, battery condition, traction and configured obstacle layout.

The ground color sensor was mounted 2.75 cm above the floor, and its thresholds were selected using readings collected in that installed position.
