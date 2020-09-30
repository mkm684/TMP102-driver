# TMP102-driver

This is a driver for The TMP102. it's digital temperature sensor from Texas Instruments.
The TMP102 uses the I2C bus of the microcontroller to communicate the temperature.
https://www.sparkfun.com/products/13314

The driver is developed for AVR from ground up. 
This is availbale to be used by anyone without any pre consultantions. 

The src folder contains a UART driver for debugging purposes.
the UART driver code is sourced from http://www.rjhcoding.com/avrc-uart.php

Use the make.sh to compile and upload your code.
example command: make.sh COM19

main.c is an example use of the driver.

Disclaimer: This library/driver is only developed for learning purposes. 
            Original driver is availbale at https://github.com/sparkfun/SparkFun_TMP102_Arduino_Library. 
            However, the library is mature and bug free to be used by any TMP102 user. 
            In case of any bugs found. I would appreciate reporting them on the Github repo, to be fixed.
