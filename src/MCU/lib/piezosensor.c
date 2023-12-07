// PIEZOSENSOR.c
// Allison Tsai
// aztsai@hmc.edu
// 12/7/2023
// Source code for piezo sensor functions
// Note: these files use GPIO output to process the piezo voltages. 
//       While they are not being used in the current design, they still will work in place of ADC + DMA. 
//       But for now, they are excluded from the build path.

#include "PIEZOSENSOR.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_RCC.h"
#include <string.h>
#include <stdio.h>



void configurePiezoSensor(){
  // Turn on GPIO clock domains
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

  // assign ADC pins
  pinMode(PIEZO_RIGHT, GPIO_INPUT);
  pinMode(PIEZO_LEFT, GPIO_INPUT);

  //pull-down resistors
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD9, 2);
  GPIOB->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD0, 2);
}

void readPiezoSensor(int pinL, int pinR){
  char hitL[5];
  char hitR[5];
  // reading from the DR A
  if(digitalRead(PIEZO_LEFT) != 0){
    togglePin(pinL);
    stpcpy(hitL, "hit");
  } else {
    stpcpy(hitL, "miss");
  }
  
  // reading from the DR B
  if(digitalRead(PIEZO_RIGHT) != 0){
    togglePin(pinR);
    stpcpy(hitR, "hit");
  } else {
    stpcpy(hitR, "miss");
  }

  printf("Left input: %s, Right input: %s \n", hitL, hitR);
}

