---
layout: page
title: Results
permalink: /results
---

TODO: insert pictures/videos here

# Results
TODO: yada yada it works

# Future Work
TODO

## MCU
TODO: talk about bug
TODO: adjusting sensitivity using software, ADC working so should be simple, just need different circuit from piezo to MCU

## FPGA
Currently, the FPGA interface to the 32 x 32 LED matrix display is mostly generic except for the colors of hits and misses. The rest of the display is only white or off. To allow for more customizable colors, the interface can be expanded to deal with RGB data. This would triple the amount of data the FPGA must deal with, which will likely require using the FPGA memory rather than using 1024-bit buses to the interface module.

The FPGA can also implement SPI to read data from the MCU rather than receiving all data through GPIO. Using SPI would allow for much more information to be received, such as the exact score a player earns for a given round, the high score for the current session, the accuracy of the player's hits, etc. 
