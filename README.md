# M.A.V.I.S. — Medical Autonomous Vision Inventory System

IoT-enabled autonomous medical logistics robot that combines ESP32-based navigation, Flask inventory management, and OpenCV QR-code verification for automated medicine dispatch.

![Demo](docs/demo.gif) <!-- TODO: add a demo GIF or robot photo -->

## Overview
M.A.V.I.S. moves medicines between storage nodes without manual handling. At each node, a camera reads a QR code and the system checks it against the inventory database before dispatching. <!-- TODO: confirm in one line what the robot physically does, e.g. line following, node-to-node routing -->

## System Architecture
```
IP Camera (IP Webcam) ──► OpenCV QR detection ──► Flask REST backend (JSON)
                                                      │  inventory + dispatch state
                                                      ▼
                                              ESP32 robot controller
                                         (navigation + motor control)
```

## Repository Structure
| Folder | Contents |
|---|---|
| `firmware/esp32/` | ESP32 robot firmware |
| `software/` | Vision pipeline and Flask backend |
| `hardware/` | Hardware design files <!-- TODO: schematic, wiring, BOM --> |
| `data/` | Inventory / sample data |
| `tests/` | Test scripts <!-- TODO: list what is tested --> |
| `docs/` | Documentation |

## Tech Stack
Python, OpenCV, Flask, ESP32 (Arduino/C++), REST API, JSON

## Getting Started
```bash
git clone https://github.com/yashthakare-07/MAVIS-Medical-Autonomous-Vision-Inventory-System.git
cd MAVIS-Medical-Autonomous-Vision-Inventory-System
pip install -r requirements.txt
# TODO: command to start the backend, e.g. python software/<file>.py
# TODO: how to flash firmware/esp32 and set Wi-Fi + server IP
# TODO: how to set the IP Webcam stream URL
```

## API
<!-- TODO: list endpoints, e.g. method, path, purpose, example JSON -->

## Results
| Metric | Value | Conditions |
|---|---|---|
| QR verification latency | `< 200 ms` <!-- TODO: replace with measured mean / max --> | controlled test, after robot stabilization, N = `TODO` trials |

## Limitations and Future Work
<!-- TODO: 2–3 honest points, e.g. controlled lighting, single-robot, no obstacle avoidance -->

## License
MIT — see [LICENSE.md](LICENSE.md).
