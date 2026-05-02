
The Maze Solver Bot demonstrates the application of embedded systems, robotics, and intelligent algorithms in autonomous navigation. It provides a practical platform to explore how machines can perceive their environment, make real-time decisions, and execute precise movements, which are essential in industries such as manufacturing, transportation, healthcare, and defense. 
The motivation for developing the Maze Solver Bot stems from the growing demand for autonomous systems capable of independent decision-making and efficient navigation. Robotics and automation are increasingly important in industries such as logistics, transportation, healthcare, and disaster management. Autonomous navigation allows machines to operate in dynamic environments without direct human intervention, and the Maze Solver Bot provides a practical platform to study and implement these systems in a controlled setting. 


## Project Overview
The project focuses on creating an intelligent, autonomous platform capable of secure indoor delivery, security patrolling, document sanitization, and real-time monitoring. It integrates navigation, monitoring, and safety features into a single, cost-effective system.
## Key Features
 * *Autonomous Navigation*: Uses line-following techniques for guided movement and ultrasonic sensing for obstacle detection and avoidance.
 * *Secure Delivery: Features a secure compartment that only opens after **OTP-based verification* via a mobile application, ensuring items reach the intended recipient.
 * *UV-C Sanitization*: Includes a relay-controlled UV-C module to sterilize documents or objects during transport.
 * *Real-time Surveillance: An **ESP32-CAM* module provides live video streaming for remote monitoring and enhanced security.
 * *Motion Detection: Integrated **PIR sensors* detect human presence and trigger real-time alerts.
## System Architecture
The system is built on a modular architecture consisting of hardware, control, and user interface modules.
### Hardware Components
 * *Controller*: ESP32 Microcontroller (Central Processing Unit with Wi-Fi/Bluetooth).
 * *Sensors*:
   * TCRT5000 IR sensors (Line tracking).
   * HC-SR04 Ultrasonic sensor (Obstacle detection).
   * Passive Infrared (PIR) sensor (Motion detection).
 * *Actuators*:
   * DC Motors with L298N Motor Driver.
   * Relay Module (UV Light Control).
   * Buzzer (Safety alerts).
### Software Stack
 * *Programming Language*: Embedded C/C++.
 * *Development Environment*: Arduino IDE.
 * *Mobile Application: Developed using the **Flutter* framework for cross-platform (Android/iOS) compatibility.
## Development Methodology
The project followed a structured lifecycle:
 1. *Requirement Analysis*: Identifying limitations in manual delivery and planning the system architecture.
 2. *Hardware Assembly*: Interfacing sensors and actuators with the ESP32.
 3. *Software Development*: Programming the navigation logic, OTP verification, and mobile app interface.
 4. *Testing*: Evaluating performance and reliability in controlled indoor environments.
## Results & Discussion
Experimental testing demonstrated stable performance under various conditions, with minimal response times. The robot successfully performed:
 * Accurate path tracking on designated lines.
 * Reliable obstacle detection and collision avoidance.
 * Secure document transfer through the integrated locking mechanism and mobile app.
## software
<img width="941" height="333" alt="image" src="https://github.com/user-attachments/assets/9c1e49f5-9d8b-42a2-8634-1f41af97bb96" />
<img width="924" height="314" alt="image" src="https://github.com/user-attachments/assets/d95c08f9-04e5-4661-a725-420a9a094309" />
<img width="948" height="328" alt="image" src="https://github.com/user-attachments/assets/1a745af3-7131-4bd4-8347-98c149d3c37f" />
## hardware
<img width="851" height="458" alt="image" src="https://github.com/user-attachments/assets/16e8932e-d86d-4426-9da8-10e1b09cd386" />
<img width="845" height="394" alt="image" src="https://github.com/user-attachments/assets/565a54b6-97a8-475a-9ac3-141f67423294" />
<img width="1448" height="1086" alt="image" src="https://github.com/user-attachments/assets/afa5e099-1d12-4b78-9d0b-88c4fd4a42f6" />

## Future Scope
 * *Advanced Patrolling*: Continuous environmental monitoring for enhanced security.
 * *AI-Based Navigation*: Implementation of autonomous decision-making and vision-based mapping.
 * *IoT Integration*: Further refinement of remote control and monitoring capabilities.



