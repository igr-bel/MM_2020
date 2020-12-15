#include "main.h"
#include "mqtt_parser.h"
#include "start_inits.h"
#include "functions.h"
unsigned long timer = 0;
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
    //------------------------------------------------------ phone keypad
    keypadInit();
    //------------------------------------------------------ sounds
    mp3_init();

}//setup()
//------------------------------------------------------------------------------------------------------
//                                        LOOP
//------------------------------------------------------------------------------------------------------
void loop(void)
{
    //REGULAR TASKS
    phoneGame();
    if(!phone_err)
    {
        takeHint();
        publishData(PHONE_ACTIVE);
    }
    if(!hint_played && !phone_send_ui_symbol)
    {
        publishData(PHONE_NOT_ACTIVE);
        phone_send_ui_symbol = true;
    }
    
	changeHint();

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
                current_state = GAME_STATE;
            }//if start game
            break;

        case GAME_STATE:
            getDrawersState();
            if((current_drawers_state != prew_drawers_state) && ! drawers_win_flag)
            {
                if (current_drawers_state == DRAWERS_WIN_OFFSET)
                {
                    current_state = DRAWERS_WIN_STATE;
                }

                publishData(DRAWERS_STATE_MASK | current_drawers_state);
            }

            getKeysState();
            if((current_keys_state != prew_keys_state) && !keys_win_flag)
            {
                if (current_keys_state == KEYS_WIN_OFFSET)
                {
                    current_state = KEYS_WIN_STATE;
                }

                publishData(KEYS_STATE_MASK | current_keys_state);
            }

            if(drawers_win_flag && keys_win_flag)
            {
                current_state = WIN_IDLE_STATE;
            }
            break;

        case DRAWERS_WIN_STATE:
            publishData(DRAWERS_WIN);
            drawers_win_flag = true;
            current_state = GAME_STATE;
            break;

        case KEYS_WIN_STATE:
            publishData(KEYS_WIN);
            keys_win_flag = true;
            current_state = GAME_STATE;
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