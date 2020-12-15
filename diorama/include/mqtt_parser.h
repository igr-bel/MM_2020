#ifndef _MQTT_PARSER_H
#define _MQTT_PARSER_H


//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include "main.h"
#include <Ethernet.h>
#include <PubSubClient.h>

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
//ETHERNET      
#define CLIENT_ID                       "client_1"

//out cmd       
#define RESET_WEB_UI                    0x200
#define PART_1_SOLVED                   0x201
#define PART_2_SOLVED                   0x202
#define PART_1_STARTED                  0x203
#define PART_2_STARTED                  0x204
#define ALL_SOLVED                      0x205

#define C_PING_RESP                     0x30D

//in cmd        
#define SLEEP_CMD                       0x309
#define RESTART_CMD                     0x302
#define RESET_CMD                       0x301
#define START_CMD                       0x306
#define SOLVE_PROP                      0x310
#define START_GAME_1                    0x311
#define START_GAME_2                    0x312
#define SWITCH_ON_SPOTS                 0x313
#define SWITCH_OFF_SPOTS                0x314
#define OPEN_LOCK                       0x315
#define SWITCH_ON_LIGHT                 0x316
#define SWITCH_OFF_LIGHT                0x317

#define S_PING_REQ                      0x30A

#define EN_SOUNDS                       0x1011
#define FR_SOUNDS                       0x1012

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
uint8_t mac[6] = {0x12,0x0D,0xB2,0xA4,0x36,0x1C};
const char* server = "192.168.0.60";
IPAddress ip(192, 168, 0, 201);
IPAddress myDns(192, 168, 0, 1);

const String sub_topic = "/client_1_sub";
const String pub_topic = "/client_1_pub";
const String brdcst_topic = "/broadcast";

//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------
EthernetClient ethClient;
PubSubClient mqttClient;

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//ethernet
void subscribeReceive(char* topic, byte* payload, unsigned int length);
void publishData(int data);
void mqttParser(uint16_t command);
inline void setup_ethernet(void);
inline void setup_mqtt(void);

//------------------------------------------------------------------------------------------------------
void mqttParser(uint16_t command)
{
    switch(command)
    {
        case SLEEP_CMD:
            current_state = SLEEP_STATE;
            break;

        case RESTART_CMD:     //restart prop
            current_state = RESTART_STATE;
            break;

        case RESET_CMD:       //reset prop
            current_state = RESET_STATE;
            break;

        case START_CMD:       //start prop
            game_start_flag = true;
            current_state = IDLE_STATE;
            break;

        case START_GAME_1:
            current_state = GAME_1_STATE;
            Serial.println(current_state);
            break;
            
        case START_GAME_2:
            publishData(PART_2_STARTED);
            //change game side
            if(current_game_side == SIDE_2)
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
            current_state = GAME_2_STATE;
            break;

        case SOLVE_PROP:
            current_state = SOLVED_BY_OPERATOR;
            break;

        case SWITCH_ON_SPOTS:
            spotLightControl(NUM_OF_SPOTS, ON);
            break;

        case SWITCH_OFF_SPOTS:
            spotLightControl(NUM_OF_SPOTS, OFF);
            break;

        case OPEN_LOCK:
            magLatchOpen();
            break;

        case SWITCH_ON_LIGHT:
            mainLightControl(ON);
            break;

        case SWITCH_OFF_LIGHT:
            mainLightControl(OFF);
            break;

        case S_PING_REQ:
            publishData(C_PING_RESP);
            break;

        case EN_SOUNDS:
            hint_lang = EN;
            break;

        case FR_SOUNDS:
            hint_lang = FR;
            break;

        default:
            break;
    }//switch

}//mqttParser()
//------------------------------------------------------------------------------------------------------
void subscribeReceive(char* topic, byte* payload, unsigned int length)
{
  //set new data ready flag
  new_data_recieved = true;

  //save new data
  for(uint8_t i=0; i<length; i++)
  {
    msg_payload[i] = payload[i];
  }//for
  msg_length = length;

}//subscribeReceive()
//------------------------------------------------------------------------------------------------------
void publishData(int data)
{
    char msgBuffer[6];  //maximum count of simbols for convert to string ( 0xFFFF = 65535 )
    
    uint8_t indx = 0;
    msgBuffer[indx] = data / 10000 + 48;
    indx = (msgBuffer[indx] != 48) ? indx + 1 : indx;
    data -= (data / 10000) * 10000;
    msgBuffer[indx] = data / 1000 + 48;
    indx = (msgBuffer[indx] != 48) ? indx + 1 : indx;
    data -= (data / 1000) * 1000;
    msgBuffer[indx] = data / 100 + 48;
    indx = (msgBuffer[indx] != 48) ? indx + 1 : indx;
    data -= (data / 100) * 100;
    msgBuffer[indx] = data / 10 + 48;
    indx = (msgBuffer[indx] != 48) ? indx + 1 : indx;
    data -= (data / 10) * 10;
    msgBuffer[indx] = (data % 10) + 48;
    indx = (msgBuffer[indx] != 48) ? indx + 1 : indx;
    msgBuffer[indx] = '\0';

    if(mqttClient.connect(CLIENT_ID)) 
    {
        mqttClient.publish(pub_topic.c_str(), msgBuffer);
    }

    return;
}//publishData()
//------------------------------------------------------------------------------------------------------
void setup_ethernet(void)
{
    //
    Ethernet.init(10);
    Ethernet.begin(mac, ip, myDns);
    delay(1500);

    // check for Ethernet hardware present
    if(Ethernet.hardwareStatus() == EthernetNoHardware) 
    {
        Serial.println(" ");
        Serial.println("[INF] Ethernet shield was not found.");
        
        while(true) 
        {
            delay(1); // empty loop
        }//while(true)
    }//if
    while(Ethernet.linkStatus() == LinkOFF) 
    {
        Serial.println("[INF] Ethernet cable is not connected.");
        delay(500);
    }//while
    // give the Ethernet shield a second to initialize:
    delay(1000);

}//setup_ethernet()
//------------------------------------------------------------------------------------------------------
void setup_mqtt(void)
{
    mqttClient.setClient(ethClient);
    mqttClient.setServer(server, 1883);

    Serial.println(" ");
    Serial.println("[INF] connecting...");
    while(!mqttClient.connect(CLIENT_ID))
    {
        Serial.println("[INF] Looks like the server connection failed...");
        Serial.println(mqttClient.state());
        delay(1000);
    }
    Serial.println("[INF] Connection has been established, well done!");

    mqttClient.setCallback(subscribeReceive);

    mqttClient.subscribe(sub_topic.c_str());
    mqttClient.subscribe(brdcst_topic.c_str());
}//setup_mqtt()
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------
#endif //_MQTT_PARSER_H