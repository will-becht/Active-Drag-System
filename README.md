# Rocketry Project

## Table of Contents
- [Introduction](#introduction)
- [Arduino Codes](#arduino-codes)
- [CAD](#cad)
- [OpenRocket](#openrocket)
- [PCB Design](#pcb-design)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction
This project is focused on the development of a model rocket, incorporating various aspects such as Arduino-based control systems, CAD designs for structural components, simulation and design using OpenRocket, and detailed PCB design for the electronics involved.

## Arduino Codes
### Overview
The `Arduino Codes` directory contains all the code necessary for the operation of the rocket's control systems. These include code for sensor integration, data logging, and motor control.

### Image
![Arduino Codes](Arduino%20Codes/sample_image.png)
*(Replace `sample_image.png` with the actual image name if it exists in the `Arduino Codes` directory)*

### Description
The Arduino code is crucial for ensuring the rocket operates as intended. It handles tasks such as sensor data acquisition, motor control, and communication between different components of the rocket.

### Code Highlights
- `main.ino`: The primary code file for the rocket's Arduino-based control system.
- `sensors.ino`: Handles the integration of various sensors used in the rocket.

## CAD
### Overview
The `CAD` directory contains the design files for the rocket's structural components. These files can be used to fabricate the parts using 3D printing or other manufacturing methods.

### Image
![CAD Design](CAD/rocket_cad_design.png)
*(Replace `rocket_cad_design.png` with the actual image name if it exists in the `CAD` directory)*

### Description
CAD files are essential for ensuring the rocket's components fit together correctly and are robust enough to withstand the forces experienced during launch and flight. 

### Design Highlights
- `rocket_body.sldprt`: SolidWorks file for the main body of the rocket.
- `fins.sldprt`: SolidWorks file for the rocket fins.

## OpenRocket
### Overview
The `OpenRocket` directory contains simulation files for the rocket. These files can be used to simulate the rocket's flight characteristics and performance.

### Images
![Rocket Design](OpenRocket/Design.png)
![Flight Simulation](OpenRocket/FlightSim.png)

### Description
OpenRocket is a free, fully-featured model rocket simulator that allows you to design and simulate the performance of rockets. The files in this directory include detailed simulations that help in predicting the flight path, stability, and performance of the rocket.

### Simulation Highlights
- `researchRocket2.ork`: OpenRocket file for the rocket simulation.
- `FlightSim.png`: An image showing the results of the flight simulation.

## PCB Design
### Overview
The `PCB Design` directory contains all the files related to the design and manufacturing of the rocket's printed circuit boards (PCBs). These PCBs are used to integrate the electronic components necessary for the rocket's operation.

### Images
![Top Layer](PCB%20Design/Images/topLayer.png)
![Bottom Layer](PCB%20Design/Images/bottomLayer.png)
![Schematic](PCB%20Design/Images/schematic.png)

### Description
PCB design is crucial for ensuring that the electronic components are securely mounted and connected properly. The files in this directory include design files, Gerber files for manufacturing, and images showing different layers and the schematic of the PCB.

### Design Highlights
- `boardFinal.brd`: The final PCB design file.
- `schematic.png`: Image of the schematic layout for the PCB.

## Installation
To install the software and code for this project, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/rocket-project.git