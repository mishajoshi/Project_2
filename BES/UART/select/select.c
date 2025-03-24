#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>

#define BAUDRATE B115200
#define MAX(a,b) (a>b? a: b)

void handle_input_from_source1(int fd){
  char d;
  int res;
  res = read(fd, &d, 1);
  if (res == 1)
    printf("Source 1: %c\n", d);
}
void handle_input_from_source2(int fd){
  char d;
  int res;
  res = read(fd, &d, 1);
  if (res == 1)
    printf("Source 2: %c\n", d);
}

int open_input_source(char * input_name) {
  int fd,c, res;
  struct termios oldtio,newtio;
  char buf[255];
  int read_req_num_bytes=5;
        
  fd = open(input_name, O_RDWR | O_NOCTTY ); 
  if (fd <0) {
    printf("Failed to open %s\n", input_name);
    return fd;
  }
        
  tcgetattr(fd,&oldtio); /* save current port settings */
        
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
        
  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  // read will block until it gets min(1, read_req_num_bytes) chars/bytes 
  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 1;  /* blocking read until 1 char received */
  
  tcflush(fd, TCIFLUSH);
  tcsetattr(fd,TCSANOW,&newtio);

  return fd;
}

int main(int argc, char * argv[] )
{
  int    fd1, fd2;  /* input sources 1 and 2 */
  fd_set readfds;    /* file descriptor set */
  int    maxfd;     /* maximum file desciptor used */
  int    loop=1;    /* loop while TRUE */ 
        
  /* open_input_source opens a device, sets the port correctly, and
     returns a file descriptor */
  fd1 = open_input_source("/dev/ttyAMA0");  
  if (fd1<0) exit(0);
  fd2 = open_input_source("/dev/ttyAMA1");  
  if (fd2<0) exit(0);
  maxfd = MAX(fd1, fd2)+1;  /* maximum bit entry (fd) to test */
        
  /* loop for input */
  while (loop) {
    FD_SET(fd1, &readfds);  /* set testing for source 1 */
    FD_SET(fd2, &readfds);  /* set testing for source 2 */
    /* block until input becomes available */
    select(maxfd, &readfds, NULL, NULL, NULL);
    if (FD_ISSET(fd1, &readfds))         /* input from source 1 available */
      handle_input_from_source1(fd1);
    if (FD_ISSET(fd2, &readfds))         /* input from source 2 available */
      handle_input_from_source2(fd2);
  }
}   
    
