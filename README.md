# M.A.V.I.S.

## Medical Autonomous Vision & Inventory System

M.A.V.I.S. is a compact, IoT-enabled Autonomous Mobile Robot (AMR) designed to integrate digital healthcare inventory management with physical medicine dispatch operations.

The system follows a hybrid Edge-to-Cloud architecture in which high-level inventory management, order processing, and computer vision are handled by a centralized Python Flask server, while the ESP32-based robotic platform performs low-level navigation and actuation.

---

## Project Objective

The system addresses the disconnect between digital healthcare inventory platforms and physical medicine retrieval.

M.A.V.I.S. combines:

- Digital inventory management
- Autonomous line-following navigation
- Wireless robot dispatch
- QR-based spatial verification
- Computer vision
- Physical-to-digital inventory synchronization

---

## System Architecture

The system is divided into three primary layers:

### 1. Cloud Layer

The Care Link Dispatch Portal provides:

- User interface
- Inventory monitoring
- Order placement
- REST API communication
- JSON-based inventory state management

### 2. Edge Computation Layer

The edge layer consists of:

- ESP32 microcontroller
- Wi-Fi communication
- Low-level control
- Motor actuation
- IR sensor processing
- Smartphone IP Webcam
- OpenCV QR verification

### 3. Physical Robot Layer

The robot consists of:

- Differential-drive glass chassis
- IR sensor array
- L298N motor driver
- Two geared DC motors
- Lithium-ion battery
- DC-DC buck converter
- Caster wheel

---

## Operating Principle

1. A medical inventory dispatch is initiated through the Care Link interface.
2. The Flask server sends a wireless command to the ESP32.
3. The ESP32 executes the navigation command.
4. The robot follows the predefined IR-based path.
5. The robot reaches the designated storage node.
6. The smartphone IP Webcam provides the visual input.
7. OpenCV detects and decodes the node QR code.
8. The decoded identifier is compared with the active dispatch request.
9. After successful verification, the corresponding inventory state is updated.

The project uses computer vision primarily for node verification rather than continuous navigation.

---

## Communication

The documented wireless dispatch command follows:

`GOTO,<node_id>`

Communication between the Flask server and ESP32 is performed using HTTP requests over a local Wi-Fi network.

---

## Vision Verification

The vision subsystem uses:

- Smartphone IP Webcam
- HTTP image acquisition
- NumPy
- OpenCV
- QRCodeDetector

Example inventory identifier:

`store1_medA`

---

## Inventory Management

The centralized Flask application maintains inventory information using a structured JSON file.

Inventory modification is performed after successful physical node verification.

Thread synchronization is used to protect shared inventory state during concurrent operations.

---

## Hardware

| Component | Function |
|---|---|
| ESP32 | Edge control and communication |
| IR Sensor Array | Line following |
| L298N | Motor driving |
| Geared DC Motors | Differential-drive motion |
| Li-ion Battery | Power source |
| DC-DC Buck Converter | Voltage regulation |
| Smartphone | IP Webcam |
| Glass Chassis | Mechanical platform |
| Caster Wheel | Mechanical support |

---

## Software

- Python
- Flask
- REST API
- OpenCV
- NumPy
- Requests
- urllib
- JSON
- ESP32 firmware environment

---

## Reported Results

The prototype was tested in a controlled laboratory environment.

Reported observations include:

- Accurate IR-based path tracking
- Stable ESP32 operation under reported conditions
- Stable Wi-Fi operation without ESP32 resets during observed current spikes
- Successful QR-based node verification
- QR decoding in under 200 ms after robot stabilization
- Inventory decrement following successful verification

---

## Known Limitation

The primary integration limitation identified during testing was WLAN state synchronization.

The ESP32 arrival HTTP request could occasionally fail because of:

- Wi-Fi latency
- TCP packet loss
- HTTP timeout behavior

This could leave the server waiting for the robot-arrival event and prevent the QR verification process from starting.

A manual UI override was therefore implemented in the Care Link frontend to allow the operator to inject the arrival event when required.

---

## Future Scope

The project report identifies three principal future enhancements:

### MQTT Communication

Replace HTTP communication with MQTT to improve message reliability using its publish-subscribe architecture and QoS mechanisms.

### Onboard Vision

Integrate an ESP32-CAM to move QR processing closer to the robot.

### Automated Payload Handling

Introduce a mechanized payload handling system such as a robotic arm or motorized payload mechanism.

---

## Repository Scope

This repository documents and represents the M.A.V.I.S. system according to the project report.

No additional hardware, algorithms, performance claims, or system capabilities are presented as existing functionality unless documented by the project.

---

## Author

**Yash Thakare**

B.Tech — Automation and Robotics Engineering

JSPM's Rajarshi Shahu College of Engineering, Pune
