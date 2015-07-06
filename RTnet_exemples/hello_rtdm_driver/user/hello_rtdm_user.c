#include <sys/mman.h>
#include <sys/time.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>

#define STR    "Hello World"

pthread_t thid_square;
int fd;

/* Thread function*/
void *thread_square (void *dummy)
{
  int err, cmd;
  struct timespec start, period, tr;

  /* Start a periodic task in 1s */
  clock_gettime(CLOCK_REALTIME, &start);
  start.tv_sec += 1;
  period.tv_sec = 1;
  period.tv_nsec = 0;
    
  // Make task periodic
  err = pthread_make_periodic_np(pthread_self(), &start, &period);

  if (err)
    {
      fprintf(stderr,"square: failed to set periodic, code %d\n",err);
      exit(EXIT_FAILURE);
    }

  /* Main loop */
  for (;;)
    {
      unsigned long overruns = 0;
	  
      /* Wait scheduler */
      err = pthread_wait_np(&overruns);

      if (err || overruns) {
	fprintf(stderr,"wait_period failed: err %d, overruns: %lu\n", err, overruns);
	exit(EXIT_FAILURE);
      }

      if (rt_dev_write (fd, STR, strlen(STR)) < 0) {
	perror ("rt_dev_write");
	exit (1);
      }
    }
}

void cleanup_upon_sig(int sig __attribute__((unused)))
{
  pthread_cancel (thid_square);
  pthread_join (thid_square, NULL);
  rt_dev_close (fd);
  exit(0);
}

int main (int ac, char **av)
{
  int err;
  char *cp, *progname = (char*)basename(av[0]), *rtdm_driver;
  struct sched_param param_square = {.sched_priority = 99 };
  pthread_attr_t thattr_square;

  signal(SIGINT, cleanup_upon_sig);
  signal(SIGTERM, cleanup_upon_sig);
  signal(SIGHUP, cleanup_upon_sig);
  signal(SIGALRM, cleanup_upon_sig);

  // Avoid paging: MANDATORY for RT !!
  mlockall(MCL_CURRENT|MCL_FUTURE);

  // Open RTDM driver
  if ((fd = rt_dev_open (av[1], 0)) < 0) {
    fprintf (stderr, "Can't open %s\n", av[1]);
    exit(EXIT_FAILURE);
  }

  // Thread attributes
  pthread_attr_init(&thattr_square);

  // Joinable 
  pthread_attr_setdetachstate(&thattr_square, PTHREAD_CREATE_JOINABLE);

  // Priority, set priority to 99
  pthread_attr_setinheritsched(&thattr_square, PTHREAD_EXPLICIT_SCHED);
  pthread_attr_setschedpolicy(&thattr_square, SCHED_FIFO);
  pthread_attr_setschedparam(&thattr_square, &param_square);

  // Create thread 
  err = pthread_create(&thid_square, &thattr_square, &thread_square, NULL);

  if (err)
    {
      fprintf(stderr,"square: failed to create square thread, code %d\n",err);
      return 0;
    }

  pause();

  return 0;
}
