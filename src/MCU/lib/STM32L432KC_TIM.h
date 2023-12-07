// STM32L432KC_TIM.h
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// Header for TIM functions

#ifndef STM32L4_TIM_H
#define STM32L4_TIM_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>  // CMSIS device library include
#include "STM32L432KC_GPIO.h"

#define SPEAKER_PIN PA2
#define BEAT_CLOCK PA5

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initTIM(TIM_TypeDef * TIMx);
void delay_millis(TIM_TypeDef * TIMx, uint32_t ms);


// initialize timers used for rhythm game
void initSpeakerTIM();

/* Sets up note duration timer and beat clock
  - ms: duration of note
*/
void delay_duration(int ms);

/* Sets up note pitch waveform
  - desired_freq: pitch/frequency of note
*/
void delay_frequency(int desired_freq);
#endif