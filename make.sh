avr-gcc -Os -DF_CPU=8000000 -mmcu=atmega328p -c main.c uart.c i2c_twi.c
avr-gcc -DF_CPU=8000000 -mmcu=atmega328p -o main.elf main.o uart.o i2c_twi.o
avr-objcopy -O ihex main.elf main.hex
avrdude -c arduino -p atmega328p -P COM19 -U flash:w:main.hex