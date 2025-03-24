#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdint.h>   // for uint32_t - 32-bit unsigned integer
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>	// Defines signal-handling functions (i.e. trap Ctrl-C)

#include "BCM_GPIO.h"

#define INPUT_POS (27)
#define OUTPUT_POS (17)

static volatile GPIO_STRUCT * gpio;     // pointer to the gpio
volatile int run = 1;

// Signal handler callback function to gracefully exit (releasing GPIO mmap) when SIGINT (from Ctrl-C) is received
// void signal_handler(int sig);

void signal_handler(int signum)
{
  printf( "\nCtrl-C pressed, cleaning up and exiting.\n" );
  run = 0;
}

int main(int argc, char * argv []) {
  int fd;
  unsigned int level_in;

  printf("Start of GPIO memory-manipulation test program.\n");
  if(getuid()!=0) {
    printf("You must run this program as root. Exiting.\n");
    return -EPERM;
  }
  if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
    printf("Unable to open /dev/mem: %s\n", strerror(errno));
    return -EBUSY;
  }
  // get a pointer that points to the peripheral base for the GPIOs
  gpio = (GPIO_STRUCT *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE,
			   MAP_SHARED, fd, GPIO_BASE);

  if (((void *) gpio) == MAP_FAILED) {
    printf("Memory mapping failed: %s [%d, 0x%x]\n", strerror(errno), errno,
	   (int32_t)gpio);
    return -EBUSY;
  }

  // Set the signal callback for Ctrl-C
  signal(SIGINT, signal_handler);

  // at this point gpio points to the GPIO peripheral base address

  // Output: GPIO 17 (header pin 11)
  // Set up the LED GPIO FSEL17 mode = 001 at addr GPFSEL1 (0004)
  // remember that adding one 32-bit value moves the addr by 4 bytes
  // writing NOT 7 (i.e., ~111) clears bits 21, 22 and 23.
  MODIFY_FIELD(gpio->GPFSEL[1], 21, 3, 1);
    
  // Input: GPIO 27 (header pin 13)
  // set up the button GPIO FSEL27 mode = 000 at addr GPFSEL2 (0008)
  // both FSEL17 and FSEL27 are 21 bits in, but on different registers
  MODIFY_FIELD(gpio->GPFSEL[2], 21, 3, 0);

  while (run) {
    level_in = gpio->GPLEV[0] & MASK(INPUT_POS);
    if (level_in) {
      gpio->GPSET[0] = MASK(OUTPUT_POS); // Turn the LED on
    } else {
      gpio->GPCLR[0] = MASK(OUTPUT_POS); // Turn the LED off
    }
  } 
  close(fd);
  return 0;
}
