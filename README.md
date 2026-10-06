# M.A.V.I.S.

## Medical Autonomous Vision & Inventory System

M.A.V.I.S. is a compact, IoT-enabled Autonomous Mobile Robot (AMR) designed to integrate digital healthcare inventory management with physical medicine dispatch operations.

The system follows a hybrid Edge-to-Cloud architecture in which high-level inventory management, order processing, and computer vision are handled by a centralized Python Flask server, while the ESP32-based robotic platform performs low-level navigation and actuation.

---

## System Overview

![M.A.V.I.S. Prototype](docs/images/mavis-prototype.png)

M.A.V.I.S. integrates a mobile robotic platform, medical inventory nodes, QR-based spatial verification, computer vision, and centralized inventory management into a single healthcare micro-logistics workflow.

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

![M.A.V.I.S. System Architecture](docs/images/system-architecture.png)

The system is divided into three primary layers.

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

## Hardware Architecture

![M.A.V.I.S. Hardware Circuit](docs/images/hardware-circuit.png)

The hardware architecture integrates the ESP32 controller, IR sensor array, L298N motor driver, geared DC motors, power system, and wireless interfaces.

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

## Implementation Flow

![M.A.V.I.S. Implementation Flow](docs/images/implementation-flowchart.png)

The implementation combines the Flask backend, ESP32 robot controller, IR-based navigation, IP Webcam image acquisition, OpenCV QR detection, and inventory verification.

---

## Communication

The documented wireless dispatch command follows:

`GOTO,<node_id>`

Communication between the Flask server and ESP32 is performed using HTTP requests over a local Wi-Fi network.

The ESP32 firmware provides an HTTP command interface for receiving robot dispatch commands and exposes status and health interfaces.

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

The QR identifier is used for physical node verification before inventory modification.

---

## Inventory Management

The centralized Flask application maintains inventory information using a structured JSON file.

Inventory data is stored in:

```text
data/inventory.json
