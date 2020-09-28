#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

//UART
void UART_init(uint16_t ubrr);
void UART_putchar(unsigned char data);
uint16_t UART_getchar(void);
void UART_putStr(char *StringPtr);

void UART_puthex8(uint8_t val);
void UART_puthex16(uint16_t val);
void UART_puthex32(uint32_t val);

void UART_putU8(uint8_t val);
void UART_putS8(int8_t val);
void UART_putU16(uint16_t val);
void UART_putS16(int16_t val);