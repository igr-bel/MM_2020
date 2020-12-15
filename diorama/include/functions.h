#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
// #define DBG

//GPIO
#define btn_9_pin                       A1
#define btn_10_pin                      2

//IN SHIFT_REGISTER     
#define latch_pin_in                    7
#define data_pin_in                     3
#define clock_pin_in                    4
#define NUM_INPUT_REGS                  1

//OUT SHIFT_REGISTER        
#define latch_pin_out                   8
#define data_pin_out                    A5
#define clock_pin_out                   A4
#define output_enable                   9
#define NUM_OUTPUT_REGS                 2
#ifdef output_enable
    #define outputEnable() digitalWrite(output_enable, 0)
    #define outputDisable() digitalWrite(output_enable, 1)
#endif

//SHIFT & OFFSET MASK
#define MAG_LOCK_OFFSET                 5
#define MAIN_LIGHT_OFFSET               4

#define SIDE_1                          6
#define SIDE_2                          7

#define NUM_OF_BUTTONS                  10
#define NOT_CHANGED                     99
#define NOT_PRESSED                     99

//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------
//                                        GLOBAL VARIABLES
//------------------------------------------------------------------------------------------------------
enum
{
    EN = 1,
    FR
};

uint8_t hint_lang = EN;

enum
{
  OFF,
  ON
};

enum
{
  SPOT_1,
  SPOT_2,
  SPOT_3,
  SPOT_4,
  SPOT_5,
  SPOT_6,
  SPOT_7,
  SPOT_8,
  SPOT_9,
  SPOT_10,
  SPOT_11,
  SPOT_12,

  NUM_OF_SPOTS
};

uint8_t current_game_side;

//-------------------------------------- SHIFT REGISTERS DATA
uint8_t shift_input_buff[NUM_INPUT_REGS] = {0};
uint8_t shift_output_buff[NUM_OUTPUT_REGS] = {0};

//-------------------------------------- GAME VARIABLES
uint8_t spots_state[NUM_OF_SPOTS];
uint8_t *game_spots_state[NUM_OF_SPOTS - 2];
uint8_t buttons_state[NUM_OF_BUTTONS];
uint8_t button_spot_link[NUM_OF_BUTTONS];

bool btn_state_changed;

//------------------------------------------------------------------------------------------------------
//                                        PROTOTYPES
//------------------------------------------------------------------------------------------------------
//HARDWARE
inline void hardware_init(void);

//RESET
void(* resetFunc) (void) = 0;

inline int8_t shiftInData(uint8_t *data_buff, uint32_t len_of_data);
int8_t shiftOutData(uint8_t *data_buff, uint32_t len_of_data);

void shuffleArr(uint8_t * arr, int16_t length);
void mainLightControl(uint8_t state);
void magLatchOpen(void);
void spotLightControl(uint8_t spot_number, uint8_t state);
void refreshSpots(void);
uint8_t getBtnState(void);  //return btn number if button state changed (new button pressed) or 99
bool isWin(uint8_t game_side);

