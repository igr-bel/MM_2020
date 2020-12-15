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
#define DRAWERS_WIN_STATE               5
#define KEYS_WIN_STATE                  6
#define CHANGE_HINT                     7


#define WIN_STATE                       10
#define WIN_IDLE_STATE                  11
#define FINAL_STATE                     12

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
//-------------------------------------------- ENUMS / STRUCTS
enum
{
    OFF,
    ON,
    ENABLE,
    DISABLE
};


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
#endif //MAIN_H_INCLUDED