# Pioneer Rocketry Flight Computer V3

Pioneer Rocketry’s third-generation flight computer is a high-performance, STM32-based avionics system designed for experimental high-power rockets. It provides state estimation, real-time telemetry, staging logic, and fully active control capabilities.
This repository contains the firmware, test tools, and documentation for the system.

---

## 🚀 Hardware Overview

### **Primary Components**
| Function | Component |
|---------|-----------|
| Main MCU | **STM32F446RE** |
| High-G IMU (Accel + Gyro) | **LSM6DSV32X** |
| Magnetometer | **MMC5603** |
| Barometer | **MS5607-02BA03** |
| GPS | **u-blox SAM-M8Q** |
| Radio | **LoRa SX1262** |

### **Additional Hardware Features**
- 3 independent **pyrotechnic deployment channels**
- Breakouts for **I²C** and **SPI** expansion
- Support for **4 servo channels** (vectoring, control surfaces, etc.)
- USB/Serial connectivity for configuration and simulation
- Ruggedized PCB designed for high-G, high-speed flight

---

## 🧠 Software Overview

The firmware is written in modern **C++**, using HAL for low-level drivers and a modular class-based architecture for sensors, telemetry, and estimation modules.

### **Key Capabilities**
- **State estimation** for:
  - Attitude (roll, pitch, yaw)
  - Position (x, y, z)
  - Velocity and angular rates
  *(Uses a custom multi-sensor Kalman Filter.)*
- **Live telemetry** over LoRa
- **Event-driven flight logic**, including:
  - Ascent/descent detection
  - Apogee detection
  - Staging and separation logic
  - Deployment sequencing
- **Active Control**
  - Servo-driven stabilization or thrust-vectoring
  - Optional closed-loop controls for advanced tests
- **Fault-tolerant design**
  - Sensor redundancy
  - Fail-safe deployment rules
  - Logging of all states and events

---

## 🧪 Testing Strategy

### **Hardware-in-the-Loop Testing (HILT)**
HILT allows the flight computer to run full flight software using simulated sensor data.

- The board connects to a PC via USB/Serial.
- A custom simulator streams “fake” IMU, barometer, and GPS data.
- The firmware behaves exactly as if it were in flight.
- Allows testing:
  - Sensor fusion
  - State transitions
  - Staging logic
  - Deployment logic
  - Data logging and telemetry

This framework accelerates development and eliminates many failure modes before field testing.

---

### **Field & Flight Testing**
Flight testing follows a staged, risk-managed approach:

1. **Initial flights:** \
   Validate basic state estimation (position, velocity, attitude) and state transitions.

2. **Intermediate flights:** \
   Add automated apogee deployment, then drogue/main testing, then multistage events.

3. **Advanced flights:**
   - High-speed, high-G tests
   - Full staging logic
   - Active control (servo-active fins or TVC)
   - Redundancy and fault recovery tests

Each successful flight unlocks the next feature set, ensuring safe and repeatable progress toward fully active control.

---

## 📂 Repository Structure

```
/Code
    /Core
        /Src
        /Inc
    /lib
/Datasheets
/PCB
```