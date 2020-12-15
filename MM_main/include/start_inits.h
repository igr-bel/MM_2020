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

	fireLedControl(OFF);
	fireLightControl(OFF);
	lightFirstRoom(OFF);
	lightSecondRoom(OFF);

	lockInControl(OPEN);
	lockSecondRoomControl(OPEN);
	lockChinaControl(OPEN);
	lockNapoleonControl(OPEN);
	lockIvanControl(OPEN);
	lockCesareControl(OPEN);
	lockCircleControl(OPEN);

    return;
}//sleep_state_init()
//------------------------------------------------------------------------------------------------------
void start_variables_init(void)
{
    //start values
    prew_ivan_state = 99;
    current_ivan_state = IVAN_CLEAR;
    prew_china_state = 99;
    current_china_state = CHINA_CLEAR;
    prew_napoleon_state = 99;
    current_napoleon_state = NAPOLEON_CLEAR;
    prew_circles_combination = 99;
    prew_circles_position = CIRCLE_CLEAR;
    circles_combination = 99;
    circles_position = CIRCLE_CLEAR;

    //state machine
    current_state = IDLE_STATE;

    //flags
    game_start_flag = false;
	sleep_state_done_flag = false;

    fire_win_flag = false;
    coliseum_win_flag = false;
    napoleon_win_flag = false;
    ivan_win_flag = false;
    china_win_flag = false;
    circle_1_win_flag = false;
    circle_2_win_flag = false;

    //loads
    gameColiseumControl(DISABLE);

	fireLedControl(OFF);
	fireLightControl(OFF);

	lightFirstRoom(ON);
	lightSecondRoom(ON);

	lockInControl(OPEN);
	lockSecondRoomControl(CLOSED);
	lockChinaControl(CLOSED);
	lockNapoleonControl(CLOSED);
	lockIvanControl(CLOSED);
	lockCesareControl(CLOSED);
	lockCircleControl(CLOSED);
    
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