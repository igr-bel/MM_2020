#include "main.h"
#include "mqtt_parser.h"
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
    setup_ethernet();
    //------------------------------------------------------ setup mqtt client
    setup_mqtt();  

}//setup()
//------------------------------------------------------------------------------------------------------
//                                        LOOP
//------------------------------------------------------------------------------------------------------
void loop(void)
{
    //REGULAR TASKS
    mqttClient.loop();

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
            //reset ui
            publishData(RESET_WEB_UI);
            break;

        case IDLE_STATE:
            if(game_start_flag == true)
            {
				fireLightControl(ON);
				lightSecondRoom(OFF);
				lockInControl(CLOSED);
                current_state = GAME_STATE;
            }//if start game
            break;

        case GAME_STATE:
            if(!fire_win_flag)
            {
                //fire button pulse
                fireLedControl(PULSE);
                
                sensors_state = getFireButtonState();
                if(sensors_state != FIRE_BUTTON_CLEAR)
                {
                    current_state = FIRE_WIN_STATE;
                }
            }//fire hidrant game

            if(!china_win_flag)
            {
                sensors_state = getChinaState();
                if(current_china_state != prew_china_state)
                {
                    if(sensors_state == CHINA_WIN)
                    {
                        current_state = CHINA_WIN_STATE;
                    }
                    else
                    {
                        publishData(sensors_state);
                    }
                }
            }//China game

            if(!napoleon_win_flag)
            {
                sensors_state = getNapoleonState();
                if(current_napoleon_state != prew_napoleon_state)
                {
                    if(sensors_state == NAPOLEON_WIN)
                    {
                        current_state = NAPOLEON_WIN_STATE;
                    }
                    else
                    {
                        publishData(sensors_state);
                    }
                }
            }//Napoleon game

            if(!coliseum_win_flag)
            {
                sensors_state = getColiseumState();
                if(sensors_state == COLISEUM_WIN)
                {
                    current_state = COLISEUM_WIN_STATE;
                }
            }//Coliseum game

            if(!ivan_win_flag)
            {
                sensors_state = getIvanState();
                if(current_ivan_state != prew_ivan_state)
                {
                    if(sensors_state == IVAN_WIN)
                    {
                        current_state = IVAN_WIN_STATE;
                    }
                    else
                    {
                        publishData(sensors_state);
                    }
                }
            }//Ivan game
 
            if(!circle_1_win_flag)
            {
                sensors_state = getCircleState();
                if(circles_combination != prew_circles_combination || 
					circles_position != prew_circles_position)
                {    
                    if(sensors_state == CIRCLE_1_WIN)
                    {
                        current_state = CIRCLE_1_WIN_STATE;
                    }
                    else
                    {
                        publishData(CIRCLE_CLEAR | (circles_combination << 3) | circles_position);
                    }   
                }
            }//circle 1 game
             
            if(!circle_2_win_flag)
            {
                sensors_state = getCircleState();
                if(circles_combination != prew_circles_combination || 
					circles_position != prew_circles_position)
                {    
                    if(sensors_state == CIRCLE_2_WIN)
                    {
                        current_state = CIRCLE_2_WIN_STATE;
                    }
                    else if(sensors_state != CIRCLE_1_WIN) //for drop one more SIRCLE_1_WIN parcel
                    {
                        publishData(CIRCLE_CLEAR | (circles_combination << 3) | circles_position);
                    }   
                }
            }//circle 2 game

            if( fire_win_flag &&
                coliseum_win_flag &&
                napoleon_win_flag &&
                ivan_win_flag &&
                china_win_flag &&
                circle_1_win_flag &&
                circle_2_win_flag)
            {
                current_state = WIN_STATE;
            }
            break;

        case FIRE_WIN_STATE:
            fire_win_flag = true;
            publishData(FIRE_WIN);
            fireLedControl(OFF);
            current_state = GAME_STATE;
            break;

        case CHINA_WIN_STATE:
            china_win_flag = true;
            publishData(CHINA_WIN);
            current_state = GAME_STATE;
            break;

        case NAPOLEON_WIN_STATE:
            napoleon_win_flag = true;
            publishData(NAPOLEON_WIN);
            current_state = GAME_STATE;
            break;

        case IVAN_WIN_STATE:
            ivan_win_flag = true;
            publishData(IVAN_WIN);
            current_state = GAME_STATE;
            break;

        case COLISEUM_WIN_STATE:
            coliseum_win_flag = true;
            publishData(COLISEUM_WIN);
            current_state = GAME_STATE;
            break;

        case CIRCLE_1_WIN_STATE:
            circle_1_win_flag = true;
            publishData(CIRCLE_1_WIN);
            current_state = GAME_STATE;
            break;

        case CIRCLE_2_WIN_STATE:
            circle_2_win_flag = true;
            publishData(CIRCLE_2_WIN);
			delay(10);
			publishData(CIRCLES_WIN);
            current_state = GAME_STATE;
            break;

        case WIN_STATE:
            current_state = WIN_IDLE_STATE;
            break;

        case WIN_IDLE_STATE:
            //wait commands from node-red
            NOP;
            delay(1);
            break;

        default:
            break;
    }//switch STATE MACHINE

  //------------------------------------------- <<< ALGORITHM

  //------------------------------------------- COMMUNICATION >>>
    //parse new data
    if(new_data_recieved)
    {
        new_data_recieved = false;

        //string to command code
        command = 0;
        uint8_t shft = (msg_length == 5) ? 8 : 12;
        for(uint8_t indx = 2; indx < msg_length; indx++)
        {
            command |= (msg_payload[indx] > 64) ? (msg_payload[indx] - 55) << shft : (msg_payload[indx] - 48) << shft;
            shft -= 4;
        }//for

        mqttParser(command);

        mqttClient.loop();

    }//if new data recieved  
  //------------------------------------------- <<< COMMUNICATION

  //------------------------------------------- RECONNECT >>>
  if(mqttClient.state() != MQTT_CONNECTED)
    {
        Serial.println("[ERR] Reconnect ... ");
        while(!mqttClient.connect(CLIENT_ID))
        {
            setup_ethernet();
            delay(1000);
        }
        setup_mqtt();  
    }//if connection is lost
  //------------------------------------------- <<< RECONNECT

  //------------------------------------------------------------------------------------

    //---DBG info
    #ifdef DBG
    if(millis() - DBG_time_delay >= DBG_out_interval)
    {
        DBG_time_delay = millis();
        
        // printDBG("");
    }
    #endif
  
}//loop()