#ifndef _MAIN_H
#define _MAIN_H

//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include <Arduino.h>

#include "functions.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
//STATE MACHINE
#define IDLE_STATE                      0
#define SLEEP_STATE                     1
#define RESET_STATE                     2
#define RESTART_STATE                   3
#define WAIT_HINT_RELEASE               4
#define WIN_STATE                       5
#define WIN_IDLE_STATE                  6
#define GAME_1_STATE                    7
#define GAME_2_STATE                    8
#define GAME_1_WIN                      9
#define GAME_2_WIN                      10
#define SOLVED_BY_OPERATOR              11

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
uint8_t pressed_button;

//------------------------------------ INPUT ETHERNET DATA
byte msg_payload[10];
uint8_t msg_length;

//--------------------------------------- DATA PARSER
uint16_t command;
String cmd_string;
char *pos = 0;
int base = 16;

//-------------------------------------- STATE MACHINE
volatile uint8_t current_state;

//------------------------------------------------------------------------------------------------------
//                                        DEBUG
//------------------------------------------------------------------------------------------------------
#ifdef DBG
  unsigned long DBG_out_interval = 100;
  unsigned long DBG_time_delay = 0;
  #define printDBG(str)     Serial.print("[DBG] "); Serial.println(str) 
#else 
  #define printDBG(str)     ;
#endif

//------------------------------------------------------------------------------------------------------
//                                        FLAGS
//------------------------------------------------------------------------------------------------------
bool game_start_flag = false;
bool new_data_recieved = false;
bool sleep_state_done_flag = false;

//------------------------------------------------------------------------------------------------------
#endif //_MAIN_H