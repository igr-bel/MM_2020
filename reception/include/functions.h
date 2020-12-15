#ifndef FUNCTIONS_INCLUDED
#define FUNCTIONS_INCLUDED

#include <Arduino.h>
#include <DFMiniMp3.h>
#include <SoftwareSerial.h>

#include "keypad.h"
#include "hints.h"

//------------------------------------------------------------------------------------------------------
//                                        DEFINES
//------------------------------------------------------------------------------------------------------
#define DBG                             
#define NOP                             do{}while(0)

//GPIO
#define PHONE_TUBE_PIN                  47
#define DOOR_LOOCK_PIN                  46
#define DRAWER_LOOCK_PIN                48

//IN SHIFT_REGISTER     
#define latch_pin_in                    2
#define data_pin_in                     18
#define clock_pin_in                    20
#define NUM_INPUT_REGS                  1

//OUT SHIFT_REGISTER        
#define latch_pin_out                   9
#define data_pin_out                    9
#define clock_pin_out                   9
// #define output_enable                   9
#define NUM_OUTPUT_REGS                 2
#ifdef output_enable
    #define outputEnable() digitalWrite(output_enable, 0)
    #define outputDisable() digitalWrite(output_enable, 1)
#endif

//SHIFT & OFFSET MASK
#define DRAWERS_WIN_OFFSET              0x0F
#define KEYS_WIN_OFFSET                 0x0F

//DF player
#define player_pin_tx1                  5   //TX1
//#define player_pin_rx1                9   //RX1
#ifndef player_pin_rx1
  #define busy_pin_1                    6
#endif

#define VOLUME                          10

//------------------------------------------------------------------------------------------------------
//Class for DF player
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError(uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved(DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};
//------------------------------------------------------------------------------------------------------
//                                        OBJECTS
//------------------------------------------------------------------------------------------------------
#ifdef player_pin_rx1
  SoftwareSerial mp3_Serial(player_pin_rx1, player_pin_tx1); // RX, TX
#else
  SoftwareSerial mp3_Serial(7, player_pin_tx1); // RX, TX
#endif
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(mp3_Serial);

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
	OPEN,
    CLOSED
};

// phone
keypad_T key;
uint8_t phone_err;
const uint8_t phone_number_length = 3;
uint8_t win_phone_number[phone_number_length] = {_7, _7, _7};
uint8_t game_phone_number[phone_number_length] = {0};
uint8_t phone_number_counter;
unsigned long phone_timer;
unsigned long phone_start_dial;
uint16_t phone_max_dial_time = 500;  //reset keyboard after hold button more this time
bool phone_number_correct;
bool phone_enable;
bool wait_btn_release = false;
bool phone_busy = false;
bool loop_sound = false;
bool err_sound = false;
bool phone_send_ui_symbol = false;

//hints
volatile uint16_t hint_stage;
uint16_t prew_hint_stage;
bool hint_repeat;
uint8_t hint_lang;
bool hint_played = false;

// drawers
volatile uint8_t current_drawers_state;
volatile uint8_t prew_drawers_state;

// keys
volatile uint8_t current_keys_state;
volatile uint8_t prew_keys_state;

//------------------------------------------------------------------------------------------------------
//                                        FLAGS
//------------------------------------------------------------------------------------------------------
bool fire_hidrant_win_flag;
bool diorama_win_flag;
bool drawers_win_flag;
bool keys_win_flag;
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

//HINTS
inline void takeHint(void);
void changeHint(void);

//MP3
inline void playSound(uint8_t track);
inline void stopSound(void);
inline void mp3_init(void);

//PHONE
void phoneGame(void);

