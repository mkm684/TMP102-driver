#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

//UART
void UART_init( unsigned int ubrr)
{
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    // Enable receiver and transmitter 
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void UART_putchar(unsigned char data)
{
    // wait for transmit buffer to be empty
    while(!(UCSR0A & (1 << UDRE0)));

    // load data into transmit register
    UDR0 = data;
}

void UART_putStr(char *StringPtr)
{ 
    while(*StringPtr != 0x00)
    {  
        UART_putchar(*StringPtr);    
        StringPtr++;
    }        
}

void UART_puthex8(uint8_t val)
{
    // extract upper and lower nibbles from input value
    uint8_t upperNibble = (val & 0xF0) >> 4;
    uint8_t lowerNibble = val & 0x0F;

    // convert nibble to its ASCII hex equivalent
    upperNibble += upperNibble > 9 ? 'A' - 10 : '0';
    lowerNibble += lowerNibble > 9 ? 'A' - 10 : '0';

    // print the characters
    UART_putchar(upperNibble);
    UART_putchar(lowerNibble);
}

void UART_puthex16(uint16_t val)
{
    // transmit upper 8 bits
    UART_puthex8((uint8_t)(val >> 8));

    // transmit lower 8 bits
    UART_puthex8((uint8_t)(val & 0x00FF));
}

void UART_puthex32(uint32_t val)
{
    // transmit upper 16 bits
    UART_puthex16((uint16_t)(val >> 16));

    // transmit lower 16 bits
    UART_puthex16((uint16_t)(val & 0xFFFF));
}

uint16_t UART_getchar(void)
{
    // wait for data
    while(!(UCSR0A & (1 << RXC0)));

    // return data
    return UDR0;
}

void UART_putU8(uint8_t val)
{
    uint8_t dig1 = '0', dig2 = '0';

    // count value in 100s place
    while(val >= 100)
    {
        val -= 100;
        dig1++;
    }

    // count value in 10s place
    while(val >= 10)
    {
        val -= 10;
        dig2++;
    }

    // print first digit (or ignore leading zeros)
    if(dig1 != '0') UART_putchar(dig1);

    // print second digit (or ignore leading zeros)
    if((dig1 != '0') || (dig2 != '0')) UART_putchar(dig2);

    // print final digit
    UART_putchar(val + '0');
}

void UART_putS8(int8_t val)
{
    // check for negative number
    if(val & 0x80)
    {
        // print negative sign
        UART_putchar('-');

        // get unsigned magnitude
        val = ~(val - 1);
    }

    // print magnitude
    UART_putU8((uint8_t)val);
}

void UART_putU16(uint16_t val)
{
    uint8_t dig1 = '0', dig2 = '0', dig3 = '0', dig4 = '0';

    // count value in 10000s place
    while(val >= 10000)
    {
        val -= 10000;
        dig1++;
    }

    // count value in 1000s place
    while(val >= 1000)
    {
        val -= 1000;
        dig2++;
    }

    // count value in 100s place
    while(val >= 100)
    {
        val -= 100;
        dig3++;
    }

    // count value in 10s place
    while(val >= 10)
    {
        val -= 10;
        dig4++;
    }

    // was previous value printed?
    uint8_t prevPrinted = 0;

    // print first digit (or ignore leading zeros)
    if(dig1 != '0') {UART_putchar(dig1); prevPrinted = 1;}

    // print second digit (or ignore leading zeros)
    if(prevPrinted || (dig2 != '0')) {UART_putchar(dig2); prevPrinted = 1;}

    // print third digit (or ignore leading zeros)
    if(prevPrinted || (dig3 != '0')) {UART_putchar(dig3); prevPrinted = 1;}

    // print third digit (or ignore leading zeros)
    if(prevPrinted || (dig4 != '0')) {UART_putchar(dig4); prevPrinted = 1;}

    // print final digit
    UART_putchar(val + '0');
}

void UART_putS16(int16_t val)
{
    // check for negative number
    if(val & 0x8000)
    {
        // print minus sign
        UART_putchar('-');

        // convert to unsigned magnitude
        val = ~(val - 1);
    }

    // print unsigned magnitude
    UART_putU16((uint16_t) val);
}