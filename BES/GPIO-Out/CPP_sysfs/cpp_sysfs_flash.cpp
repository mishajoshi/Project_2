/* A Simple GPIO application
*  Written by Derek Molloy for the book "Exploring Raspberry Pi" */

#include<iostream>
#include<unistd.h>  // for the usleep() function
#include"GPIO.h"
using namespace exploringRPi;
using namespace std;

int main(){
   GPIO outGPIO(17);    // pin 11
   outGPIO.setDirection(OUTPUT);    // basic output example
   for (int i=0; i<10000000; i++){        // flash the LED many times
      outGPIO.setValue(HIGH);       // turn the LED on
      outGPIO.setValue(LOW);        // turn the LED off
   }

   return 0;
}
