avr-gcc -Os -DF_CPU=8000000 -mmcu=atmega328p -c src/main.c src/uart.c src/i2c_twi.c
avr-gcc -DF_CPU=8000000 -mmcu=atmega328p -o bin/main.elf bin/main.o bin/uart.o bin/i2c_twi.o
avr-objcopy -O ihex bin/main.elf bin/main.hex
avrdude -c arduino -p atmega328p -P COM19 -U flash:w:bin/main.hex