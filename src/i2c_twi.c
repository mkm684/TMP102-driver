#include "i2c_twi.h"

//I2C
struct TMP102 *me; // intenral use

/* equation
SCL frequency (BR) = (F_CPU) / (16 + 2*TWBR * prescaler)*/
static void i2c_init_baud_rate(uint8_t buad_rate, uint8_t prescaler)
{
    // SCL freq 50khz
    TWSR = 0x00;
    TWBR = 0x96;            
    TWCR |= 1 << TWEN;
}

//TRANSMISSION
static void i2c_write(uint8_t data[], int size) 
{
    // START BIT
    _i2cStart(); 
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != START && (TWSR & 0xF8) != REPEATED_START)
        _error_check(0); // no specfifc error case 

    // TRANSMIT ADDRESS
    TWDR = (me->addr << 1) & 0xFE;
    _i2cReply();
    while (!(TWCR & (1<<TWINT)));
    _i2cStatus(MT_SLA_ACK, MT_SLA_NOACK);

    // TRANSMIT DATA
    int i = 0;
    while(i < size){
        TWDR = data[i];
        _i2cReply();
        while (!(TWCR & (1<<TWINT)));
        _i2cStatus(MT_SLA_DATA_ACK, MT_SLA_DATA_NOACK);
        i++;
    }

    // STOP
    _i2cStop();
}

static double i2c_read(uint8_t reg_addr, uint8_t *pbyte)
{
    // POINT
    _i2c_point(reg_addr);
    // START BIT
    _i2cStart(); 
    while (!(TWCR & (1<<TWINT)));
    if ((TWSR & 0xF8) != START && (TWSR & 0xF8) != REPEATED_START)
        _error_check(0); // no specfifc error case 

    // TRANSMIT ADDRESS
    TWDR = (me->addr << 1) | 0x01;
    _i2cReply();
    while (!(TWCR & (1<<TWINT)));
    _i2cStatus(MT_SLA_ACK_R, MT_SLA_NOACK_R);

    // GET DATA
    uint8_t bytes_num = 0; 
    while (bytes_num < 2)
    {   
        _i2cReply();
        while (!(TWCR & (1<<TWINT)));
        pbyte[bytes_num] = (uint8_t)TWDR;
        if ((TWSR & 0xF8) != MT_SLA_DATA_ACK_R 
                && (TWSR & 0xF8) != MT_SLA_DATA_ACK_REP_R)
            _error_check(MT_SLA_DATA_NOACK_R);
        bytes_num++;
    }

    // STOP
    _i2cStop();
    
    uint16_t temprature = me->cal_temp(pbyte);
    return (double)temprature/100;
}

static bool alert(void) 
{
    uint8_t curr_conf[2];
    i2c_read(CONFIG_REG, curr_conf);
    if (curr_conf[1] & (1 << AL)) {
        return true;
    }
    return false;
}

//CONFIG
static void config_bit(ConfigBit_t bit, bool val)
{
    uint8_t curr_conf[2];
    i2c_read(CONFIG_REG, curr_conf);
    if (bit == AL){
        strcpy(me->ErrPtr, "Err: Write to AL read-only bit\n");
        return;
    }

    uint8_t data[3] = {CONFIG_REG, curr_conf[0], curr_conf[1]}; 
    if (bit > 7) {
        if (val) {
            data[1] |= (1<<bit-8);
        } else {
            data[1] &= ~(1<<bit-8);
        }
    } else {
        if(val){
            data[2] |= (1<<bit);
        } else {
            data[2] &= ~(1<<bit);
        }
    }

    i2c_write(data, 3);
    if (bit == EM)
        me->em = val;
    else if (bit == POL)
        me->local_POL = val;
    else if (bit == TM)
        me->local_ISR = val;
}

    //0:0.25hz
static void config_conv_rate(uint8_t val)
{
    if (val == 0) {      // 0.25 hz
        config_bit(R0, false);
        config_bit(R1, false);
    }else if(val == 1) { // 1 hz
        config_bit(R0, false);
        config_bit(R1, true);
    }else if(val == 4) { // 4 hz
        config_bit(R0, true);
        config_bit(R1, false);
    }else if(val == 8) { // 8 hz
        config_bit(R0, true);
        config_bit(R1, true);
    }else{
        strcpy(me->ErrPtr, "Err: Invalid conv rate mode\n");
    }
}

