#ifndef _MQTT_CMDS
#define _MQTT_CMDS

//------------------------------------------------------------------------------------------------------
//                                        OUT COMMANDS
//------------------------------------------------------------------------------------------------------
//stuff     
#define RESET_WEB_UI                    0x200

#define C_PING_RESP                     0x30D

//game
#define IVAN_CLEAR                      0x600
#define IVAN_WIN                        0x603
#define FIRE_BUTTON_CLEAR               0x400
#define FIRE_WIN                        0x401
#define CHINA_CLEAR                     0x410
#define CHINA_WIN                       0x41F
#define NAPOLEON_CLEAR                  0x480
#define NAPOLEON_WIN                    0x4FF
#define COLISEUM_CLEAR                  0x420
#define COLISEUM_WIN                    0x421
#define CIRCLE_CLEAR                    0x440
#define CIRCLE_1_WIN                    0x44F
#define CIRCLE_2_WIN                    0x457
#define CIRCLES_WIN                     0x458

// #define ALL_WIN                         0x0000

//------------------------------------------------------------------------------------------------------
//                                        IN COMMANDS
//------------------------------------------------------------------------------------------------------
//stuff       
#define SLEEP_CMD                       0x309
#define RESTART_CMD                     0x302
#define RESET_CMD                       0x301
#define START_CMD                       0x306

#define S_PING_REQ                      0x30A

#define EN_SOUNDS                       0x1011
#define FR_SOUNDS                       0x1012

#define FIRST_DOOR_LOCK                 0x501
#define FIRST_DOOR_UNLOCK               0x502
#define SECOND_DOOR_LOCK                0x503
#define SECOND_DOOR_UNLOCK              0x504
#define CHINA_LOCK                      0x505
#define CHINA_UNLOCK                    0x506
#define NAPOLEON_LOCK                   0x507
#define NAPOLEON_UNLOCK                 0x508
#define IVAN_LOCK                       0x509
#define IVAN_UNLOCK                     0x50A
#define CESARE_LOCK                     0x50B
#define CESARE_UNLOCK                   0x50C
#define CIRCLES_LOCK                    0x50D
#define CIRCLES_UNLOCK                  0x50E
#define COLISEUM_LOCK                   0x701
#define COLISEUM_UNLOCK                 0x702
#define FIRST_ROOM_LIGHT_ON             0x703
#define FIRST_ROOM_LIGHT_OFF            0x704
#define SECOND_ROOM_LIGHT_ON            0x705
#define SECOND_ROOM_LIGHT_OFF           0x706
#define FIRE_LED_ON                     0x707
#define FIRE_LED_OFF                    0x708
#define FIRE_LIGHT_ON                   0x709
#define FIRE_LIGHT_OFF                  0x70A
#define COLISEUM_SET_ENABLE             0x70B
#define COLISEUM_SET_DISABLE            0x70C

//game
#define SOLVE_FIRE                      0x901
#define SOLVE_CHINA                     0x908
#define SOLVE_NAPOLEON                  0x906
#define SOLVE_IVAN                      0x907
#define SOLVE_COLISEUM                  0x905
#define SOLVE_CIRCLE1                   0x909
#define SOLVE_CIRCLE2                   0x90A

//------------------------------------------------------------------------------------------------------
#endif  //_MQTT_CMDS