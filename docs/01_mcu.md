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
The MCU uses three peripherals: `ADC`, `DMA`, and `TIM`. The `ADC` peripheral is responsible for converting the analog voltage from the piezo sensors to DC voltage. The `DMA` peripheral is responsible for data transfers between the ADC registers and a temporary memory address. The `TIM` peripheral is responsible for generating the beat clock, the frequency waveform that goes to the speaker, and a duration timer. 

## ADC and DMA

The piezo sensors were connected to a comparator circuit that sends 3.3 V to the `ADC` pin if the sensor generates a voltage above 1.1 V. The sensor is also connected in parallel with a resetting resistor that has a value of 10 MΩ. When the generated voltage has dissipated to a value below the threshold, the current will then flow through the resetting resistor, helping to discharge any leftover voltage. When the `ADC` pin receives the 3.3 V, the data register will generate a value anywhere between 0 and 255. For both sensors, the data register will generate a value of at least 180. 

To process both sensors, the `ADC` peripheral was configured such that there would be continous conversions with an 8-bit resolution; `ADC` would scan the right sensor pin first (channel 16) and then the left sensor pin (channel 15). Each channel was set to have a sampling time of 2.5 ADC clock cycles.

Since there is more than one channel being converted at a time, the `DMA` peripheral was set up to manage these conversions. For this, the `DMAEN` and `EOCIE` bits were set to 1 such that channel interrupts and `DMA` mode was enabled for `ADC`. After a channel has been converted, the channel interrupt will be written to 1, signaling a `DMA` request to be generated. This allows the transfer of the converted data from the `ADC_DR` register to a specified temporary memory address. 

## TIM

To match the FPGA, the MCU's system clock was set to be 24 MHz. Three timers were used in this design: `TIM2`, `TIM15`, and `TIM16`. Both `TIM2` and `TIM15` were configured to their PWM mode, where a signal is generated with a frequency determined by the value of the auto reload register (`TIMx_ARR`) and a duty cycle determined by the value of the capture/compare register (`TIMx_CCRx`). For simplicity, both waveforms have a duty cycle of 50%. `TIM2` represents the duration of each note; this is the beat clock. `TIM15` represents the frequency of the note; this is speaker pin. `TIM16` also represents the duration of each note, but instead of being configured into an output waveform, it was used in a looping logic. 

There is an amplifier circuit between the speaker pin and the speaker because the current draw of the speaker is greater than the max current output of the MCU pin. The internal gain of the amplifier is 20 and the minimum gain needed is 10, thus the circuit was wired according to the schematic below, exluding any resistor and bypass capacitors. 
![AudioAmplifierCircuit](./assets/schematics/AudioAmplifierCircuit.png)

## main.c

TODO:

# Sources
TODO: cite engineering stack exchange in reference for resetting resistor + taiko drum design