#!/bin/bash
#  Toggle a GPIO pin

COUNTER=0
while [ $COUNTER -lt 100000 ]; do
    gpioset gpiochip0 18=1
    sleep 0.015
    gpioset gpiochip0 18=0
    sleep 0.015
    let COUNTER=COUNTER+1
done
