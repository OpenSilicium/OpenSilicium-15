#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include "brb.h"

main (int ac, char **av)
{
  int x, fd;

  fd = open (av[1], O_RDWR);

  if (fd < 0) {
    perror ("open");
    exit (1);
  }

  while (1) {
    ioctl (fd, BRB_STATUS, &x);

    switch (x) {
    case 0x15 : 
      printf ("Closed\n");
      break;

    case 0x17 : 
      printf ("Opened\n");
      break;

    case 0x16 : 
      printf ("FIRE !!!\n");
      break;

    default:
      printf ("Onknown status !\n");
      break;
    }

    usleep (200000);
  }

  close (fd);
}
