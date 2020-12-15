#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

//------------------------------------------------------------------------------------------------------
//                                        INCLUDDES
//------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
#define DBG

//GPIO
#define	IN_GAME_ENABLE					A1
#define OUT_GAME_SOLVE					3 
#define IS_GAME_ENABLE()				!digitalRead(IN_GAME_ENABLE)
#define GAME_SOLVED()					digitalWrite(OUT_GAME_SOLVE, HIGH);
#define GAME_NOT_SOLVED()				digitalWrite(OUT_GAME_SOLVE, LOW);

//IN SHIFT_REGISTER     
#define latch_pin_in                    6
#define data_pin_in                     4
#define clock_pin_in                    5
#define NUM_INPUT_REGS                  1

//OUT SHIFT_REGISTER        
#define latch_pin_out                   9
#define data_pin_out                    8
#define clock_pin_out                   7
#define output_enable                   10
#define NUM_OUTPUT_REGS                 1
#ifdef output_enable
	#define outputEnable() digitalWrite(output_enable, 0)
	#define outputDisable() digitalWrite(output_enable, 1)
#endif

//SHIFT & OFFSET MASK
#define NUM_OF_BUTTONS                   6
#define NOT_CHANGED                     99
#define NOT_PRESSED                     99

//DELAYS SHOW WIN SEQUENCE
#define SHOW_LED_DELAY                  500
#define HIDE_LED_DELAY                  500
#define PAUSE_SEQUENCE_DELAY            1000


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
	LED_1,
	LED_2,
	LED_3,
	LED_4,
	LED_5,
	LED_6,

	NUM_OF_LEDS
};

enum
{
	ERROR,
	NO_ERROR
};

//-------------------------------------- SHIFT REGISTERS DATA
uint8_t shift_input_buff[NUM_INPUT_REGS] = {0};
uint8_t shift_output_buff[NUM_OUTPUT_REGS] = {0};

//-------------------------------------- GAME VARIABLES
uint8_t leds_state[NUM_OF_LEDS];
uint8_t buttons_state[NUM_OF_BUTTONS];
uint8_t game_counter = 0;
uint8_t win_sequence[NUM_OF_BUTTONS];

bool btn_state_changed;
uint8_t pressed_button;

unsigned long show_sequence_timer;

//------------------------------------------------------------------------------------------------------
//                                        FLAGS
//------------------------------------------------------------------------------------------------------
bool game_start_flag = false;

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

void ledsControl(uint8_t led_number, uint8_t state);
void refreshLeds(void);
uint8_t getBtnState(void);  //return btn number if button state changed (new button pressed) or NOT_PRESSED (NOT_CHANGED)
inline void showWinSequence(void);
uint8_t checkWin(void);

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
	pinMode(IN_GAME_ENABLE, INPUT);
	pinMode(OUT_GAME_SOLVE, OUTPUT);

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
    //  Serial.println("[DBG] shift in ");
    //    for(uint8_t i = 0; i < len_of_data; i++)
    //    {
    //     Serial.print("data_buff[");
    //     Serial.print(i);
    //     Serial.print("] = ");
    //     Serial.println(data_buff[i]);
    //    }//for
    //    Serial.println("<<< ");
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
    //  Serial.println("[DBG] function shift out");
    //    Serial.print("data_buff[0] = ");
    //    Serial.println(data_buff[0]);
    //    Serial.print("data_buff[1] = ");
    //    Serial.println(data_buff[1]);
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
void ledsControl(uint8_t led_number, uint8_t state)
{
    if(led_number == NUM_OF_LEDS)
    {
       switch(state)
       {
            case ON:
                shift_output_buff[0] = 0xFF;
                break;

            case OFF:
                shift_output_buff[0] = 0x00;
                break;
       }//switch

       for(uint8_t i = 0; i < NUM_OF_LEDS; i++)
       {
           leds_state[i] = (state == ON) ? 1 : 0;
       }//for
    }//if all spots
    else
    {
        (state == ON) ? shift_output_buff[0] |= (1 << led_number) : shift_output_buff[0] &= ~(1 << led_number);
        leds_state[led_number] = (state == ON) ? 1 : 0 ; //for refreshLeds  function
    }

    shiftOutData(shift_output_buff, NUM_OUTPUT_REGS);
}//spotLightControl()
//------------------------------------------------------------------------------------------------------
void refreshLeds(void)
{
	for(uint8_t i = 0; i < NUM_OF_LEDS; i++)
	{
		(leds_state[i] == ON) ? ledsControl(i, ON) : ledsControl(i, OFF); 
	}
}//refreshSpots()
//------------------------------------------------------------------------------------------------------
uint8_t getBtnState(void)
{
    uint8_t current_button;

    current_button = NOT_PRESSED;

    shiftInData(shift_input_buff, NUM_INPUT_REGS);
    for(uint8_t i = 0; i < NUM_OF_BUTTONS; i++)
    {
        buttons_state[i] = ((shift_input_buff[0] & (1 << i)) != 0) ? 1 : 0 ;
    }

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
    {    
		delay(5);
	    do
	    {
	        shiftInData(shift_input_buff, NUM_INPUT_REGS);
	    } while(shift_input_buff[0] != 0);
	}
    
    return current_button;

}//getBtnState()
//------------------------------------------------------------------------------------------------------
uint8_t checkWin(void)
{
	if(pressed_button == win_sequence[game_counter])
	{
		return NO_ERROR;
	}
	else
	{
		return ERROR;
	}
}//checkWin()
//------------------------------------------------------------------------------------------------------
inline void showWinSequence(void)
{
	unsigned long delay_timer;

	for(uint8_t i = 0; i < NUM_OF_LEDS; i++)
	{
		ledsControl(win_sequence[i], ON);
		delay_timer = millis();
		while((millis() - delay_timer) < SHOW_LED_DELAY)
		{
			pressed_button = getBtnState();
			if(pressed_button != NOT_PRESSED)
			{
				game_start_flag = true;
				return;
			}
			delay(5);
		}

		ledsControl(win_sequence[i], OFF);
		delay_timer = millis();
		while((millis() - delay_timer) < HIDE_LED_DELAY)
		{
			pressed_button = getBtnState();
			if(pressed_button != NOT_PRESSED)
			{
				game_start_flag = true;
				return;
			}
			delay(5);
		}
	}
}//showWinSequence()
//------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------
#endif //_FUNCTIONS_H