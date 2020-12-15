#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include "functions.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
//STATE MACHINE
#define IDLE_STATE                      0
#define SLEEP_STATE                     1
#define RESET_STATE                     2
#define RESTART_STATE                   3

#define GAME_STATE                      4
#define IVAN_WIN_STATE                  5
#define FIRE_WIN_STATE                  6
#define CHINA_WIN_STATE                 7
#define NAPOLEON_WIN_STATE              8
#define COLISEUM_WIN_STATE              9
#define CIRCLE_1_WIN_STATE              10
#define CIRCLE_2_WIN_STATE              11


#define WIN_STATE                       20
#define WIN_IDLE_STATE                  21
#define FINAL_STATE                     22


//LEDS / ANIMATION / TIMERS / DELAYS

//OFFSETS

//SHIFT MASKS

//HINTS

//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------

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

uint16_t sensors_state;

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
#endif //MAIN_H_INCLUDED