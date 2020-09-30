#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "uart.h"
#include "i2c_twi.h"

#define BAUD 9600
#define MYUBRR  (F_CPU / 4 / BAUD - 1) / 2

//Helper functions
int16_t conc(uint8_t byte[]);
void portInit();

struct TMP102 sensor;

int main() {
    uint8_t byte[2];

    UART_init(MYUBRR);
    portInit();
    TMP102.new(&sensor, 0x48, false);

    // BR = (F_CPU) / (16 + 2*TWBR * 1) ~= 50kHz
    sensor.i2c_init_baud_rate(0x96, 0x0);
    sensor.config_faults(1);      // 1 fault
    sensor.config_bit(POL, true); // polarity 1
    sensor.config_bit(TM, false); // compartor mode
    sensor.config_conv_rate(1);   // 1hz
    sensor.config_bit(SD, false); // shutdown mode off

    // read and print the config reg
    sensor.i2c_read(CONFIG_REG, byte);
    UART_puthex16(conc(byte));
    UART_putchar('\n');

    sensor.config_THIGH(24.16);
    sensor.config_TLOW(23.8);   

	while(1) 
    {
        sensor.i2c_read(TEMP_REG, byte);
        UART_putStr("Temprature: ");
        UART_putS16(sensor.cal_temp(byte)/100);
        UART_putchar('.');
        UART_putS16(sensor.cal_temp(byte)%100);
        UART_putStr(" | Alert: ");    
        UART_puthex8(sensor.alert()); 
        UART_putStr(" | Status: ");    
        UART_putStr(sensor.ErrPtr);   
        UART_putchar('\n');    
        _delay_ms(500);
	}

    // garbage collection to be improved
    free(sensor.ErrPtr);
    return 0;
}

void portInit()
{
    DDRB = 0x00;
    DDRD = 0x00;
    PORTD = 0xFF;
    PORTB = 0xFF;
}

int16_t conc(uint8_t byte[])
{
    return ((int16_t)byte[0] << 8) | (byte[1]);
}

//Example of TMP102 ISR handler
ISR (INT0_vect)          //if pin D2 is used
{
    UART_putStr("INT0_vect");
    if (sensor.local_ISR) {
        UART_putStr("interrupt mode");
    } else {
        UART_putStr("compartor mode");
    }
    if (sensor.local_POL) {
        if (PIND2) {
            UART_putStr("Temprature went above THIGH_REG");
        } else {
            UART_putStr("Temprature went below TLOW_REG");
        }
    } else {
        if (PIND2){
            UART_putStr("Temprature went below TLOW_REG");
        } else {
            UART_putStr("Temprature went above THIGH_REG");
        }
    }
}

ISR (INT1_vect)        //if pin D3 is used
{
    UART_putStr("INT1_vect");
}