---
layout: page
title: MCU Design
permalink: /mcu
---

# New Hardware
The new hardware used was Adafruit's small enclosed piezo sensors. Piezo devices can convert mechanical energy into electrical energy or vice versa. Thus, they were used to "sense" user inputs by translating vibration into readable voltage.

The two piezo sensors were attached to back of two drum pads. These pads were laser cut out of 1/8" plywood and screwed onto a circular base with stands using 10-24 button head screws and hex nuts. Both the base and stands were machined using a vertical bandsaw and were made from 1/2" plywood. To isolate the vibrations from the pads, rubber washers and TPU spacers were placed between the base and the pads. The CAD for the drum is available [here](https://github.com/julia-du/Rhythm-Game/tree/main/src/CAD).

# MCU System Design
![MCUBlockDiagram](./assets/schematics/MCUBlockDiagram.png)
The MCU uses three peripherals: `ADC`, `DMA`, and `TIM`.