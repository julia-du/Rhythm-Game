/*
    Main Header: Contains general defines and selected portions of CMSIS files
    @file main.h
    @author Allison Tsai
    @version 1.0 12/7/2023
*/

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"
#include "beatmap.h"
#define BUFF_LEN 32

// connections to FPGA
#define HITL PA0
#define MISSL PA11
#define HITR PA6
#define MISSR PB5
#define ROUNDNUM0 PB3
#define ROUNDNUM1 PB4
#define GAMEOVER PA8
#define BEATL PA10
#define BEATR PA9


#endif // MAIN_H