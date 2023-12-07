// STM32L432KC_DMA.c
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// Source code for DMA functions

#include "STM32L432KC_DMA.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"


volatile uint16_t userInput[3];

void configureDMA(){
  // Turn on DMA clock domain
  RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN);

  DMA1_Channel1->CPAR |= _VAL2FLD(DMA_CPAR_PA, (uint32_t)&ADC1->DR); // peripheral address
  DMA1_Channel1->CMAR |= _VAL2FLD(DMA_CMAR_MA, (uint32_t)&userInput); // memory address
  DMA1_Channel1->CNDTR |= _VAL2FLD(DMA_CNDTR_NDT, 2); // number of data to transfer

  DMA1_Channel1->CCR |= _VAL2FLD(DMA_CCR_PL, 2); // channel priority high
  DMA1_Channel1->CCR &= ~(DMA_CCR_DIR); // read from peripheral
  DMA1_Channel1->CCR |= (DMA_CCR_CIRC); // enable circular mode
  DMA1_Channel1->CCR |= (DMA_CCR_MINC); // enable memory increment mode
  DMA1_Channel1->CCR |= (DMA_CCR_MSIZE); // memory size: 16 bits

  DMA1_Channel1->CCR |= (DMA_CCR_EN); // enable channel
}

int USER_RIGHT(){
  if(userInput[0] >= 180){
    return 1;
  } else {
    return 0;
  }
}

int USER_LEFT(){
  if(userInput[2] >= 180){
    return 1;
  } else {
    return 0;
  }
}



