#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

bool
writebytes (char* x, unsigned int bsize)
{
  // invoke write system call
  int testWrite = write(1, x, bsize);

  // if write == 0 or negative, unable to write
  if (testWrite <= 0) {

    printf ("%d", testWrite);
    fprintf (stderr, "Unable to write bytes. \n");
    exit(1);

  }
  else {
    return true;
  }

}
