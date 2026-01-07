# Pioneer Rocketry Flight Computer

Pioneer Rocketry’s latest generation flight computer for high power, STM32-based avionics system designed for experimental high-power rockets. It provides state estimation, real-time telemetry, staging logic, and fully active control capabilities.
This repository contains the firmware, test tools, and documentation for the system.


---

## Prerequisites

Before building this project, ensure you have the following tools installed and added to your system `PATH`:

1.  **[Visual Studio Code](https://code.visualstudio.com/)** or **[STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html#get-software)**
2.  **[STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)** (v6.13 or newer) - Required to generate hardware config.
3.  **Make** Use `winget install ezwinports.make` or `choco install make` - Build system generator.
4.  **[Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)** - The compiler (e.g., `arm-none-eabi-gcc`).
5.  **[Ninja Build](https://github.com/ninja-build/ninja/releases)** (Optional) - A faster build system than Make.

### VS Code Extensions
Install the following extensions within VS Code:
* **[Cortex Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug)**

---

## Build Instructions

### 1. Clone and Open
Clone the repository and open the folder in VS Code:
```bash
git clone --recurse-submodules https://github.com/Pioneer-Rocketry/Flight-Computer.git
```
### 2. Build the Firmware

You can build the project using the status bar at the bottom or the Command Palette:

* **Status Bar:** Click **Build** (or the generic "Gear" icon).
* **Shortcut:** Press `F7`.

*Successful build output will create a `Code.elf` and `Code.bin` file in the `./Code/build/` directory.*

### 3. Flash and Debug

Connect your STM32 board via ST-Link.

1. Press `F5` to start debugging.
2. The extension will automatically:
* Build the latest code.
* Flash the board using OpenOCD/ST-Link.
* Open the debug console.

### Troubleshooting

* **"Multiple target patterns" error:** Ensure your project folder path contains **NO SPACES** (e.g., use `C:\Dev\Project` instead of `C:\Users\My Name\Project`).
* **C++ vs C:** If `main.cpp` is not being compiled, check `CMakeLists.txt` to ensure `CXX` is enabled and `main.cpp` is listed in the source files. If main.c is in the CDEF, remove it and delete the file.


## Hardware Overview

### **Primary Components**

| Function | Component |
|----|----|
| Main MCU | **STM32F446RE** |
| High-G IMU (Accel + Gyro) | **LSM6DSV32X** |
| Magnetometer | **MMC5603** |
| Barometer | **MS5607-02BA03** |
| GPS | **u-blox SAM-M8Q** |
| Radio | **LoRa SX1262** |

### **Additional Hardware Features**

* 3 independent **pyrotechnic deployment channels**
* Breakouts for **I²C** and **SPI** expansion
* Support for **4 servo channels** (vectoring, control surfaces, etc.)
* USB/Serial connectivity for configuration and simulation
* Ruggedized PCB designed for high-G, high-speed flight


---

## Software Overview

The firmware is written in modern **C++**, using HAL for low-level drivers and a modular class-based architecture for sensors, telemetry, and estimation modules.

### **Key Capabilities**

* **State estimation** for:
  * Attitude (roll, pitch, yaw)
  * Position (x, y, z)
  * Velocity and angular rates
    *(Uses a custom multi-sensor Kalman Filter.)*
* **Live telemetry** over LoRa
* **Event-driven flight logic**, including:
  * Ascent/descent detection
  * Apogee detection
  * Staging and separation logic
  * Deployment sequencing
* **Active Control**
  * Servo-driven stabilization or thrust-vectoring
  * Optional closed-loop controls for advanced tests
* **Fault-tolerant design**
  * Sensor redundancy
  * Fail-safe deployment rules
  * Logging of all states and events


---

## Testing Strategy

### **Hardware-in-the-Loop Testing (HILT)**

HILT allows the flight computer to run full flight software using simulated sensor data.

* The board connects to a PC via USB/Serial.
* A custom simulator streams “fake” IMU, barometer, and GPS data.
* The firmware behaves exactly as if it were in flight.
* Allows testing:
  * Sensor fusion
  * State transitions
  * Staging logic
  * Deployment logic
  * Data logging and telemetry

This framework accelerates development and eliminates many failure modes before field testing.


---

### **Field & Flight Testing**

Flight testing follows a staged, risk-managed approach:


1. **Initial flights:** Validate basic state estimation (position, velocity, attitude) and state transitions.
2. **Intermediate flights:** Add automated apogee deployment, then drogue/main testing, then multistage events.
3. **Advanced flights:**
   * High-speed, high-G tests
   * Full staging logic
   * Active control (servo-active fins or TVC)
   * Redundancy and fault recovery tests

Each successful flight unlocks the next feature set, ensuring safe and repeatable progress toward fully active control.


---

## Repository Structure


```
Flight-Computer/
├── Code/
│   ├── Core/
│   │   ├──Src/
│   │   └── Inc/
│   └── lib/
├── Datasheets/
└── PCB/
```



