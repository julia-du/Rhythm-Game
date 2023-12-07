// STM32L432KC_ADC.c
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// Source code for ADC functions

#include "STM32L432KC_ADC.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"

// ADC Voltage Regulator Start-Up Time [us]
int tadcvreg = 20; 

// Simple delay function to use a bunch of NOPs to stall the processor
void delay(int ms) {
   while (ms-- > 0) {
      volatile int x=1000;
      while (x-- > 0)
         __asm("nop");
   }
}

void configureADC() {
  // Turn on GPIOB clock domain
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
  
  //Set up ADC clock
  RCC->AHB2ENR |= (RCC_AHB2ENR_ADCEN);
  RCC->CCIPR |= _VAL2FLD(RCC_CCIPR_ADCSEL, 3); 
  ADC1->CR |= _VAL2FLD(ADC_CCR_CKMODE, 0);
  
  // assign ADC pins
  pinMode(PIEZO_RIGHT, GPIO_ANALOG);
  pinMode(PIEZO_LEFT, GPIO_ANALOG);
  
  // Calibration setup
  ADC1->CR &= ~(ADC_CR_DEEPPWD); // Disable deep power down 
  ADC1->CR |= ADC_CR_ADVREGEN; // Enable ADC voltage regulator
  delay(tadcvreg); // wait for regulator start-up time
  ADC1->CR &= ~(ADC_CR_ADEN); // Ensure ADC disabled
  ADC1->CR &= ~(ADC_CR_ADCALDIF); // Input mode single-ended
  ADC1->CR |= ADC_CR_ADCAL; // Enable calibration
  while(ADC1->CR & ADC_CR_ADCAL){} // wait for calibration mode to end

  // Enabling ADC
  ADC1->CR |= ADC_CR_ADEN; // Enable ADC
  while((ADC1->ISR & ADC_ISR_ADRDY) != 1){} // wait for ADC to be ready
  ADC1->SMPR2 |= _VAL2FLD(ADC_SMPR2_SMP15, 1); // Sample time 2.5
  ADC1->SMPR2 |= _VAL2FLD(ADC_SMPR2_SMP16, 1); // Sample time 2.5
  ADC1->IER |= (ADC_IER_EOCIE); // Enable EOC interrupt 
  ADC1->CFGR |= (ADC_CFGR_DMAEN); // Enable DMA
  ADC1->CFGR |= (ADC_CFGR_DMACFG); // DMA circular mode

  // Conversion setup
  ADC1->CFGR |= (ADC_CFGR_CONT); // Continous conversions
  ADC1->SQR1 |= _VAL2FLD(ADC_SQR1_SQ1, 16); // Scan channel 16 (PB1) first (right)
  ADC1->SQR1 |= _VAL2FLD(ADC_SQR1_SQ2, 15); // Scan channel 15 (PB0) second (left)
  ADC1->SQR1 |= _VAL2FLD(ADC_SQR1_L, 1); // Scan two channels
  ADC1->CFGR |= _VAL2FLD(ADC_CFGR_RES, 2); // 8-bit resolution
}
