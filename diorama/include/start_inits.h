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
    spotLightControl(NUM_OF_SPOTS, OFF);
    mainLightControl(OFF);
    sleep_state_done_flag = true;
    return;
}//sleep_state_init()
//------------------------------------------------------------------------------------------------------
void start_variables_init(void)
{
    //start values
    //set to zero all spots and set the pointers to game spots
    uint8_t j = 0;
    for(uint8_t i = 0; i < NUM_OF_SPOTS; i++)
    {
        spots_state[i] = 0;
        if(i != SPOT_1 && i != SPOT_7)
        {
            game_spots_state[j++] = &spots_state[i];
        }    
    }

    //state machine
    current_state = IDLE_STATE;

    //flags
    sleep_state_done_flag = false;
    game_start_flag = false;

    new_data_recieved = false;

    //first buttons -> spots links
    for(uint8_t i = 0; i < NUM_OF_BUTTONS; i++)
    {
        button_spot_link[i] = i;
    }
    for(uint8_t i = 0; i < 10; i++)
    {
        shuffleArr(button_spot_link, NUM_OF_BUTTONS);
    }
    
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