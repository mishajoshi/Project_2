#include<iostream>
#include"GPIO.h"
using namespace exploringRPi;
using namespace std;

int main(){
  GPIO outGPIO(17), inGPIO(27);

  inGPIO.setDirection(INPUT);    //button is an input
  outGPIO.setDirection(OUTPUT);  //LED is an output
  inGPIO.setEdgeType(BOTH);    //wait for either edge
  outGPIO.streamOpen();          //fast write, ready file
  outGPIO.streamWrite(LOW);      //turn the LED off
  while (1) {
    inGPIO.waitForEdge();          //will wait forever
    outGPIO.streamWrite(inGPIO.getValue()); //write new value
  }
  outGPIO.streamClose();         //close the output stream
  return 0;
}
