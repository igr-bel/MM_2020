#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#include <Arduino.h>

#include "hints.h"
#include "RFID.h"
#include "mqtt_cmds.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
#define DBG                             
#define NOP                             do{}while(0)

//FIRE BTN PULSE
#define BTN_PULSE_DELAY                 2
#define PWM_STEP                        5 

//GPIO
#define LIGHT_1_ROOM_PIN                28
#define LIGHT_2_ROOM_PIN                29                
#define FIRE_LED_PIN                    13
#define FIRE_LIGHT_PIN                  12                        

//RFID
#define RFID_1_PIN                      47
#define RFID_2_PIN                      48

//IN SHIFT_REGISTER     
#define latch_pin_in                    5
#define data_pin_in                     7
#define clock_pin_in                    9
#define NUM_INPUT_REGS                  3

//OUT SHIFT_REGISTER        
#define latch_pin_out                   24
#define data_pin_out                    23
#define clock_pin_out                   22
#define output_enable                   25
#define NUM_OUTPUT_REGS                 1
#ifdef output_enable
    #define outputEnable() digitalWrite(output_enable, 0)
    #define outputDisable() digitalWrite(output_enable, 1)
#endif

//SHIFT & OFFSET MASK
#define DOR_IN_LOCK_OFFSET              0
#define DOR_2_LOCK_OFFSET               1
#define CHINA_LOCK_OFFSET               2
#define NAPOLEON_LOCK_OFFSET            3
#define IVAN_LOCK_OFFSET                4
#define CESARE_LOCK_OFFSET              5
#define CIRCLE_LOCK_OFFSET              6
#define COLISEUM_EN_OFFSET              7

//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------
RFID reader_1;
RFID reader_2;

//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
enum
{
    EN = 1,
    FR
};

enum
{
    CLOSED,
    OPEN
};

enum
{
    OFF,
    ON,
    PULSE
};

enum
{
    DISABLE,
    ENABLE
};

enum
{
    OK,
    ERR
};

// shift registers data
uint8_t shift_input_buff[NUM_INPUT_REGS] = {0};
uint8_t shift_output_buff[NUM_OUTPUT_REGS] = {0};

//game states
uint16_t prew_ivan_state;
uint16_t current_ivan_state;
uint16_t prew_china_state;
uint16_t current_china_state;
uint16_t prew_napoleon_state;
uint16_t current_napoleon_state;
uint16_t prew_circles_combination;
uint16_t prew_circles_position;
uint16_t circles_combination;
uint16_t circles_position;

//------------------------------------------------------------------------------------------------------
//                                        FLAGS
//------------------------------------------------------------------------------------------------------
bool fire_win_flag;
bool coliseum_win_flag;
bool napoleon_win_flag;
bool ivan_win_flag;
bool china_win_flag;
bool circle_1_win_flag;
bool circle_2_win_flag;

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//HARDWARE
inline void hardware_init(void);

//SHIFT
inline int8_t shiftInData(uint8_t *data_buff, uint32_t len_of_data);
inline int8_t shiftOutData(uint8_t *data_buff, uint32_t len_of_data);

//GAME
//controls
void fireLedControl(uint8_t state);
void fireLightControl(uint8_t state);
void lightFirstRoom(uint8_t state);
void lightSecondRoom(uint8_t state);
void lockInControl(uint8_t state);
void lockSecondRoomControl(uint8_t state);
void lockChinaControl(uint8_t state);
void lockNapoleonControl(uint8_t state);
void lockIvanControl(uint8_t state);
void lockCesareControl(uint8_t state);
void lockCircleControl(uint8_t state);
void gameColiseumControl(uint8_t state);

//states
uint16_t getFireButtonState(void);
uint16_t getChinaState(void);
uint16_t getNapoleonState(void);
uint16_t getColiseumState(void);
uint16_t getCircleState(void);
uint16_t getIvanState(void);

//RESET
void(* resetFunc) (void) = 0;

