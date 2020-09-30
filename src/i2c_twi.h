#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//registers def
#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define TLOW_REG 0x02
#define THIGH_REG 0x03

//start
#define START             0x08
#define REPEATED_START    0X10

//write
#define MT_SLA_ACK        0x18
#define MT_SLA_NOACK      0x20
#define MT_SLA_DATA_ACK   0x28
#define MT_SLA_DATA_NOACK 0x20

//read
#define MT_SLA_ACK_R        0x40
#define MT_SLA_NOACK_R      0x48
#define MT_SLA_DATA_ACK_R   0x50
#define MT_SLA_DATA_ACK_REP_R 0x58 //repeated ack
#define MT_SLA_DATA_NOACK_R   0X38

//error size
#define MAX_SIZE_STR        64

typedef enum {
    SD = 8,
    TM = 9,
    POL = 10,
    F0 = 11,
    F1 = 12,
    R0 = 13,
    R1 = 14,
    OS = 15,
    EM = 4, 
    AL = 5,
    CR0 = 6,
    CR1 = 7 
} ConfigBit_t;

struct TMP102 {
    // Attr
    uint8_t addr;
    char *ErrPtr; // Last error that ever happened. (doesn't clear until new error occur | empty means no error ever happened)
    bool local_POL;  //for ISR handling | POL 0: false, POL 1: true
    bool local_ISR;  // for ISR handling | interupt mode: false, compartor mode: true
    bool em; // for post i2c data processing

    //I2C methods
    //public
    void (*i2c_init_baud_rate)(uint8_t buad_rate, uint8_t prescaler);
    void (*i2c_write)(uint8_t data[], int size);
    double (*i2c_read)(uint8_t reg_addr, uint8_t *pbyte);
    bool (*alert)(void); 
    void (*config_bit)(ConfigBit_t bit, bool val);
    void (*config_conv_rate)(uint8_t val);
    void (*config_faults)(uint8_t val);
    void (*config_THIGH)(double temp);
    void (*config_TLOW)(double temp);
    uint16_t (*cal_temp)(uint8_t byte[]);
};

    //private
    void _i2c_point(uint8_t reg_addr);
    void _i2cStart(void);
    void _i2cStop(void);
    void _i2cReply(void);
    void _i2cStatus(uint8_t expected, uint8_t error);
    void _error_check(uint8_t error);


extern const struct TMP102Class {
    void (*new)(void* this, uint8_t address, bool extended_mode);
} TMP102;


