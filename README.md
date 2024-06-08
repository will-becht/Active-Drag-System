# Rocketry Project

## Table of Contents
- [Introduction](#introduction)
- [Flight Software](#flight-software)
- [PCB Design](#pcb-design)
- [CAD](#cad)
- [OpenRocket](#openrocket)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Introduction
This project is focused on the development of a model rocket, incorporating various aspects such as an active drag system, CAD designs for structural components, simulation and design using OpenRocket, and detailed PCB design for the electronics involved.

## Flight Software
### Overview
The `Flight Software` section contains the code necessary for controlling the rocket's Active Drag System. This system adjusts the rocket's drag tabs based on readings from an altimeter and accelerometer to control the rocket's apogee.

### Image
![Flight Software](Flight%20Software/sample_image.png)
*(Replace `sample_image.png` with the actual image name if it exists in the `Flight Software` directory)*

### Description
The `flightCode_Updated_v2.ino` file implements a state machine to manage the deployment of drag tabs in the Active Drag System. This system actively modulates the drag during flight to achieve a desired apogee by processing real-time data from the altimeter and accelerometer. The state machine ensures that the drag tabs are deployed or retracted based on the rocket's altitude and acceleration, thus controlling the peak altitude accurately.

### Code Highlights
- `flightCode_Updated_v2.ino`: Contains the state machine that manages the drag tabs deployment using altimeter and accelerometer data.

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

## CAD
### Overview
The `CAD` directory contains the design files for the rocket's structural components. These files can be used to fabricate the parts using 3D printing or other manufacturing methods.

### Images
![CAD Overview](CAD/cad_overview.png)
![Rocket Design](CAD/rocket_design.png)
![CAD Assembly](CAD/assembly.png)

### Description
CAD files are essential for ensuring the rocket's components fit together correctly and are robust enough to withstand the forces experienced during launch and flight. The CAD designs include detailed models of the rocket body, fins, nose cone, and other critical components.

### Design Highlights
- `rocket_body.sldprt`: SolidWorks file for the main body of the rocket.
- `fins.sldprt`: SolidWorks file for the rocket fins.
- `nose_cone.sldprt`: SolidWorks file for the rocket's nose cone.

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

## Installation
To install the software and code for this project, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/rocket-project.git