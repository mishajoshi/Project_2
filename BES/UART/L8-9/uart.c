#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>

int main(int argc, char *argv[]){
  int file, count;
  char tty_name[64];
  if(argc!=3){
    printf("Please pass a tty to use (e.g. ttyAMA1) and a string to send. Exiting!\n");
    return -2;
  }
  sprintf(tty_name, "/dev/%s", argv[1]);
  printf("Trying to open %s\n", tty_name);
  // use /dev/ttyS0 for the RPi3 and /dev/ttyAMA* for other models
  if ((file = open(tty_name, O_RDWR | O_NOCTTY | O_NDELAY))<0){
    perror("UART: Failed to open the device.\n");
    return -1;
  }
  struct termios options;
  tcgetattr(file, &options);
  options.c_cflag = B115200 | CS8 | CREAD | CLOCAL;
  options.c_iflag = IGNPAR | ICRNL;
  tcflush(file, TCIFLUSH);
  tcsetattr(file, TCSANOW, &options);
  while (1) {
    if ((count = write(file, argv[2], strlen(argv[2])))<0){
      perror("UART: Failed to write to the output\n");
      return -1;
    }
    write(file, "\n\r", 2);
    usleep(100000);
  }
  close(file);
  return 0;
}
