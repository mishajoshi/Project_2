/* A Simple GPIO application
*  Written by Derek Molloy for the book "Exploring Raspberry Pi" */

#include<iostream>
#include<unistd.h>  // for the usleep() function
#include"GPIO.h"
using namespace exploringRPi;
using namespace std;

int main(){
   GPIO outGPIO(17), inGPIO(27);    // pin 11 and pin 13

   outGPIO.setDirection(OUTPUT);    // basic output example
   inGPIO.setDirection(INPUT);      // basic input example

   outGPIO.streamOpen();            // fast write example
   while (1) {
     outGPIO.streamWrite(inGPIO.getValue());
   }
   outGPIO.streamClose();           // close the stream
   
   return 0;
}
