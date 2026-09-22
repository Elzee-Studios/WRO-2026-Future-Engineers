# WRO 2026 Future Engineers — Self-Driving Car

This repository documents our autonomous vehicle for the WRO 2026 Future Engineers category. It contains our engineering notebook, technical documentation, source code, bill of materials, photographs and demonstration-video links.

> **Team:** KFS Engineers  
> **Country:** Saudi Arabia  
> **School/Organization:** King Faisal Schools  
> **Team members:** Layth Alzahrani & Mohammed Albassam  
> **Coach:** Eng. Mohamed Salaheldin Abdulhaq

## Repository guide

| Section | Contents |
|---|---|
| [`engineering-journal/`](engineering-journal/) | KFS Engineers WRO 2026 engineering notebook |
| [`docs/`](docs/) | Technical documentation for the vehicle |
| [`hardware/`](hardware/) | Bill of materials |
| [`src/`](src/) | Open Challenge and Obstacle Challenge source code |
| [`vehicle-photos/`](vehicle-photos/) | Vehicle photographs |
| [`team-photos/`](team-photos/) | Team photographs |
| [`video/`](video/) | Challenge demonstration videos |

## 1. Project overview

Our vehicle is built around an Arduino Mega with rear-wheel propulsion and servo-operated front steering. A DC motor drives the rear wheels through a differential, while a steering servo controls the front Ackermann linkage.

For the Open Challenge, a ground color sensor detects blue floor lines. Each accepted blue-line detection triggers a timed left turn, and the vehicle counts 12 accepted detections before completing its final straight movement and stopping.

For the Obstacle Challenge, the current program uses two predefined sequences of timed forward movements and steering commands.

## 2. Mobility and mechanical design

The vehicle uses a metal chassis with an upper electronics platform, rear-wheel propulsion through a differential and servo-operated front Ackermann steering.

Full documentation: [`docs/01-mobility-and-mechanical-design.md`](docs/01-mobility-and-mechanical-design.md)

## 3. Power and sensor architecture

The vehicle uses a 12 V, 3000 mAh Studica battery and an Arduino Mega as its main controller. Hardware includes an L298N motor driver, steering servo, ground color sensor, five ultrasonic sensors and a HuskyLens 1.

Full documentation: [`docs/02-power-and-sensor-architecture.md`](docs/02-power-and-sensor-architecture.md)

## 4. Software architecture and challenge strategy

Separate Arduino sketches are used for the Open Challenge and Obstacle Challenge.

The Open Challenge uses ground-color detection, corner counting and timed steering. The Obstacle Challenge uses two predefined sequences of timed movements.

Full documentation: [`docs/03-software-and-obstacle-strategy.md`](docs/03-software-and-obstacle-strategy.md)

## 5. Systems thinking and engineering decisions

This section documents the major engineering decisions, constraints, testing observations, wiring repairs and tradeoffs involved in the vehicle's development.

Full documentation: [`docs/04-systems-thinking-and-decisions.md`](docs/04-systems-thinking-and-decisions.md)

## 6. Reproducing the vehicle

This section documents the vehicle's wiring, Arduino setup, software configuration, calibration and verification procedure.

Full documentation: [`docs/05-reproducibility.md`](docs/05-reproducibility.md)

## 7. Demonstration videos

- Open Challenge: [Add YouTube link]
- Obstacle Challenge: [Add YouTube link]

## 8. Engineering notebook

The engineering notebook is available in [`engineering-journal/`](engineering-journal/).
