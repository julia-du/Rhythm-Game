---
layout: page
title: Documentation
permalink: /docs
---

# Schematic

![Schematic](./assets/schematics/Schematic.png)

# Source Code Overview

The source code for the project is located in the GitHub repository [here](https://github.com/julia-du/Rhythm-Game/tree/main/src).
- FPGA
    - main.sv: SystemVerilog for the FPGA
    - pins.pdc: Pin mappings for the FPGA
    - LED Display Patterns: folder containing text files for the various patterns for the LED display; read by main.sv
- CAD
    - LED-Display-Stand.sldprt/.stl: CAD of stand used for the LED display; 2 of these were 3D printed and attached to the display with 10mm M3 screws
    - Drum.sldprt/.x_t: CAD of the mechanical design of the drum
- MCU
    - main.c: C code for MCU
    - lib files: source and header files for used peripherals and the beatmap

# Bill of Materials

| Part | Part Number | Vendor | Unit Price | Quantity | Link |
| ---- | ----------- | ------ | ---------- | -------- | ---- |
| Piezo Sensor | 1740 | Adafruit | $0.95 | 6 | [link](https://www.adafruit.com/product/1740) |
| 32 x 32 LED Matrix Display | 2026 | Adafruit | $34.95 | 1 | [link](https://www.adafruit.com/product/2026) |
| Speaker | CT12E-05T204-8 | Stockroom | N/A | 1 | N/A |
| Low Voltage Audio Power Amp | LM386 | Stockroom | N/A | 1 | N/A |
| Single Supply CMOS Op Amp | MCP6002 | Stockroom | N/A | 1 | N/A |
| FPGA | UPDUINO V3.0 | MicroPs Kit | N/A | 1 | N/A |
| MCU | STM32L432KC | MicroPs Kit | N/A | 1 | N/A |
| 1/2" Plywood | N/A | Wood Shop | N/A | 1 | N/A |
| 10 mm M3 Screw | N/A | Stockroom | N/A | 4 | N/A |
| 10-24 Screw | N/A | Stockroom | N/A | 6 | N/A |
| 10-24 Hex Nut | N/A | Stockroom | N/A | 6 | N/A |

**Total cost: $39.70**
