/* A Simple GPIO application
*  Written by Derek Molloy for the book "Exploring Raspberry Pi" */

#define KEEP_STREAM_OPEN

#include<iostream>
#include<unistd.h>  // for the usleep() function
#include"GPIO.h"
using namespace exploringRPi;
using namespace std;

int main(){
   GPIO outGPIO(17), inGPIO(27);    // pin 11 and pin 13

   outGPIO.setDirection(OUTPUT);    // basic output example
   inGPIO.setDirection(INPUT);      // basic input example

#ifndef KEEP_STREAM_OPEN
   while (1) {
     outGPIO.setValue(inGPIO.getValue());
   }
#else 
   outGPIO.streamOpen();            // fast write example
   while (1) {
     outGPIO.streamWrite(inGPIO.getValue());
   }
   outGPIO.streamClose();           // close the stream
#endif
   
   return 0;
}
