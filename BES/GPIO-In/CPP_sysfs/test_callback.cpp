#include<iostream>
#include<unistd.h>
#include"GPIO.h"
using namespace exploringRPi;
using namespace std;

GPIO *outGPIO, *inGPIO;           // global pointers

int activateLED(int var){
#if 0 // toggle the LED
  static GPIO_VALUE led_on = LOW;
  led_on = (GPIO_VALUE) !led_on;
  outGPIO->streamWrite(led_on);    // toggle the LED
#else // flash the LED
  outGPIO->streamWrite((GPIO_VALUE) 1);
  outGPIO->streamWrite((GPIO_VALUE) 0);
#endif  
  cout << "Button Pressed" << endl;
  return 0;
}

int main(){
   inGPIO = new GPIO(27);         // the button
   outGPIO = new GPIO(17);        // the LED
   inGPIO->setDirection(INPUT);   // the button is an input
   outGPIO->setDirection(OUTPUT); // the LED is an output
   outGPIO->streamOpen();         // use fast write to LED
   outGPIO->streamWrite(LOW);     // turn the LED off
   inGPIO->setEdgeType(RISING);   // wait for rising edge
   cout << "You have 100 seconds to press the button:" << endl;
   inGPIO->waitForEdge(&activateLED); // pass the callback function
   cout << "Listening, but also doing something else..." << endl;
   for(int i=0; i<100; i++){
      usleep(1000000);            // sleep for 1 second
      cout << "[sec]" << flush;   // display a message
   }
   outGPIO->streamWrite(LOW);     // turn off the LED after 10 seconds
   outGPIO->streamClose();        // shutdown the stream
   return 0;
}
