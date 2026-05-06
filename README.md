# Alex: Search & Rescue Robot (CG2111A)

> NUS Engineering Principles and Practices II - AY24/25 Semester 2

## Overview
Alex is a teleoperated search-and-rescue robot designed to navigate a simulated disaster environment (Moonbase CEG), identify injured astronauts, and perform rescue actions within 8 minutes. 

- 🟢 **Green astronaut** → Release a medpack
- 🔴 **Red astronaut** → Grab and transport to the Parking Area
- 🗺️ **Simultaneously** maps the environment in real-time using SLAM

## System Architecture
Alex uses a two-tier computing architecture: 

- **Raspberry Pi** - High-level control, SLAM mapping (BreezySLAM), camera feed server, and tele-operator interface via SSH
- **Arduino Mega 2560** - Low-level firmware, motor control, colour sensing, IMU-based rotation, and servo detection

Communication between the RPi and Arduino is handled over UART, using a custom packet structure with magic number and checksum-based error detection. 

## Hardware Components

| Component | Purpose |
|---|---|
| RPLidar | Environment mapping (SLAM) |
| BMI160 IMU | Precise rotation tracking via gyroscope integration |
| Colour Sensor + Polariser | Red/Green astronaut identification |
| Front Servo (Claws) | Grabbing the Red Astronaut |
| Back Servo | Medpack release mechanism |
| RPi Camera | Live visual feed for tele-operator |
| Arduino Mega 2560 | Low-level motor and sensor control |

## Controls (WASD Scheme)

### Movement

| Key | Action |
|---|---|
| `W` | Move Forward |
| `S` | Move Backward |
| `A` | 90° Left Turn |
| `D` | 90° Right Turn |
| `Space` | Stop immediately |

### Nudging (Precise Control)

| Key | Action |
|---|---|
| `i` / `k` | Nudge Forward / Backward (200ms) |
| `j` / `l` | Nudge Left / Right (5°) |
| `I/J/K/L` | Same as above with custom input duration/angle |

### Miscellaneous

| Key | Action |
|---|---|
| `V` | Activate colour sensor |
| `M` / `N` | Open / Close claws |
| `R` | Release medpack |
| `Q` | Close connection |

## Key Algorithms

### IMU-Based Rotation (BMI160)
Integrates angular rate (gz) along the z-axis every 10ms to accumulate rotation angle. Motors stop once the target angle is reached, regardless of wheel traction.

### Colour Detection
Reads raw R, G, B values from the colour sensor. Classifies astronaut colour by comparing the ratio of red-to-green and green-to-red values. A polariser and baffle were added to reduce LED glare and light leakage for more accurate readings.

### SLAM Mapping
LiDAR data is fed into BreezySLAM on the RPi to generate a live 2D map. The map is visualized via matplotlib and viewed remotely using VNC. Alex's bounding box and claw bounds are overlaid on the map for precise positioning. 

### Communication & Error Handling
Packets use a magic number (`0xFCFDFEFF`) and XOR checksum for validation. Invalid packets return one of the three error responses: `RESP_BAD_PACKET`, `RESP_BAD_CHECKSUM`, or `RESP_BAD_COMMAND`. 

## File Structure

### Arduino (`/Alex`)

| File | Description |
|---|---|
| `Alex.ino` | Main Arduino firmware — motor control, packet handling |
| `BMI160.ino` | IMU-based rotation tracking |
| `Claw.ino` | Servo control for claws and medpack release |
| `ColourSensorMega.ino` | Colour sensor integration |
| `robotlib.ino` | Robot movement library |
| `constants.h` | Shared constants (speeds, pins, thresholds) |
| `packet.h` | Packet type and command definitions |
| `samples.h` | Colour sensor calibration samples |

### Raspberry Pi

| File | Description |
|---|---|
| `alex-pi.cpp` | Main RPi control program — tele-operator interface |
| `serialize.cpp` / `.h` | Packet serialization and deserialization |
| `serial.cpp` / `.h` | UART serial communication with Arduino |
| `buffer.cpp` / `.h` | Buffer management for packet assembly |
| `packet.h` / `constants.h` | Shared packet and constants definitions |

### Colour Sensor (`/ColourSensorMega`)

| File | Description |
|---|---|
| `ColourSensorMega.ino` | Standalone colour sensor sketch |
| `ColourSensorMega.h` | Colour sensor header |

### Scripts

| File | Description |
|---|---|
| `START_ALEX_PI.sh` | Launches the RPi control program |
| `START_CAMERA.sh` | Starts the camera feed server |
| `START_SLAM.sh` | Starts the SLAM mapping process |
