#ifndef _MQTT_PARSER_H
#define _MQTT_PARSER_H


//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------
#include "main.h"
#include <Ethernet.h>
#include <PubSubClient.h>
#include "hints.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
//ETHERNET      
#define CLIENT_ID                       "client_2"
#define ETHERNET_SS_PIN                 53

//out cmd       
#define RESET_WEB_UI                    0x200
#define PHONE_ACTIVE                    0x206
#define PHONE_NOT_ACTIVE                0x207
#define DRAWERS_WIN                     0x40F
#define DRAWERS_STATE_MASK              0x400
#define KEYS_WIN                        0x50F
#define KEYS_STATE_MASK                 0x500

#define C_PING_RESP                     0x30D

//in cmd        
#define SLEEP_CMD                       0x309
#define RESTART_CMD                     0x302
#define RESET_CMD                       0x301
#define START_CMD                       0x306

#define SOLVE_DRAWERS                   0x315
#define SOLVE_KEYS                      0x325

#define OPEN_DOOR_LOCK                  0x323
#define CLOSE_DOOR_LOCK                 0x324
#define OPEN_DRAWER_LOCK                0x313
#define CLOSE_DRAWER_LOCK               0x314

#define TAKE_PHONE_HINT                 0x331

#define S_PING_REQ                      0x30A

#define EN_SOUNDS                       0x1011
#define FR_SOUNDS                       0x1012

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
uint8_t mac[6] = {0xFF,0x08,0x12,0xCA,0xFD,0x04};
const char* server = "192.168.0.60";
IPAddress ip(192, 168, 0, 202);
IPAddress myDns(192, 168, 0, 1);

const String sub_topic = "/client_2_sub";
const String pub_topic = "/client_2_pub";
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

        case S_PING_REQ:
            publishData(C_PING_RESP);
            break;

        //game
        case SOLVE_DRAWERS:
            current_state = DRAWERS_WIN_STATE;
            break;

        case SOLVE_KEYS:
            current_state = KEYS_WIN_STATE;
            break;

        case OPEN_DOOR_LOCK:
            doorLockControl(OPEN);
            break;

        case CLOSE_DOOR_LOCK:
            doorLockControl(CLOSED);
            break;

        case OPEN_DRAWER_LOCK:
            drawerLockControl(OPEN);
            break;

        case CLOSE_DRAWER_LOCK:
            drawerLockControl(CLOSED);
            break;

        case TAKE_PHONE_HINT:
            takeHint();
            break;

        //hints
        case FIRE_HINT:
            fire_hidrant_win_flag = true;
            break;
        
        case DIORAMA_HINT:
            diorama_win_flag = true;
            break;
                    
        case DRAWERS_HINT:
            drawers_win_flag = true;
            break;
                    
        case KEYS_HINT:
            keys_win_flag = true;
            break;
                    
        case COLISEUM_HINT:
            coliseum_win_flag = true;
            break;
                    
        case NAPOLEON_HINT:
            napoleon_win_flag = true;
            break;
                    
        case IVAN_HINT:
            ivan_win_flag = true;
            break;
                    
        case CHINA_HINT:
            china_win_flag = true;
            break;
                    
        case CIRC_1_HINT:
            circle_1_win_flag = true;
            break;
                    
        case CIRC_2_HINT:
            circle_2_win_flag = true;
            break;

        //sounds
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
    //int to string //*char 
    uint8_t indx = 0;
    msgBuffer[indx++] = data / 10000 + 48;
    data -= (data / 10000) * 10000;
    msgBuffer[indx++] = data / 1000 + 48;
    data -= (data / 1000) * 1000;
    msgBuffer[indx++] = data / 100 + 48;
    data -= (data / 100) * 100;
    msgBuffer[indx++] = data / 10 + 48;
    data -= (data / 10) * 10;
    msgBuffer[indx++] = (data % 10) + 48;
    msgBuffer[indx] = '\0';
    //drop first zero
    indx = 0;
    while(msgBuffer[0] == 48)
    {
        for(uint8_t i = 0; i < 5; i++)
        {
            msgBuffer[i] = msgBuffer[i+1];
        }
    }

    if(mqttClient.connect(CLIENT_ID)) 
    {
        mqttClient.publish(pub_topic.c_str(), msgBuffer);
    }

    return;
}//publishData()
//------------------------------------------------------------------------------------------------------
void setup_ethernet(void)
{
    //ss pin
    Ethernet.init(ETHERNET_SS_PIN);
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