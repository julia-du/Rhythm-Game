// STM32L432KC_ADC.h
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// Header for ADC functions

#ifndef STM32L4_ADC_H
#define STM32L4_ADC_H

#include <stdint.h>
#include <stm32l432xx.h>

#define PIEZO_RIGHT PB1
#define PIEZO_LEFT PB0

// CH5: PA0
// CH6: PA1
// CH7: PA2
// CH8: PA3
// CH9: PA4
// CH10: PA5
// CH11: PA6
// CH12: PA7
// CH15: PB0
// CH16: PB1



///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

// Initializes ADC peripheral
void configureADC();

#endif