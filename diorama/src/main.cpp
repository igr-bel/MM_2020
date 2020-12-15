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
  //------------------------------------------------------

}//setup()
//------------------------------------------------------------------------------------------------------
//                                        LOOP
//------------------------------------------------------------------------------------------------------
void loop(void)
{
  //------------------------------------------- ALGORITHM >>>
  //------------------------------------------------------------------------------------

// printDBG(current_state);

// delay(1000);
//===
// game_start_flag = true;
//===

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
      publishData(PART_1_STARTED);
      current_state = GAME_1_STATE;
      mainLightControl(ON);
      outputEnable();
      
      if((random() % 2) == 0)
      {
        current_game_side = SIDE_1;
        spotLightControl(SPOT_1, ON);
        spotLightControl(SPOT_7, OFF);
      }
      else
      {
        current_game_side = SIDE_2;  
        spotLightControl(SPOT_7, ON);
        spotLightControl(SPOT_1, OFF);
      }//if_else
    }//if start game
    break;

  case GAME_1_STATE:
    pressed_button = getBtnState();
    if(pressed_button != NOT_PRESSED)
    {
      *game_spots_state[button_spot_link[pressed_button]] = !*game_spots_state[button_spot_link[pressed_button]];
    }

    if(isWin(current_game_side))
    {
      Serial.println("WIN!!!");
      current_state = GAME_1_WIN;
    }
    break;

  case GAME_2_STATE: 
    pressed_button = getBtnState();
    if(pressed_button != NOT_PRESSED)
    {
      *game_spots_state[button_spot_link[pressed_button]] = !*game_spots_state[button_spot_link[pressed_button]];
    }

    if(isWin(current_game_side))
    {
      current_state = GAME_2_WIN;
      refreshSpots();
      Serial.println("WIN!!!");
      delay(1000); 
      spotLightControl(NUM_OF_SPOTS, OFF);
    }
    break;

  case GAME_1_WIN:
    publishData(PART_1_SOLVED);

    delay(1000); 
    spotLightControl(NUM_OF_SPOTS, OFF);

    // shuffle buttons
    shuffleArr(button_spot_link, NUM_OF_BUTTONS);

    current_state = WIN_IDLE_STATE;
    break;

  case GAME_2_WIN:
    publishData(PART_2_SOLVED);
    delay(100);
    publishData(ALL_SOLVED);

    delay(1000); 
    spotLightControl(NUM_OF_SPOTS, OFF);

    current_state = WIN_IDLE_STATE;
    break;

  case WIN_IDLE_STATE:
    //wait commands from node-red
    delay(1);
    break;

  case SOLVED_BY_OPERATOR:
    publishData(ALL_SOLVED);
    current_state = WIN_IDLE_STATE;
    break;

  default:
    break;
  }//switch STATE MACHINE

  //REGULAR TASKS
  mqttClient.loop();
  refreshSpots();

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