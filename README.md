# JKH 3-Sensor to 1 Output Project

This repository contains the hardware design files and firmware for the JKH 3-Sensor to 1 Output project. The system is built around a PCB that interfaces with three infrared (IR) type sensors and combines their signals to provide a single output.

## Project Overview

The goal of this project is to read data from three IR sensors and output a single consolidated signal. This can be useful in applications where multiple sensor inputs need to be merged or used as a logical combination (e.g., any sensor active = output active).

Key components:

- **PCB** design located in the `pcb/` folder containing the schematic and board layout files.
- **Firmware** written for a microcontroller using PlatformIO; source code resides under `Firmware/JKH_3_sensor_1/src/main.cpp`.
- **BOM** and datasheets are included to assist with assembly and sourcing components.

## Hardware

Folders:

- `pcb/` - Contains PCB design files (`.json` exported from EasyEDA) and related documentation.
- `sch/` - The schematic source files.
- `datasheet/` - Component datasheets referenced by the design.
- `BOM_JKH_3_sensor_1_2026-03-10.csv` - Bill of Materials for the board.

### PCB Features

- Connectors for three IR sensor modules.
- Microcontroller footprint (defined by the firmware requirements).
- Single output connector to transmit the combined sensor result.

## Firmware

The firmware is located in `Firmware/JKH_3_sensor_1` and built with PlatformIO.

### Getting Started

1. Install [PlatformIO](https://platformio.org/) in your development environment (e.g., VS Code).
2. Open the `Firmware/JKH_3_sensor_1` folder in the PlatformIO project.
3. Build the project:

   ```sh
   pio run
   ```

4. Upload to the target microcontroller:

   ```sh
   pio run --target upload
   ```

### Description

The code reads analog/digital values from three IR sensors, performs any necessary filtering or logic, and sets a single output pin accordingly. See `src/main.cpp` for implementation details.

## Assembly

Refer to the BOM and datasheets for component selection and placement. Use the schematic files in `sch/` for reference when populating the PCB.

## License

This project is distributed under the [MIT License](LICENSE).

See the `LICENSE` file for the full terms and conditions.

## Contact

For questions or support, contact the project maintainer.
