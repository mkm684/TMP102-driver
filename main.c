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

int main() {
    uint8_t byte[2];

    UART_init(MYUBRR);
    portInit();
    struct TMP102 sensor = TMP102.new(0x48, false);

    //BR = (F_CPU) / (16 + 2*TWBR * 1) ~= 50kHz
    sensor.i2c_init_baud_rate(0x96, 0x0);
    sensor.config_faults(1);      // 1 fault
    sensor.config_bit(POL, true); // polarity 0
    sensor.config_bit(TM, false); // compartor mode
    sensor.config_conv_rate(1);   // 1hz
    sensor.config_bit(SD, false); // shutdown mode off

    sensor.i2c_read(CONFIG_REG, byte);
    UART_puthex16(conc(byte));
    UART_putchar('\n');

    sensor.config_THIGH(20.16);
    sensor.config_TLOW(0);

	while(1) {
        sensor.i2c_read(TEMP_REG, byte);
        UART_putS16(sensor.cal_temp(byte)/100);
        UART_putchar('.');
        UART_putS16(sensor.cal_temp(byte)%100);    
        UART_putchar('\n');       
        // UART_puthex8(sensor.alert()); 
        // UART_putchar('\n');       
        // UART_puthex8(PINB);
        // UART_putchar('\n');       
        // UART_puthex8(PIND);
        // UART_putchar('\n');   
        _delay_ms(500);
	}

    free(sensor.ErrPtr);
    free(&sensor);
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

ISR (INT0_vect)          //if pin 2 is used
{
    UART_putStr("INT0_vect");
}
ISR (INT1_vect)        //if pin 3 is used
{
    UART_putStr("INT1_vect");
}