//------------------------------------------------------------------------------------------------------
//                                       BEHAVIOR
//------------------------------------------------------------------------------------------------------
void hardware_init(void)
{
    #ifdef DBG
        Serial.begin(9600);
        Serial.println("___DBG___");
        Serial.println("[DBG] start init ... ");
    #endif

    //------------------------------------------------------ GPIO
    // shift registers init
    pinMode(data_pin_in, INPUT_PULLUP);
    pinMode(clock_pin_in, OUTPUT);
    pinMode(latch_pin_in, OUTPUT);

    pinMode(data_pin_out, OUTPUT);
    pinMode(clock_pin_out, OUTPUT);
    pinMode(latch_pin_out, OUTPUT);

    #ifdef output_enable
        pinMode(output_enable, OUTPUT);
    #endif
    outputDisable();

    pinMode(btn_9_pin, INPUT_PULLUP);
    pinMode(btn_10_pin, INPUT_PULLUP);

    //------------------------------------------------------ RAND INITS
    randomSeed(analogRead(0));

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

    //записать данные с входов во внутренний регистр 
    digitalWrite(latch_pin_in, LOW);
    digitalWrite(latch_pin_in, HIGH);

    //последовательное получение данных из 74HC165
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

    #ifdef DBG
     Serial.println("[DBG] shift in ");
       for(int i = 0; i < len_of_data; i++)
       {
        Serial.print("data_buff[");
        Serial.print(i);
        Serial.print("] = ");
        Serial.println(data_buff[i]);
       }//for
       Serial.println("<<< ");
    #endif

    return 0;
}//shiftInData()
//------------------------------------------------------------------------------------------------------
int8_t shiftOutData(uint8_t *data_buff, uint32_t len_of_data)
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

    #ifdef DBG
     Serial.println("[DBG] function shift out");
       Serial.print("data_buff[0] = ");
       Serial.println(data_buff[0]);
       Serial.print("data_buff[1] = ");
       Serial.println(data_buff[1]);
    #endif

    return 0;
}//shiftOutData()
//------------------------------------------------------------------------------------------------------
void shuffleArr(uint8_t * arr, int16_t length)
{
    int8_t foo;
    int8_t rand;

    for (uint8_t i = 0; i < length; i++)
    {
        rand = (uint8_t)(random() % length);
        foo = arr[rand];
        arr[rand] = arr[i];
        arr[i] = foo;
    }
    
    return;
}//shuffle()
//------------------------------------------------------------------------------------------------------
void mainLightControl(uint8_t state)
{
    if(state == ON)
    {
        shift_output_buff[1] |= (1 << MAIN_LIGHT_OFFSET);   
    }
    else if(state == OFF)
    {
        shift_output_buff[1] &= ~(1 << MAIN_LIGHT_OFFSET);
    }

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//mainLightControl()
//------------------------------------------------------------------------------------------------------
void magLatchOpen(void)
{
    //short pulse for magnet latch 
    shift_output_buff[1] |= (1 << MAG_LOCK_OFFSET);  
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS); 
    delay(500);
    shift_output_buff[1] &= ~(1 << MAG_LOCK_OFFSET);
    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//magLockControl()
//------------------------------------------------------------------------------------------------------
void spotLightControl(uint8_t spot_number, uint8_t state)
{
    if(spot_number == NUM_OF_SPOTS)
    {
       switch(state)
       {
            case ON:
                shift_output_buff[0] = 0xFF;
                shift_output_buff[1] |= 0x0F;
                break;

            case OFF:
                shift_output_buff[0] = 0x00;
                shift_output_buff[1] &= 0xF0;
                break;
       }//switch

       for(uint8_t i = 0; i < NUM_OF_SPOTS; i++)
       {
           spots_state[i] = (state == ON) ? 1 : 0;
       }//for
    }//if all spots
    else if(spot_number < 8)
    {
        (state == ON) ? shift_output_buff[0] |= (1 << spot_number) : shift_output_buff[0] &= ~(1 << spot_number);
        spots_state[spot_number] = (state == ON) ? 1 : 0 ; //for refreshSpots  function
    }
    else
    {
        (state == ON) ? shift_output_buff[1] |= (1 << (spot_number - 8)) : shift_output_buff[1] &= ~(1 << (spot_number - 8));
        spots_state[spot_number] = (state == ON) ? 1 : 0 ; //for refreshSpots  function
    }//if_else

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//spotLightControl()
//------------------------------------------------------------------------------------------------------
void refreshSpots(void)
{
    for(uint8_t i = 0; i < NUM_OF_SPOTS; i++)
    {
        (spots_state[i] == ON) ? spotLightControl(i, ON) : spotLightControl(i, OFF); 
    }
}//refreshSpots()
//------------------------------------------------------------------------------------------------------
uint8_t getBtnState(void)
{
    uint8_t current_button;

    current_button = NOT_PRESSED;

    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    for(uint8_t i = 0; i < NUM_OF_BUTTONS - 2; i++)
    {
        buttons_state[i] = ((shift_input_buff[0] & (1 << i)) != 0) ? 1 : 0 ;
    }
    buttons_state[8] = !digitalRead(btn_9_pin);
    buttons_state[9] = !digitalRead(btn_10_pin);

    for(uint8_t i = 0; i < NUM_OF_BUTTONS; i++)
    {
        if(buttons_state[i] == 1)
        {
            current_button = i;
            break;
        }
    }

    //wait button release
    if(current_button != NOT_PRESSED)
        delay(7);
    do
    {
        shiftInData(shift_input_buff, NUM_INPUT_REGS);
    } while(shift_input_buff[0] != 0 || !digitalRead(btn_9_pin) || !digitalRead(btn_10_pin));
    
    return current_button;

}//getBtnState()
//------------------------------------------------------------------------------------------------------
bool isWin(uint8_t game_side)
{
    uint8_t winner = 1;

    switch(game_side)
    {
        case SIDE_1:
            for(uint8_t i = 0; i < 5; i++)
            {
                winner = (*game_spots_state[i] == 0) ? 0 : winner; 
            }
            for(uint8_t i = 5; i < 10; i++)
            {
                winner = (*game_spots_state[i] == 1) ? 0 : winner; 
            }
            break;

        case SIDE_2:
            for(uint8_t i = 5; i < 10; i++)
            {
                winner = (*game_spots_state[i] == 0) ? 0 : winner; 
            }
            for(uint8_t i = 0; i < 5; i++)
            {
                winner = (*game_spots_state[i] == 1) ? 0 : winner; 
            }
            break;
    }//switch

    return winner;

}//isWin()
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
#endif //_FUNCTIONS_H