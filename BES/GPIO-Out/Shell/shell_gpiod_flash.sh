#!/bin/bash
#  Short script to toggle a GPIO pin with gpiod at the highest frequency
#  possible using Bash - by Derek Molloy, mod. by Alex Dean
COUNTER=0
while [ $COUNTER -lt 100000 ]; do
    gpioset gpiochip0 17=1
    let COUNTER=COUNTER+1
    gpioset gpiochip0 17=0
done
