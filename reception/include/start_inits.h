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
    sleep_state_done_flag = true;

	//loads
	doorLockControl(OPEN);
	drawerLockControl(OPEN);

    return;
}//sleep_state_init()
//------------------------------------------------------------------------------------------------------
void start_variables_init(void)
{
    //start values
    hint_stage = FIRE_HINT;
    hint_repeat = false;
    hint_lang = FR;

    prew_drawers_state = 0xFF;
    prew_keys_state = 0xFF;

    //state machine
    current_state = IDLE_STATE;

    //flags
    fire_hidrant_win_flag = false;
    diorama_win_flag = false;
    drawers_win_flag = false;
    keys_win_flag = false;
    coliseum_win_flag = false;
    napoleon_win_flag = false;
    ivan_win_flag = false;
    china_win_flag = false;
    circle_1_win_flag = false;
    circle_2_win_flag = false;

	sleep_state_done_flag = false;
	game_start_flag = false;

	//loads
	doorLockControl(CLOSED);
	drawerLockControl(CLOSED);
    
    return;
}//start_variables_init()
//------------------------------------------------------------------------------------------------------
void props_is_ready(void)
{
    new_data_recieved = false;

    return;
}//props_is_ready()
//------------------------------------------------------------------------------------------------------

#endif //_START_INITS_H