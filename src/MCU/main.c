/*
File: main.c
Author: Allison Tsai
Email: aztsai@hmc.edu
Date: 12/7/2023
Purpose: Handles all logic of the rhythm game: Taiko
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////
int userPoints = 0;
int totalPoints = 78;
int Left_hit = 0;
int Right_hit = 0;
int Round = 0;
int Speed = 6;
int T;
int t;
char hitLmsg[5];
char hitRmsg[5];

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

 int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);
  
  configureDMA();
  configureADC();
  initSpeakerTIM();
  
  pinMode(HITL, GPIO_OUTPUT);
  pinMode(MISSL, GPIO_OUTPUT);
  pinMode(HITR, GPIO_OUTPUT);
  pinMode(MISSR, GPIO_OUTPUT);
  pinMode(ROUNDNUM0, GPIO_OUTPUT);
  pinMode(ROUNDNUM1, GPIO_OUTPUT);
  pinMode(GAMEOVER, GPIO_OUTPUT);
  pinMode(BEATL, GPIO_OUTPUT);
  pinMode(BEATR, GPIO_OUTPUT);
  
 
  /*
  // this while loop helps to quickly test if the piezo sensors are working properly on the drum pad
  while(1){
    ADC1->CR |= ADC_CR_ADSTART; // start regular conversions   
    while((ADC1->ISR & ADC_ISR_EOS) == 0){} // wait for EOS flag 
    int LEFT_INPUT = USER_LEFT();
    int RIGHT_INPUT = USER_RIGHT(); 

    if(LEFT_INPUT){ 
        stpcpy(hitLmsg, "hit");
    } else {
        stpcpy(hitLmsg, "miss");
    }

    if(RIGHT_INPUT){
       stpcpy(hitRmsg, "hit");
    } else {
        stpcpy(hitRmsg, "miss");
    }

    printf("Left input: %s, Right input: %s, \n", hitLmsg, hitRmsg);
    ADC1->ISR &= ~(ADC_ISR_EOS); // reset EOS flag
    ADC1->CR &= ~(ADC_CR_ADSTART); // stop conversions
  }
  */
  
  while (Round < 3){
    // writing round number to the FPGA
    if (Round == 1 || Round == 3){
      digitalWrite(ROUNDNUM0, 1);
    } else {
      digitalWrite(ROUNDNUM0, 0);
    }

    if (Round == 2 || Round == 3){
      digitalWrite(ROUNDNUM1, 1);
    } else {
      digitalWrite(ROUNDNUM1, 0);
    }
    
    
    userPoints = 0; // reset user points at the beginning of every round
    
    printf("Round: %d \n", Round);

    T = (1700*Speed)/6; // duration of a note in ms
    t = T*0.0625; // a sixteenth note in ms
     /*
    // dividng notes into sixteenth note
       // quarter note:     played 4 times  (.25)
       // eighth note:      played 2 times  (.125)
       // quarter dot note: played 6 times  (.375)
       // half note:        played 8 times  (.5)
       // whole note:       played 16 times (1)
    
    // current song in note_map: Tetris theme 
    */
    int note_map[][2] = {
    // pre measure 1
    {0, t}, {0, t}, {0, t}, {0, t},
    {0, t}, {0, t}, {0, t}, {0, t},
    {0, t}, {0, t}, {0, t}, {0, t},
    {0, t}, {0, t}, {0, t}, {0, t},

    // pre meaasure 2
    {0, t}, {0, t}, {0, t}, {0, t},
    {0, t}, {0, t}, {0, t}, {0, t},
    {0, t}, {0, t}, {0, t}, {0, t},
    {0, t}, {0, t}, {0, t}, {0, t},

    // measure 1
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {B4, t}, {B4, t},
    {C5, t}, {C5, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {C5, t}, {C5, t},
    {B4, t}, {B4, t},

    // measure 2
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {A4, t}, {A4, t},
    {C5, t}, {C5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {D5, t}, {D5, t},
    {C5, t}, {C5, t},

    // measure 3
    {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t},
    {C5, t}, {C5, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},


    // measure 4
    {C5, t}, {C5, t}, {C5, t}, {C5, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {0, t}, {0, t}, {0, t}, {0, t},
  
    // measure 5
    {0, t}, {0, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {F5, t}, {F5, t},
    {A5, t}, {A5, t}, {A5, t}, {A5, t}, 
    {G5, t}, {G5, t},
    {F5, t}, {F5, t},

    // measure 6
    {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {C5, t}, {C5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {D5, t}, {D5, t}, 
    {C5, t}, {C5, t},

    // measure 7
    {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t},
    {C5, t}, {C5, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t}, 
    {E5, t}, {E5, t}, {E5, t}, {E5, t},

    // measure 8
    {C5, t}, {C5, t}, {C5, t}, {C5, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {0, t}, {0, t}, {0, t}, {0, t},
  
    // measure 9
    {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t},

    // measure 10
    {D5, t}, {D5, t}, {D5, t}, {D5, t},  {D5, t}, {D5, t}, {D5, t}, {D5, t}, 
    {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t},

    // measure 11
    {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t}, {A4, t}, {A4, t}, {A4, t}, {A4, t},

    // measure 12
    {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t}, {G4s, t},

    // measure 13
    {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t}, {C5, t},

    // measure 14
    {D5, t}, {D5, t}, {D5, t}, {D5, t},  {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t},

    // measure 15
    {C5, t}, {C5, t}, {C5, t}, {C5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {A5, t}, {A5, t}, {A5, t}, {A5, t},
    {A5, t}, {A5, t}, {A5, t}, {A5, t},

    // measure 16
    {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t}, {G5s, t},
    
    // measure 17
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {B4, t}, {B4, t},
    {C5, t}, {C5, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {C5, t}, {C5, t},
    {B4, t}, {B4, t},

    // measure 18
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {A4, t}, {A4, t},
    {C5, t}, {C5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {D5, t}, {D5, t},
    {C5, t}, {C5, t},

    // measure 19
    {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t}, {B4, t},
    {C5, t}, {C5, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},

    // measure 20
    {C5, t}, {C5, t}, {C5, t}, {C5, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {0, t}, {0, t}, {0, t}, {0, t},

    // measure 21
    {0, t}, {0, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {F5, t}, {F5, t},
    {A5, t}, {A5, t}, {A5, t}, {A5, t},
    {G5, t}, {G5, t},
    {F5, t}, {F5, t},

    // measure 22 
    {0, t}, {0, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {C5, t}, {C5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},
    {D5, t}, {D5, t},
    {C5, t}, {C5, t},

    // measure 23
    {0, t}, {0, t},
    {B4, t}, {B4, t}, {B4, t}, {B4, t},
    {C5, t}, {C5, t},
    {D5, t}, {D5, t}, {D5, t}, {D5, t},
    {E5, t}, {E5, t}, {E5, t}, {E5, t},

    // measure 24
    {0, t}, {0, t},
    {C5, t}, {C5, t}, {C5, t}, {C5, t},
    {A4, t}, {A4, t},
    {A4, t}, {A4, t}, {A4, t}, {A4, t},
    {0, t}, {0, t}, {0, t}, {0, t},
    
    {0,0}}; 
    
    //
    for (int i = 0; i < 500; i++){
      // sending beats to the FPGA, accounting for delay
      digitalWrite(BEATL, !beat_map[i+28][1]);
      digitalWrite(BEATR, !beat_map[i+28][2]);
      
      // resetting MCU to FPGA pins
      digitalWrite(HITL, 0);
      digitalWrite(HITR, 0);
      digitalWrite(MISSL, 0);
      digitalWrite(MISSR, 0);


      if(note_map[i][1] == 0){
        // if the duration is 0, the song is over
        TIM15->ARR &= ~(TIM_ARR_ARR);
        TIM2->ARR &= ~(TIM_ARR_ARR);
        printf("Your score: %d \n", userPoints);
        printf("Total points: %d \n", totalPoints);
        break; 
      } else if (note_map[i][0] == 0){
        // if frequency is 0, set ARR and CCR1 to 0
        TIM15->ARR &= ~(TIM_ARR_ARR);    
        TIM15->CCR1 &= ~(TIM_CCR1_CCR1); 
        delay_duration(note_map[i][1]);
      } else {
        delay_duration(note_map[i][1]);
        delay_frequency(note_map[i][0]);
      }
      
      // clear counter
      TIM15->CNT &= ~(TIM_CNT_CNT);
      TIM16->CNT &= ~(TIM_CNT_CNT);

      // reset the UIF bit
      TIM16->SR &= ~(TIM_SR_UIF);
   
      while((TIM16->SR & TIM_SR_UIF) == 0){ // while the note is still playing

        ADC1->CR |= ADC_CR_ADSTART; // start regular conversions   
        while((ADC1->ISR & ADC_ISR_EOS) == 0){} // wait for EOS flag 
        Left_hit = USER_LEFT();
        Right_hit = USER_RIGHT();
        ADC1->ISR &= ~(ADC_ISR_EOS); // reset EOS flag
        ADC1->CR &= ~(ADC_CR_ADSTART); // stop conversions
        
        
        
        if (Left_hit || Right_hit){ // read beatmap and check if the user hit on time
          for (int j = -1; j < 2; j++){ // users have a window of 3 sixteenth notes to hit the beat
            if (beat_map[abs(i+j)][0]){
              if(Left_hit == beat_map[abs(i+j)][1] && Right_hit == beat_map[abs(i+j)][2]){
                
                if(beat_map[abs(i+j)][1]){ // checking if left side had the beat
                  digitalWrite(HITL, 1);
                  printf("hitL! \n");
                }

                if(beat_map[abs(i+j)][2]){ // checking if right side had the beat
                  digitalWrite(HITR, 1);
                  printf("hitR! \n");
                }
                userPoints++;
                beat_map[abs(i+j)][0] = 0; // setting beat in beat_map to 0 to avoid double counting

              } else { // if there was a beat but it was played incorrectly
                if(beat_map[i-1][1]){
                  digitalWrite(MISSL, 1);
                }
                if(beat_map[i-1][2]){
                  digitalWrite(MISSR, 1);
                }
              }
            }
          }
        } else if (beat_map[i-1][0]) { // if there was a beat but no user input

          if(beat_map[i-1][1]){
            digitalWrite(MISSL, 1);
          }
          if(beat_map[i-1][2]){
            digitalWrite(MISSR, 1);
          }

        } else {
        }
                 
      }    
    }
    if(userPoints < totalPoints/2){ // checking if userPoints met the threshold to move onto another round
      digitalWrite(GAMEOVER, 1);
      printf("Game Over");
      break;
    }
    memcpy(beat_map, beat_map_copy, sizeof(beat_map_copy)); // copying original map onto changed map before start of next round
    Round++;
    Speed--;
  }

  if(Round == 3){
    digitalWrite(ROUNDNUM0, 1);
    digitalWrite(ROUNDNUM1, 1);
    printf("You win!");
  }
}
