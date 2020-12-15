#include "main.h"
#include "start_inits.h"
#include "functions.h"

//======================================================================================================
//------------------------------------------------------------------------------------------------------
//                                        SETUP
//------------------------------------------------------------------------------------------------------
void setup(void)
{
	//------------------------------------------------------ GAME INITS
	hardware_init();
	//------------------------------------------------------ GAME INITS
	start_variables_init();
	//------------------------------------------------------ ETHERNET

	//------------------------------------------------------

}//setup()
//------------------------------------------------------------------------------------------------------
//                                        LOOP
//------------------------------------------------------------------------------------------------------
void loop(void)
{
	if(IS_GAME_ENABLE())
	{
		outputEnable();
		show_sequence_timer = millis() + PAUSE_SEQUENCE_DELAY;
	}
	else
	{
		outputDisable();
	}
	
	//------------------------------------------- ALGORITHM >>>
	//------------------------------------------------------------------------------------
	//STATE MACHINE
	switch(current_state)
	{
		case SLEEP_STATE:
			if(!sleep_state_done_flag)
			{
				sleep_state_init();
			}
			break;

		case RESET_STATE:
			resetFunc();
			break;

		case RESTART_STATE:
			start_variables_init();
			break;

		case IDLE_STATE:
			//show win sequence
			if((millis() - show_sequence_timer) > PAUSE_SEQUENCE_DELAY)
			{
				showWinSequence();
				show_sequence_timer = millis();
			}

			//if start game
			if(game_start_flag == true)
			{
				ledsControl(NUM_OF_LEDS, OFF);
				leds_state[pressed_button] = ON;
				game_counter = 0;
				current_state = GAME_1_STATE;
				break;
			}

			pressed_button = getBtnState();
			if(pressed_button != NOT_PRESSED)
			{
				game_start_flag = true;
			}
			break;

		case GAME_1_STATE:
			if(pressed_button != NOT_PRESSED)
			{
				if(checkWin() == ERROR)
				{
					for(uint8_t i = 0; i < 2; i++)
					{
						ledsControl(NUM_OF_LEDS, ON);
						delay(100);
						ledsControl(NUM_OF_LEDS, OFF);
						delay(100);
					}
					shuffleArr(win_sequence, NUM_OF_BUTTONS);
					current_state = IDLE_STATE;
					game_start_flag = false;
					delay(2000);
					break;
				}
				else
				{
					game_counter++;
					ledsControl(pressed_button, ON);
				}

				//check win
				if(game_counter == 6)
				{
					current_state = GAME_1_WIN;
					break;
				}
			}

			pressed_button = getBtnState();

			//check game enable
			if(!IS_GAME_ENABLE())
			{
				current_state = IDLE_STATE;
				game_start_flag = false;
				outputDisable();
				GAME_NOT_SOLVED();
				delay(1000);
			}
			break;

		case GAME_1_WIN:
			ledsControl(NUM_OF_LEDS, ON);
			delay(1000);
			ledsControl(NUM_OF_LEDS, OFF);
			for(uint8_t i = 0; i < NUM_OF_LEDS; i++)
			{
				ledsControl(i, ON);
				delay(150);		
			}
			delay(1000);
			ledsControl(NUM_OF_LEDS, OFF);
			GAME_SOLVED();
			current_state = WIN_IDLE_STATE;
			break;


		case WIN_IDLE_STATE:
			if(!IS_GAME_ENABLE())
			{
				current_state = IDLE_STATE;
				game_start_flag = false;
				outputDisable();
				GAME_NOT_SOLVED();
				delay(1000);
			}
			delay(100);
			break;

		case SOLVED_BY_OPERATOR:

			current_state = WIN_IDLE_STATE;
			break;

		default:
			break;
	}//switch STATE MACHINE

  //------------------------------------------- <<< ALGORITHM


  //---DBG info
	#ifdef DBG
		if(millis() - DBG_time_delay >= DBG_out_interval)
			{
			DBG_time_delay = millis();

			// printDBG("");
			}
	#endif
  
}//loop()