static void config_faults(uint8_t val)
{   
    if (val == 1) {     // 1 fault
        config_bit(F0, false);
        config_bit(F1, false);
    }else if(val == 2){ // 2 fault
        config_bit(F0, false);
        config_bit(F1, true);
    }else if(val == 4){ // 4 fault
        config_bit(F0, true);
        config_bit(F1, false);
    }else if(val == 8){ // 8 fault
        config_bit(F0, true);
        config_bit(F1, true);
    }else{
        strcpy(me->ErrPtr, "Err: Invalid config fault mode\n");
    }
}

//T_HIGH and T_LOW
static void config_THIGH(double temp)
{
    int16_t data = temp * 16;
    if (me->em){
        data = data << 3;
    } else { 
        data = data << 4;
    }
    uint8_t buffer[3] = {THIGH_REG, (uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
    me->i2c_write(buffer, 3);
}

static void config_TLOW(double temp)
{
    int16_t data = temp * 16;
    if (me->em){
        data = data << 3;
    } else { 
        data = data << 4;
    }
    uint8_t buffer[3] = {TLOW_REG, (uint8_t)(data >> 8), (uint8_t)(data & 0xFF)};
    me->i2c_write(buffer, 3);
}

static uint16_t cal_temp(uint8_t byte[])
{
    uint16_t data;
    if (me->em){
        data = ((uint16_t)byte[0] << 5) | (byte[1] >> 3);
        if (data > 0xFFF) 
            data |= 0xF000;
    } else {
        data = ((uint16_t)byte[0] << 4) | (byte[1] >> 4);
        if (data > 0x7FF) 
            data |= 0xF000;
    }
    return data* 6.25 ;
}

//private 
void _i2c_point(uint8_t reg_addr)
{
    uint8_t data[1] ={reg_addr}; 
    me->i2c_write(data, 1);
}

void _i2cStart(void) {
    TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWEA);//|(1<<TWIE);    
}

void _i2cStop(void) {
    TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN)|(1<<TWEA);//|(1<<TWIE);    
}

void _i2cReply(void) {
    TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);//|(1<<TWIE);
}

void _i2cStatus(uint8_t expected, uint8_t error) 
{
    if ((TWSR & 0xF8) != expected)
        _error_check(error);
}

void _error_check(uint8_t error) 
{
    if ((TWSR & 0xF8) == error){
        strcpy(me->ErrPtr, "Error NO_ACK: TWSR : ");
    }else{
        strcpy(me->ErrPtr, "Err TWSR!=NO_ACK| TWSR: ");
    }
    // extract upper and lower nibbles from input value
    uint8_t upperNibble = (TWSR & 0xF0) >> 4;
    uint8_t lowerNibble = TWSR & 0x0F;
    // convert nibble to its ASCII hex equivalent
    upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
    lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';
    //put the error code in error pointer

    strcpy(me->ErrPtr+26, &upperNibble);
    strcpy(me->ErrPtr+27, &lowerNibble);
}

static void new(void* this, uint8_t address, bool extended_mode) {
    //OBJ INIT
    char* temp_ptr = malloc(sizeof(char)*MAX_SIZE_STR + 1);
    strcpy(temp_ptr, ""); // init error to empty string
    // me = malloc(sizeof(struct TMP102));
    me  = (struct TMP102*)this;
    me->em = extended_mode;
    me->local_ISR = false;   // default
    me->local_POL = false;  // default
    me->addr = address;
    me->ErrPtr = temp_ptr;
    me->i2c_init_baud_rate = &i2c_init_baud_rate;
    me->i2c_write = &i2c_write;
    me->i2c_read = &i2c_read;
    me->alert = &alert;
    me->config_bit = &config_bit;
    me->config_conv_rate = &config_conv_rate;
    me->config_faults = &config_faults;
    me->config_THIGH = &config_THIGH;
    me->config_TLOW = &config_TLOW;
    me->cal_temp = &cal_temp;

    //EXT Interupt Enable;
    EICRA = 0x05; //Any logical change on INT0 and INT1 generates an interrupt request.
    EIMSK = 0x03;
    sei();

    // return *me;
}
const struct TMP102Class TMP102={.new=&new};