/*
,* Copyright © 2013, Malcolm Sparks <malcolm@congreve.com>. All Rights Reserved.
,*
,* A program to convert USB firing events from the Dream Cheeky 'Big Red Button' to MQTT events.
,*/

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LID_CLOSED 21
#define BUTTON_PRESSED 22
#define LID_OPEN 23

int main(int argc, char **argv)
{
  int fd;
  int i, res, desc_size = 0;
  char buf[256];

  /* Use a udev rule to make this device */
  fd = open(argv[1], O_RDWR|O_NONBLOCK);

  if (fd < 0) {
    perror("Unable to open device");
    return 1;
  }

  int prior = LID_CLOSED;

  while (1) {
    memset(buf, 0x0, sizeof(buf));
    buf[0] = 0x08;
    buf[7] = 0x02;

    res = write(fd, buf, 8);
    if (res < 0) {
      perror("write");
      exit(1);
    }

    memset(buf, 0x0, sizeof(buf));
    res = read(fd, buf, 8);

    if (res >= 0) {
      if (prior == LID_CLOSED && buf[0] == LID_OPEN) {
         printf("Ready to fire!\n");
//         system("sudo initctl stop xbmc");
         fflush(stdout);
      } else if (prior != BUTTON_PRESSED && buf[0] == BUTTON_PRESSED) {
         printf("Fire!\n");
 //        system("shutdown now -h");
         fflush(stdout);
      } else if (prior != LID_CLOSED && buf[0] == LID_CLOSED) {
         printf("Stand down!\n");
         fflush(stdout);
      }
      prior = buf[0];
    }

    usleep(200000); /* Sleep for 200 ms*/
  }
}
