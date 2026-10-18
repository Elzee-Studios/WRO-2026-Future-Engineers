# The Invader — WRO 2026 Future Engineers

This repository documents The Invader, our autonomous vehicle for the WRO 2026 Future Engineers category. It contains our engineering notebook, technical documentation, Arduino code, bill of materials, photographs, and demonstration videos.

> **Team:** KFS Engineers  
> **Country:** Saudi Arabia  
> **School/Organization:** King Faisal Schools, Riyadh  
> **Team members:** Mohammed Albassam and Layth Alzahrani  
> **Coach:** Eng. Mohamed Salaheldin Abdulhaq

## Repository guide

| Section | Contents |
|---|---|
| [`engineering-journal/`](engineering-journal/) | Engineering notebook |
| [`docs/`](docs/) | Technical documentation |
| [`hardware/`](hardware/) | Bill of materials |
| [`src/`](src/) | Open Challenge and Obstacle Challenge code |
| [`vehicle-photos/`](vehicle-photos/) | Vehicle photographs |
| [`team-photos/`](team-photos/) | Team photographs |
| [`video/`](video/) | Demonstration videos |

## 1. Project overview

The Invader is built around an Arduino Mega 2560. One DC gear motor drives the rear wheels through a differential, while a servo operates the front Ackermann steering linkage. A DRV8871 motor driver controls the drive motor.

For the Open Challenge, a downward-facing TCS3200 color sensor reads the blue and orange corner lines. The first line determines the driving direction: blue selects anticlockwise left turns, and orange selects clockwise right turns. The vehicle counts 12 corners across three laps, drives into the finish section, and brakes. Five ultrasonic sensors provide distance readings from the surrounding walls.

For the Obstacle Challenge, the vehicle uses a HuskyLens 2 to detect red and green obstacles, ultrasonic sensors to measure nearby distances, and the color sensor to identify corner lines. Its steering logic aims to pass red obstacles on the right and green obstacles on the left. Parallel parking is still in development.

## 2. Mobility and mechanical design

The vehicle has a metal chassis with an upper electronics deck, rear-wheel drive through a differential, and servo-operated Ackermann steering at the front. Its measured dimensions are 275 × 195 × 210 mm.

Full documentation: [`docs/01-mobility-and-mechanical-design.md`](docs/01-mobility-and-mechanical-design.md)

## 3. Power and sensor architecture

A 14.8 V, 1500 mAh 4S LiPo supplies the vehicle. A step-down regulator provides 12 V for the Arduino Mega and drive system. A separate USB power bank supplies the HuskyLens 2, which communicates with the Mega over UART.

The TCS3200 color sensor is mounted beneath the vehicle, 27.5 mm above the floor. Five HC-SR04 ultrasonic sensors face left, front-left, front, front-right, and right.

Full documentation: [`docs/02-power-and-sensor-architecture.md`](docs/02-power-and-sensor-architecture.md)

## 4. Software architecture and challenge strategy

Separate Arduino sketches control the Open Challenge and Obstacle Challenge.

The Open Challenge combines color detection, direction selection, corner counting, timed steering, ultrasonic distance readings, and a finish routine. The Obstacle Challenge combines camera-based obstacle detection, ultrasonic distance sensing, and color-line detection to navigate the course.

Full documentation: [`docs/03-software-and-obstacle-strategy.md`](docs/03-software-and-obstacle-strategy.md)

## 5. Engineering decisions and testing

This section explains our mechanical and electrical choices, power budget, sensor calibration, wiring precautions, known risks, and testing results.

Full documentation: [`docs/04-systems-thinking-and-decisions.md`](docs/04-systems-thinking-and-decisions.md)

## 6. Reproducing the vehicle

The build guide covers assembly, wiring, Arduino setup, color-sensor calibration, and pre-run checks.

Full documentation: [`docs/05-reproducibility.md`](docs/05-reproducibility.md)

## 7. Demonstration videos

- Open Challenge: https://www.youtube.com/watch?v=uSCZ3VZdSqs
- Obstacle Challenge: https://www.youtube.com/watch?v=QbwrhJ6V9O4

## 8. Engineering notebook

The complete engineering notebook is available in [`engineering-journal/`](engineering-journal/).
