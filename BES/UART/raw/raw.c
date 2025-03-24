#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
        included by <termios.h> */
#define BAUDRATE B115200            
/* change this definition for the correct port */
#define MODEMDEVICE "/dev/ttyAMA0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

int main(int argc, char * argv[]) 
{
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];
  int read_req_num_bytes=5;
        
  fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
  if (fd <0) {perror(MODEMDEVICE); exit(-1); }
        
  tcgetattr(fd,&oldtio); /* save current port settings */
        
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
        
  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  // read will block until it gets min(10, read_req_num_bytes) chars/bytes 
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 10;  /* blocking read until 10 chars received */
  
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);
        
  while (STOP==FALSE) {       /* loop for input */
    res = read(fd,buf, read_req_num_bytes);   /* request this many characters */
    if (res > 0) {
      buf[res]=0;               /* so we can printf... */
      printf(":%s:%d\n", buf, res);
      if (buf[0]=='z')
	STOP=TRUE;
    } // else error
  }
  tcsetattr(fd,TCSANOW,&oldtio);
}
