/*                                                                  
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */

// build with -lpthread
// run with root/sudo 

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

extern int c_gpiod_flash();

void *thread_func(void *data)
{
  /* Do RT specific stuff here */
  printf("Thread function is starting.\n");
  c_gpiod_flash();
  return NULL;
}
 
int main(int argc, char* argv[])
{
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t thread;
  int ret;
  int rt=1;

  printf("%s: pass any argument to disable 'real-time' features.\n", argv[0]);
  if (argc > 1) {
    rt = 0;
    printf("'Real-time' disabled\n");
  } else {
    printf("'Real-time' enabled\n");
  }

  if (rt) {
    /* Lock memory */
    if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
      printf("mlockall failed: %m\n");
      exit(-2);
    }
  }
 
  /* Initialize pthread attributes (default values) */
  ret = pthread_attr_init(&attr);
  if (ret) {
    printf("init pthread attributes failed\n");
    goto out;
  }

  if (rt) {
    /* Set a specific stack size  */
    ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
    if (ret) {
      printf("pthread setstacksize failed\n");
      goto out;
    }  
    /* Set scheduler policy and priority of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
    if (ret) {
      printf("pthread setschedpolicy failed\n");
      goto out;
    }
    param.sched_priority = 80;
    ret = pthread_attr_setschedparam(&attr, &param);
    if (ret) {
      printf("pthread setschedparam failed\n");
      goto out;
    }
    /* Use scheduling parameters of attr */
    ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    if (ret) {
      printf("pthread setinheritsched failed\n");
      goto out;
    }
  }
    /* Create a pthread with specified attributes or defaults */
  ret = pthread_create(&thread, rt? &attr : NULL, thread_func, NULL);
  if (ret) {
    printf("create pthread failed\n");
    goto out;
  }
 
  /* Join the thread and wait until it is done */
  ret = pthread_join(thread, NULL);
  if (ret)
    printf("join pthread failed: %m\n");
 
 out:
  return ret;
}
