/*                                                                  
 * POSIX Real Time Example
 * using a single pthread as RT thread
 */

/* Arguments
   -l      Lock memory
   -p 1234 Set task period to 1234 us
   -f      RT_SCHED_FIFO request
*/

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define DEFAULT_TASK_PERIOD_US  100 
#define MIN_TASK_PERIOD_US       40 // Safe on Pi 3, but conservative    

int task_period_us = DEFAULT_TASK_PERIOD_US;

struct period_info {
  struct timespec next_period;
  long period_ns;
};

extern int c_gpiod_init();
extern void c_gpiod_flash();
extern void c_gpiod_close();

static void inc_period(struct period_info *pinfo) {
  pinfo->next_period.tv_nsec += pinfo->period_ns;
 
  while (pinfo->next_period.tv_nsec >= 1000000000) {
    /* timespec nsec overflow */
    pinfo->next_period.tv_sec++;
    pinfo->next_period.tv_nsec -= 1000000000;
  }
}
 
static void periodic_task_init(struct period_info *pinfo) {
  /* for simplicity, hardcoding a period */
  pinfo->period_ns = task_period_us*1000;
  clock_gettime(CLOCK_MONOTONIC, &(pinfo->next_period));
  c_gpiod_init();
}
 
static void do_rt_task() {
  /* Do RT stuff here. */
  c_gpiod_flash();
}
 
static void wait_rest_of_period(struct period_info *pinfo) {
  inc_period(pinfo);
 
  /* for simplicity, ignoring possibilities of signal wakes */
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &pinfo->next_period, NULL);
}

void *simple_cyclic_task(void *data) {
  struct period_info pinfo;
 
  periodic_task_init(&pinfo); 
  while (1) {
    do_rt_task();
    wait_rest_of_period(&pinfo);
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t thread;
  int ret;
  int lock_mem = 0, rt_sched_fifo = 0;

  for (int i=1; i<argc; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
      case 'l':
	lock_mem = 1;
	break;
      case 'p': // Task period in us
	if (argc > i+1) {
	  if (1 != sscanf(argv[i+1],"%d", &task_period_us)) {
	    printf("Period argument %s ignored.\n", argv[i+1]);
	  } else {
	    if (task_period_us < MIN_TASK_PERIOD_US)
	      task_period_us = MIN_TASK_PERIOD_US;
	  }
	}
	i++; // skip over period argument
	break;
      case 'f':
	rt_sched_fifo = 1;
	break;
      default:
	printf("Argument %s ignored.\n", argv[i]);
	break;
      }
    }
  }
  printf("\n");
  if (rt_sched_fifo)
    printf("rt_sched_fifo requested. ");
  if (lock_mem)
    printf("Memory locking requested. ");
  printf("Task period %d us requested.\n", task_period_us);
   
  if (lock_mem) {
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
 
  /* Set a specific stack size  */
  ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
  if (ret) {
    printf("pthread setstacksize failed\n");
    goto out;
  }

  if (rt_sched_fifo) {
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
  }  else {
    /* Set scheduler policy of pthread */
    ret = pthread_attr_setschedpolicy(&attr, SCHED_OTHER); // SCHED_BATCH, SCHED_IDLE
    if (ret) {
      printf("pthread setschedpolicy failed\n");
      goto out;
    }
  }

  /* Use scheduling parameters of attr */
  ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
  if (ret) {
    printf("pthread setinheritsched failed\n");
    goto out;
  }
 
  /* Create a pthread with specified attributes */
  ret = pthread_create(&thread, &attr, simple_cyclic_task, NULL);
  if (ret) {
    printf("create pthread failed\n");
    goto out;
  }
 
  /* Join the thread and wait until it is done */
  ret = pthread_join(thread, NULL);
  if (ret)
    printf("join pthread failed: %m\n");
 
 out:
  c_gpiod_close();
  return ret;
}
