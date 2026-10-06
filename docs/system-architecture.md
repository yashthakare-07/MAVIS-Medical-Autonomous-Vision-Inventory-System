# M.A.V.I.S. System Architecture

## Overview

M.A.V.I.S. uses a three-layer Edge-to-Cloud architecture for healthcare micro-logistics.

The system consists of:

1. Cloud Layer
2. Edge Computation Layer
3. Physical Robot Layer

## Cloud Layer

The Care Link Dispatch Portal provides:

- User interface
- Inventory monitoring
- Order placement
- REST API communication
- JSON-based inventory state management

## Edge Computation Layer

The edge layer consists of:

- ESP32 microcontroller
- Wi-Fi communication
- Low-level robot control
- Motor actuation
- IR sensor processing
- Smartphone IP Webcam
- OpenCV QR verification

## Physical Robot Layer

The physical platform consists of:

- Differential-drive glass chassis
- IR sensor array
- L298N motor driver
- Two geared DC motors
- Lithium-ion battery
- DC-DC buck converter
- Caster wheel

## System Flow

```text
User
  |
  v
Care Link Dispatch Portal
  |
  v
Flask Server
  |
  v
HTTP Dispatch Command
  |
  v
ESP32
  |
  v
Motor Driver
  |
  v
Differential Drive Robot
  |
  v
IR Navigation
  |
  v
Storage Node
  |
  v
QR Verification
  |
  v
Inventory Update
