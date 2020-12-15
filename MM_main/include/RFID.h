#ifndef RFID_h
#define RFID_h


#define Time_init 480
#define period 60
#define command_period 10


class RFID
{
    public:
        uint8_t Pin_RFID;
        uint8_t id[8];
        bool RFID_card_is_present;

    public:
        void RFID_init(uint8_t pin);
        void RFID_find_card();

        uint8_t RFID_read_bit();
        uint8_t RFID_read_byte();

        void RFID_write_bit(uint8_t bit);
        void RFID_write_byte(uint8_t byte);

        void RFID_read_card();
};

#endif //RFID_h
