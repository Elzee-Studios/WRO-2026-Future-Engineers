# Software Architecture and Challenge Strategy

## Software overview
[Describe the programming language, development environment, libraries, controller, and major modules.]

## Module relationships
[Explain how sensor input becomes an estimate of the track state, a driving decision, and motor/steering commands. Add a flowchart or state-machine diagram in `schemes/`.]

## Open Challenge strategy
[Explain direction detection, wall/lane following, corner recognition, lap counting, start/finish detection, and autonomous stopping.]

## Obstacle Challenge strategy
[Explain red/green pillar detection, passing-side decisions, collision avoidance, lap counting, parking-area detection, and parallel parking.]

## Control and tuning
[Document steering control, motor-speed control, thresholds, filtering, PID values if used, and the method used to tune them.]

## Failure handling
[Explain recovery or safe behavior for uncertain sensor readings and unexpected track conditions.]

## Supporting files
- Source code: [`../src/`](../src/)
- Software diagrams: [`../schemes/`](../schemes/)
- Testing evidence: [`../testing/`](../testing/)

