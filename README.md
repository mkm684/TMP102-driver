# TMP102-driver

This is a driver for The TMP102. it's digital temperature sensor from Texas Instruments.
The TMP102 uses the I2C bus of the microcontroller to communicate the temperature.

The driver is developed for AVR from ground up. 
This is availbale to be used by anyone without any pre consultantions. 

The src folder contains a UART driver for debugging purposes.
the UART driver code is sourced from http://www.rjhcoding.com/avrc-uart.php

Use the make.sh to compile and upload the library.
example command: make.sh COM19

main.c is an example use of the driver.
