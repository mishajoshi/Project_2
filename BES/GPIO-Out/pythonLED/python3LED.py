#!/usr/bin/python3
# A small Python program to set up GPIO4 as an LED that can be
# turned on or off from the Linux console. 
# Written by Derek Molloy for the book "Exploring Raspberry Pi"
# setup to export GPIO
# close to unexport GPIO
# modified by AGD 1/2023

import sys
from time import sleep
LED_PATH = "/sys/class/gpio/gpio17/"
SYSFS_DIR = "/sys/class/gpio/"
LED_NUMBER = "17"

def writeLED ( filename, value, path=LED_PATH ):
   "This function writes the value passed to the file in the path"
   fo = open( path + filename,"w")  
   fo.write(value)
   fo.close()
   return

def fasttoggleLED ( filename, numtoggles=10000, path=LED_PATH ):
   "This function writes 1s and 0s to the file in the path"
   fo = open( path + filename,"w")  
   for i in range(numtoggles):
      fo.write("1")
      fo.flush()
      fo.write("0")
      fo.flush()
   fo.close()
   return


print("Starting the GPIO LED Python script")
if len(sys.argv)!=2:
   print("There is an incorrect number of arguments")
   print("  usage is:  pythonLED.py command")
   print("  where command is one of setup, on, off, status, or close")
   sys.exit(2)
if sys.argv[1]=="on":
   print("Turning the LED on")
   writeLED (filename="value", value="1")
elif sys.argv[1]=="off":
   print("Turning the LED off")
   writeLED (filename="value", value="0")
elif sys.argv[1]=="setup":
   print("Setting up the LED GPIO")
   writeLED (filename="export", value=LED_NUMBER, path=SYSFS_DIR)
   sleep(0.1);
   writeLED (filename="direction", value="out")
elif sys.argv[1]=="close":
   print("Closing down the LED GPIO")
   writeLED (filename="unexport", value=LED_NUMBER, path=SYSFS_DIR)
elif sys.argv[1]=="status":
   print("Getting the LED state value")
   fo = open( LED4_PATH + "value", "r")
   print(fo.read())
   fo.close()
elif sys.argv[1]=="toggle":
   print("Toggling LED repeatedly")
   for i in range(100000):
      writeLED (filename="value", value="1")
      writeLED (filename="value", value="0")
elif sys.argv[1]=="fast-toggle":
   print("Fast-Toggling LED repeatedly")
   fasttoggleLED (filename="value", numtoggles=500000)
else:
   print("Invalid Command!")
print("End of Python script")