//------------------------------------------------------------------------------------------------------
//                                       BEHAVIOR
//------------------------------------------------------------------------------------------------------
inline void hardware_init(void)
{
    #ifdef DBG
        Serial.begin(9600);
        Serial.println("___DBG___");
        Serial.println("[DBG] start init ... ");
    #endif

    //------------------------------------------------------ GPIO
    // rfid
    reader_1.RFID_init(RFID_1_PIN);
    reader_2.RFID_init(RFID_2_PIN);

    pinMode(FIRE_LED_PIN, OUTPUT);
    pinMode(FIRE_LIGHT_PIN, OUTPUT);

    pinMode(LIGHT_1_ROOM_PIN, OUTPUT);
    pinMode(LIGHT_2_ROOM_PIN, OUTPUT);

    // shift registers init
    pinMode(data_pin_in, INPUT_PULLUP);
    pinMode(clock_pin_in, OUTPUT);
    pinMode(latch_pin_in, OUTPUT);

    pinMode(data_pin_out, OUTPUT);
    pinMode(clock_pin_out, OUTPUT);
    pinMode(latch_pin_out, OUTPUT);

    #ifdef output_enable
        pinMode(output_enable, OUTPUT);
        outputEnable();
    #endif


    #ifdef DBG
        Serial.println("[DBG] done!");
    #endif
}//hardware_init()
//------------------------------------------------------------------------------------------------------
inline int8_t shiftInData(uint8_t *data_buff, uint32_t len_of_data)
{
    if(data_buff == nullptr)
    {
        return -1;  
    }
    else if(len_of_data == 0)
    {
        return -2;  
    }

    // write data from parallel inputs
    digitalWrite(latch_pin_in, LOW);
    digitalWrite(latch_pin_in, HIGH);

    // serial read data from 74HC165
    for(uint8_t i = 0; i < len_of_data; i++)
    {
        data_buff[i] = 0;
        for(int8_t j = 7; j >=0 ; j--)
        {
            digitalWrite(clock_pin_in, LOW);
            data_buff[i] |= (digitalRead(data_pin_in)) ? (0 << j) : (1 << j);
            digitalWrite(clock_pin_in, HIGH);
        }//for_inner
    }//for

    return 0;
}//shiftInData()
//------------------------------------------------------------------------------------------------------
inline int8_t shiftOutData(uint8_t *data_buff, uint32_t len_of_data)
{
    if(data_buff == nullptr)
    {
        return -1;  
    }
    else if(len_of_data == 0)
    {
        return -2;  
    }

    digitalWrite(latch_pin_out, LOW);

    for(int8_t i = len_of_data - 1; i >= 0; i--)
    {
        shiftOut(data_pin_out, clock_pin_out, MSBFIRST, data_buff[i]);
    }
    digitalWrite(latch_pin_out, HIGH);

    return 0;
}//shiftOutData()
//------------------------------------------------------------------------------------------------------
void fireLedControl(uint8_t state)
{
    static uint8_t led_lvl = 0;
    static int8_t pwm_direction = PWM_STEP;
    static unsigned long btn_timer = 0;

    switch(state)
    {
        case ON:
            digitalWrite(FIRE_LED_PIN, HIGH);
            break;

        case OFF:
            digitalWrite(FIRE_LED_PIN, LOW);
            break;

        case PULSE:
            if((millis() - btn_timer) > BTN_PULSE_DELAY)
            {
                if(led_lvl == 5)
                {
                    pwm_direction = PWM_STEP;
                }
                else if(led_lvl == 255)
                {
                    pwm_direction = 0 - PWM_STEP;
                }

                led_lvl += pwm_direction;  
                analogWrite(FIRE_LED_PIN, led_lvl);
                btn_timer = millis();
            }
            break;
    }//switch
}//fireLedControl()
//------------------------------------------------------------------------------------------------------
void fireLightControl(uint8_t state)
{
    digitalWrite(FIRE_LIGHT_PIN, state);
}//fireLightControl()
//------------------------------------------------------------------------------------------------------
void lightFirstRoom(uint8_t state)
{
    digitalWrite(LIGHT_1_ROOM_PIN, state);
}//lightFirstRoom()
//------------------------------------------------------------------------------------------------------
void lightSecondRoom(uint8_t state)
{
    digitalWrite(LIGHT_2_ROOM_PIN, state);
}//lightSecondRoom()
//------------------------------------------------------------------------------------------------------
void lockInControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << DOR_IN_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << DOR_IN_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockInControl()
//------------------------------------------------------------------------------------------------------
void lockSecondRoomControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << DOR_2_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << DOR_2_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockSecondRoomControl()
//------------------------------------------------------------------------------------------------------
void lockChinaControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << CHINA_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << CHINA_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockChinaControl()
//------------------------------------------------------------------------------------------------------
void lockNapoleonControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << NAPOLEON_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << NAPOLEON_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockNapoleonControl()
//------------------------------------------------------------------------------------------------------
void lockIvanControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << IVAN_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << IVAN_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockIvanControl()
//------------------------------------------------------------------------------------------------------
void lockCesareControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << CESARE_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << CESARE_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockCesareControl()
//------------------------------------------------------------------------------------------------------
void lockCircleControl(uint8_t state)
{
    switch(state)
    {
        case CLOSED:
            shift_output_buff[0] |= (1 << CIRCLE_LOCK_OFFSET);   
            break;

        case OPEN:
            shift_output_buff[0] &= ~(1 << CIRCLE_LOCK_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//lockCircleControl()
//------------------------------------------------------------------------------------------------------
void gameColiseumControl(uint8_t state)
{
    switch(state)
    {
        case ENABLE:
            shift_output_buff[0] |= (1 << COLISEUM_EN_OFFSET);   
            break;

        case DISABLE:
            shift_output_buff[0] &= ~(1 << COLISEUM_EN_OFFSET);
            break;
    }
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//gameColiseumControl()
//------------------------------------------------------------------------------------------------------
uint16_t getFireButtonState(void)
{
    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    
    return (shift_input_buff[0] & (1 << 0)) ? 0x401 : FIRE_BUTTON_CLEAR;
}//getFireButtonState()
//------------------------------------------------------------------------------------------------------
uint16_t getChinaState(void)
{
    prew_china_state = current_china_state;
    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    current_china_state = (shift_input_buff[0] >> 1) & 0x0F;

    if(current_china_state == 0x0F)
    {
        return CHINA_WIN;
    }
    else
    {
        return (CHINA_CLEAR | current_china_state);
    }
}//getChinaState()
//------------------------------------------------------------------------------------------------------
uint16_t getNapoleonState(void)
{
	uint8_t in_0_inverse, in_1_inverse;

    prew_napoleon_state = current_napoleon_state;
    shiftInData(shift_input_buff, NUM_INPUT_REGS);

	in_0_inverse = ~shift_input_buff[0];
	in_1_inverse = ~shift_input_buff[1];

    current_napoleon_state = ((in_0_inverse >> 5) & 0x07) | 
              ((in_1_inverse << 3) & 0x78);

    if(current_napoleon_state == 0x007F)
    {
        return NAPOLEON_WIN;
    }
    else
    {
        return (NAPOLEON_CLEAR | current_napoleon_state);
    }
}//getNapoleonState()
//------------------------------------------------------------------------------------------------------
uint16_t getColiseumState(void)
{
    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    
    return (shift_input_buff[1] & (1 << 4)) ? COLISEUM_WIN : COLISEUM_CLEAR;
}//getColiseumState()
//------------------------------------------------------------------------------------------------------
uint16_t getCircleState(void)
{
    prew_circles_combination = circles_combination;
    prew_circles_position = circles_position;

    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    circles_combination = shift_input_buff[2] & 0x03;
    circles_position = shift_input_buff[1] >> 5;

    switch(circles_combination)
    {
        case 0x01:
            if(circles_position == 0x07)
            {
                return CIRCLE_1_WIN;
            } 
            break;

        case 0x02: 
            if(circles_position == 0x07)
            {
                return CIRCLE_2_WIN;
            } 
            break;
    }//switch

    // never be execute in normal work.
    return CIRCLE_CLEAR;
}//getCircleState()
//------------------------------------------------------------------------------------------------------
uint16_t getIvanState(void)
{
    // Variables for decrease MQTT traffic.
    /// RFID readers when card is present change output from "TRUE" to "FALSE" every few milliseconds.
    /// Counters reset to zero when card not present for 10 calls this function only. And send 1 parsel.
    static uint8_t reader_1_card_counter;
    static uint8_t reader_2_card_counter;

    prew_ivan_state = current_ivan_state;

    reader_1.RFID_find_card();
    if(reader_1.RFID_card_is_present)
    {
        current_ivan_state  = current_ivan_state | (1 << 0);
        reader_1_card_counter = 10;
    }
    else
    {
        (reader_1_card_counter > 0) ? reader_1_card_counter-- : reader_1_card_counter = 0;
        if(reader_1_card_counter == 0)
        {
            current_ivan_state = current_ivan_state & (~(1 << 0));
        }
    }

    reader_2.RFID_find_card(); 
    if(reader_2.RFID_card_is_present)
    {
        current_ivan_state = current_ivan_state | (1 << 1);
        reader_2_card_counter = 10;
    }
    else
    {
        (reader_2_card_counter > 0) ? reader_2_card_counter-- : reader_2_card_counter = 0;
        if(reader_2_card_counter == 0)
        {
            current_ivan_state = current_ivan_state & (~(1 << 1));
        }
    }
    
    return current_ivan_state;
}//getIvanState()
//------------------------------------------------------------------------------------------------------

#endif //FUNCTIONS_INCLUDED