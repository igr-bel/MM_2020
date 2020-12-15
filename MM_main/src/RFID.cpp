#include "Arduino.h"
#include "RFID.h"
 
//------------------------------------------------------------------------------------------------------
void RFID::RFID_init(uint8_t pin)
{
	Pin_RFID = pin;
}//RFID_init()
//------------------------------------------------------------------------------------------------------
void RFID::RFID_find_card()
{
	RFID_card_is_present = false;

    pinMode(Pin_RFID, OUTPUT);
    digitalWrite(Pin_RFID, LOW);
    delayMicroseconds(Time_init);
    pinMode(Pin_RFID, INPUT_PULLUP);

    delayMicroseconds(70);
	
    if(digitalRead(Pin_RFID) == 0)
	{ 
		RFID_card_is_present = true;
	}

    delayMicroseconds(410);
}//RFID_detection()
//------------------------------------------------------------------------------------------------------
uint8_t RFID::RFID_read_bit()
{
    uint8_t bit;

    pinMode(Pin_RFID,OUTPUT);
    digitalWrite(Pin_RFID,LOW);
    delayMicroseconds(1);

    pinMode(Pin_RFID,INPUT_PULLUP);
    delayMicroseconds(12);

    bit = digitalRead(Pin_RFID);
    delayMicroseconds(60);

    return bit;
}//FID_read_bit()
//------------------------------------------------------------------------------------------------------
uint8_t RFID::RFID_read_byte()
{
    uint8_t i;
    uint8_t byte_readed = 0x00;

    for (i = 0; i < 8; i++)
    {
        byte_readed |= RFID_read_bit();
	    
        if(i != 7)
	    {
            byte_readed = (byte_readed << 1);
        }//if
    }//for

    return byte_readed;
}//RFID_read_byte()
//------------------------------------------------------------------------------------------------------
void RFID::RFID_write_bit(uint8_t bit)
{
    pinMode(Pin_RFID,OUTPUT);
    digitalWrite(Pin_RFID,LOW);
    if(bit == 0)
    {
        delayMicroseconds(70);
    }//if

    if(bit == 1)
    {
        delayMicroseconds(command_period);
        pinMode(Pin_RFID, INPUT_PULLUP);
        delayMicroseconds(70);
    }//if

    pinMode(Pin_RFID,INPUT_PULLUP);
}//RFID_write_bit()
//------------------------------------------------------------------------------------------------------
void RFID::RFID_write_byte(uint8_t byte)
{
    uint8_t i, k;

    for(i = 0; i < 8; i++)
    {
        k = byte & 0x01;
        RFID_write_bit(k);
        byte = (byte >> 1);
    }//for

    delayMicroseconds(command_period);
}//RFID_write_byte(
//------------------------------------------------------------------------------------------------------
void RFID::RFID_read_card()
{
    uint8_t i;

    RFID_find_card();

    if(RFID_card_is_present)
    {
	    RFID_write_byte(0x33);
	    for (i = 0; i < 8; i++)
	    {
		    id[i] = RFID_read_byte();
	    }//for
    }
    else
    {
	    for (i = 0; i < 8; i++)
	    {
		    id[i] = 0xFF;
	    }//for
    }//if else

}//RFID_deconnection()
//------------------------------------------------------------------------------------------------------
