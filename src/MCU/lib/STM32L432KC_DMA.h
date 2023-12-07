// STM32L432KC_DMA.h
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// Header for DMA functions

#ifndef STM32L4_DMA_H
#define STM32L4_DMA_H

#include <stdint.h>
#include <stm32l432xx.h>



///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

// Initializes DMA peripheral
void configureDMA();

// reads temporary memory address and returns 1 or 0 depending on value in address (for left input)
int USER_LEFT();

// reads temporary memory address and returns 1 or 0 depending on value in address (for right input)
int USER_RIGHT();
#endif