//GAME
uint8_t getDrawersState(void);
uint8_t getKeysState(void);
void doorLockControl(uint8_t state);
void drawerLockControl(uint8_t state);

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
    pinMode(PHONE_TUBE_PIN, INPUT);
    pinMode(DOOR_LOOCK_PIN, OUTPUT);
    pinMode(DRAWER_LOOCK_PIN, OUTPUT);

    // shift registers init
    pinMode(data_pin_in, INPUT_PULLUP);
    pinMode(clock_pin_in, OUTPUT);
    pinMode(latch_pin_in, OUTPUT);

    #ifdef output_enable
        pinMode(output_enable, OUTPUT);
        outputEnable();
    #endif


    #ifdef busy_pin_1
        pinMode(busy_pin_1, INPUT);
    #endif


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
    //    for(int i = 0; i < len_of_data; i++)
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
inline void playSound(uint8_t track)
{
    mp3.stop();
    delay(5);
    mp3.playMp3FolderTrack(track);

    #ifdef DBG     
        Serial.print("[DBG] play track - ");
        Serial.println(track); 
    #endif
  
}//playSound()
//------------------------------------------------------------------------------------------------------
inline void stopSound(void)
{
    mp3.stop();
    delay(5);
}//stopSound()
//------------------------------------------------------------------------------------------------------
inline void mp3_init(void)
{
    #ifdef busy_pin_1
        pinMode(busy_pin_1, INPUT);
        #define mp3isBusy !digitalRead(busy_pin_1)
    #endif

    mp3_Serial.begin (9600);
    mp3.begin();
    mp3.setVolume(VOLUME);

}//mp3_init()
//------------------------------------------------------------------------------------------------------
void phoneGame(void)
{
    phone_err = 1;

    if(hint_played && !digitalRead(busy_pin_1))
    {
		//if tube down and hint still played
		phone_enable = !digitalRead(PHONE_TUBE_PIN);
    	if(phone_enable)
        	return;
		else
			mp3.stop();
    }
    else
    {
        if(hint_played)
        {
            phone_send_ui_symbol = false;
        }

        hint_played = false;
    }
    
    
    //if wait release
    if(wait_btn_release)
    {
        key = getKey();
        if(key.state != RELEASED)
        {
            return;
        }
        else
        {
            wait_btn_release = false;
        }
    }

    phone_enable = !digitalRead(PHONE_TUBE_PIN);

    if(phone_enable)
    {
        if(phone_number_counter == 0 && !loop_sound && !err_sound)
        {
            // mp3.loopGlobalTrack(PHONE_BEEP_SOUND);
			mp3.playMp3FolderTrack(PHONE_BEEP_SOUND);
            loop_sound = true;
        }  

        key = getKey();
        if(key.key_code != _NONE)
        {
            mp3.stop();
			// mp3.loopGlobalTrack(PHONE_BTN_SOUND);
			mp3.playMp3FolderTrack(PHONE_BTN_SOUND);
            loop_sound = false;

            delay(150);
            if(phone_number_counter < phone_number_length)
            {
                game_phone_number[phone_number_counter] = key.key_code;
                phone_number_counter++;
            }
            else
            {
                phone_number_counter = 0;
                for(uint8_t i = 0; i < phone_number_length; i++)
                {
                    game_phone_number[i] = _NONE;
                }
            }

            phone_start_dial = millis();
            do
            {
                if(millis() - phone_start_dial > phone_max_dial_time)
                {
                    wait_btn_release = true;
                    break;
                }
                key = getKey();
            } while(key.state == HOLD);

            //check correct number
            if(phone_number_counter == phone_number_length)
            {
                phone_err = 0;
                for(uint8_t i = 0; i < phone_number_length; i++)
                {
                    if(win_phone_number[i] != game_phone_number[i])
                      phone_err = 1;
                }

                if(phone_err)
                {
                    // mp3.loopGlobalTrack(PHONE_ERR_SOUND);
					mp3.playMp3FolderTrack(PHONE_ERR_SOUND);
                    err_sound = true;
                }

                phone_number_counter = 0;
            }//check number

        }//if phone button pressed
    }//if phone enable (tube is up)
    else
    {
        //stop sound
        stopSound();
        loop_sound = false;
        err_sound = false;

        //clear game array
        for(uint8_t i = 0; i < phone_number_length; i++)
        {
            game_phone_number[i] = _NONE;
            phone_number_counter = 0;
        }
    }
    

    return;
}//phoneGame()
//------------------------------------------------------------------------------------------------------
inline void takeHint(void)
{
    hint_played = true;
    err_sound = true;

    switch(hint_stage)
    {
        case FIRE_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(FIRE_SOUND_1) : playSound(FIRE_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(FIRE_SOUND_2) : playSound(FIRE_SOUND_2_FR);
            break;


        case DIORAMA_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(DIORAMA_SOUND_1) : playSound(DIORAMA_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(DIORAMA_SOUND_2) : playSound(DIORAMA_SOUND_2_FR);
            break;

        case DRAWERS_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(DRAWERS_SOUND_1) : playSound(DRAWERS_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(DRAWERS_SOUND_2) : playSound(DRAWERS_SOUND_2_FR);
            break;

        case KEYS_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(KEYS_SOUND_1) : playSound(KEYS_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(KEYS_SOUND_2) : playSound(KEYS_SOUND_2_FR);
            break;

        case COLISEUM_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(COLISEUM_SOUND_1) : playSound(COLISEUM_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(COLISEUM_SOUND_2) : playSound(COLISEUM_SOUND_2_FR);
            break;

        case NAPOLEON_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(NAPOLEON_SOUND_1) : playSound(NAPOLEON_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(NAPOLEON_SOUND_2) : playSound(NAPOLEON_SOUND_2_FR);
            break;

        case IVAN_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(IVAN_SOUND_1) : playSound(IVAN_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(IVAN_SOUND_2) : playSound(IVAN_SOUND_2_FR);
            break;

        case CHINA_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(CHINA_SOUND_1) : playSound(CHINA_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(CHINA_SOUND_2) : playSound(CHINA_SOUND_2_FR);
            break;

        case CIRC_1_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(CIRC1_SOUND_1) : playSound(CIRC1_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(CIRC1_SOUND_2) : playSound(CIRC1_SOUND_2_FR);
            break;

        case CIRC_2_HINT:
            if(!hint_repeat)
                (hint_lang == EN) ?  playSound(CIRC2_SOUND_1) : playSound(CIRC2_SOUND_1_FR);
            else
                (hint_lang == EN) ?  playSound(CIRC2_SOUND_2) : playSound(CIRC2_SOUND_2_FR);
            break;

        case NULL_HINT:
            break;

    }//switch hint

    delay(100);

    hint_repeat = true;

}//takeHint()
//------------------------------------------------------------------------------------------------------
uint8_t getDrawersState(void)
{
    uint8_t buff[1];
    prew_drawers_state = current_drawers_state;
    current_drawers_state = 0;

    shiftInData(buff, 1);
    current_drawers_state = buff[0] & DRAWERS_WIN_OFFSET;

    return current_drawers_state;
}//getDrawersState()
//------------------------------------------------------------------------------------------------------
uint8_t getKeysState(void)
{
    uint8_t buff[1];
    prew_keys_state = current_keys_state;
    current_keys_state = 0;

    shiftInData(buff, 1);
    current_keys_state = (buff[0] >> 4) & KEYS_WIN_OFFSET;

    return current_keys_state;
}//getKeysState()
//------------------------------------------------------------------------------------------------------
void doorLockControl(uint8_t state)
{
    digitalWrite(DOOR_LOOCK_PIN, state);
}//doorLockControl()
//------------------------------------------------------------------------------------------------------
void drawerLockControl(uint8_t state)
{
    digitalWrite(DRAWER_LOOCK_PIN, state);
}//drawerLockControl()
//------------------------------------------------------------------------------------------------------
void changeHint(void)
{
	switch(hint_stage)
	{
		case FIRE_HINT:
			hint_stage = (fire_hidrant_win_flag) ?  DIORAMA_HINT : FIRE_HINT;
			break;

		case DIORAMA_HINT:
			hint_stage = (diorama_win_flag) ? DRAWERS_HINT : DIORAMA_HINT;
			break;

		case DRAWERS_HINT:
    		hint_stage = (drawers_win_flag == true) ? KEYS_HINT : DRAWERS_HINT;
			break;

		case KEYS_HINT:
    		hint_stage = (keys_win_flag) ? COLISEUM_HINT : KEYS_HINT;
			break;

		case COLISEUM_HINT:
    		hint_stage = (coliseum_win_flag) ? NAPOLEON_HINT : COLISEUM_HINT;
			break;

		case NAPOLEON_HINT:
			hint_stage = (napoleon_win_flag) ? IVAN_HINT : NAPOLEON_HINT;
			break;

		case IVAN_HINT:
			hint_stage = (ivan_win_flag) ? CHINA_HINT : IVAN_HINT;   
			break;

		case CHINA_HINT:
			hint_stage = (china_win_flag) ? CIRC_1_HINT : CHINA_HINT;
			break;

		case CIRC_1_HINT:
			hint_stage = (circle_1_win_flag) ? CIRC_2_HINT : CIRC_1_HINT;
			break;

		case CIRC_2_HINT:
			hint_stage = (circle_2_win_flag) ? NULL_HINT : CIRC_2_HINT;
			break;
	}//switch

	if(prew_hint_stage != hint_stage)
	{
    	hint_repeat = false;
		prew_hint_stage = hint_stage;
	}
}//changeHint()
//------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------



#endif //FUNCTIONS_INCLUDED