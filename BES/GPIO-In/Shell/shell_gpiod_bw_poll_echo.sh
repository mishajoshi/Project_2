#!/bin/bash
#  Short script to toggle a GPIO pin at the highest frequency
#  possible using Bash - by Derek Molloy

#  AGD - modified to echo input to output using libgpiod

COUNTER=0
while [ $COUNTER -lt 100000 ]; do
    gpioset gpiochip0 17=`gpioget gpiochip0 27`
    let COUNTER=COUNTER+1
done
