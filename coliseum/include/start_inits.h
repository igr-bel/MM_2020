#ifndef _START_INITS_H
#define _START_INITS_H

#include "main.h"

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//hardware
inline void sleep_state_init(void);
inline void start_variables_init(void);
inline void props_is_ready(void);


//------------------------------------------------------------------------------------------------------
//                                       BEHAVIOR
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
void sleep_state_init(void)
{
    ledsControl(NUM_OF_LEDS, OFF);
    sleep_state_done_flag = true;
    return;
}//sleep_state_init()
//------------------------------------------------------------------------------------------------------
void start_variables_init(void)
{
    //start values
    for(uint8_t i = 0; i < NUM_OF_BUTTONS; i++)
    {
        win_sequence[i] = i;
    }
    shuffleArr(win_sequence, NUM_OF_BUTTONS);
    shuffleArr(win_sequence, NUM_OF_BUTTONS);

	show_sequence_timer = millis();

    //state machine
    current_state = IDLE_STATE;

    //flags
    sleep_state_done_flag = false;
    game_start_flag = false;

	//state
	GAME_NOT_SOLVED();

    
    return;
}//start_variables_init()
//------------------------------------------------------------------------------------------------------
void props_is_ready(void)
{


    return;
}//props_is_ready()
//------------------------------------------------------------------------------------------------------

#endif //_START_INITS_H