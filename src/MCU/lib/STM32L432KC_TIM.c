// STM32F401RE_TIM.c
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"


void initTIM(TIM_TypeDef * TIMx){
  // Set prescaler to give 1 ms time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e3));

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR = ms;// Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

void initSpeakerTIM(){
  // Turn on clock domains
  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
  RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  RCC->APB2ENR |= (RCC_APB2ENR_TIM16EN);
  
  // set up Speaker and Beat Clock
  pinMode(SPEAKER_PIN, GPIO_ALT);
  pinMode(BEAT_CLOCK, GPIO_ALT);
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL2, 14);
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 1);

  // set clock enable to 1
  TIM2->CR1 |= (TIM_CR1_CEN);
  TIM15->CR1 |= (TIM_CR1_CEN); 
  TIM16->CR1 |= (TIM_CR1_CEN);
  
  // set UDIS to 0
  TIM2->CR1 &= ~(TIM_CR1_UDIS);
  TIM15->CR1 &= ~(TIM_CR1_UDIS); 
  TIM16->CR1 &= ~(TIM_CR1_UDIS); 
  
  // clear prescaler
  TIM2->PSC &= ~(TIM_PSC_PSC);
  TIM15->PSC &= ~(TIM_PSC_PSC);
  TIM16->PSC &= ~(TIM_PSC_PSC);
  
  // setting prescaler
  uint32_t psc_div_duration = (uint32_t) ((SystemCoreClock/1e3));
  TIM2->PSC |= _VAL2FLD(TIM_PSC_PSC, psc_div_duration-1); // beat clock
  TIM15->PSC |= _VAL2FLD(TIM_PSC_PSC, 12); // frequency
  TIM16->PSC |= _VAL2FLD(TIM_PSC_PSC, psc_div_duration-1); // duration

  // setting up PWM: TIM15
  TIM15->EGR |= (TIM_EGR_UG); // force update generation
  TIM15->CCMR1 |= _VAL2FLD(TIM_CCMR1_OC1M, 7); // enable PWM mode 2
  TIM15->BDTR |= (TIM_BDTR_MOE); // main output enable
  TIM15->CCMR1 |= (TIM_CCMR1_OC1PE); // enable CCR preload
  TIM15->CR1 |= (TIM_CR1_ARPE); // enable auto-reload preload
  TIM15->CCER |= (TIM_CCER_CC1E); // enable CCR1 as output
  
  // setting up PWM: TIM2
  TIM2->EGR |= (TIM_EGR_UG); // force update generation
  TIM2->CCMR1 |= _VAL2FLD(TIM_CCMR1_OC1M, 7); // enable PWM mode 2
  TIM2->BDTR |= (TIM_BDTR_MOE); // main output enable
  TIM2->CCMR1 |= (TIM_CCMR1_OC1PE); // enable CCR preload
  TIM2->CR1 |= (TIM_CR1_ARPE); // enable auto-reload preload
  TIM2->CCER |= (TIM_CCER_CC1E); // enable CCR1 as output
}

void delay_duration(int ms){
  // Setting ARR for duration timer
  TIM16->ARR &= ~(TIM_ARR_ARR); // clear ARR
  TIM16->ARR |= _VAL2FLD(TIM_ARR_ARR, ms); 

  // Setting up waveform for beat clock
  TIM2->EGR |=(TIM_EGR_UG); // force update generation
  TIM2->ARR &= ~(TIM_ARR_ARR); // clear ARR
  TIM2->CCR1 &= ~(TIM_CCR1_CCR1); // clear CCR1
  TIM2->ARR |= _VAL2FLD(TIM_ARR_ARR, ms); 
  TIM2->CCR1 |= _VAL2FLD(TIM_CCR1_CCR1, ms/2); // setting duty cycle to 50%
  TIM2->EGR |=(TIM_EGR_UG); // force update generation
}

void delay_frequency(int desired_freq){
  // Setting up waveform for note pitch
  TIM15->EGR |=(TIM_EGR_UG); // force update generation 
  TIM15->ARR &= ~(TIM_ARR_ARR); // clear ARR
  TIM15->CCR1 &= ~(TIM_CCR1_CCR1); // clear CCR1      
  TIM15->ARR |= _VAL2FLD(TIM_ARR_ARR, 24E6/(13*desired_freq)); 
  TIM15->CCR1 |= _VAL2FLD(TIM_CCR1_CCR1, 24E6/(26*desired_freq)); // setting duty cycle to 50%
  TIM15->EGR |=(TIM_EGR_UG); // force update generation 
}  
