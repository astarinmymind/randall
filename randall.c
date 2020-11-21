/* Generate N bytes of random output.  */

/* When generating output this program uses the x86-64 RDRAND
   instruction if available to generate random numbers, falling back
   on /dev/random and stdio otherwise.

   This program is not portable.  Compile it with gcc -mrdrnd for a
   x86-64 machine.

   Copyright 2015, 2017, 2020 Paul Eggert

   This program is free software: you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./output.h"
#include "./rand64-sw.h"
#include "./rand64-hw.h"
#include "./mrand48.h"
#include "./options.h"

/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  struct opts opts;
  read_options(argc, argv, &opts);

  /* If there's no work to do, don't worry about which library to use.  */
  if (opts.nbytes == 0)
    return 0;
  
  // invalid options
  if (opts.valid == false) {
    fprintf (stderr, "Invalid invocation. \n");
    exit(1);
  }
  
  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */
  void (*initialize) (void);
  unsigned long long (*rand64) (void);
  void (*finalize) (void);

  // rdrand
  if (rdrand_supported () && (opts.input == DEFAULT || opts.input == RDRAND))
    {
      initialize = hardware_rand64_init;
      rand64 = hardware_rand64;
      finalize = hardware_rand64_fini;
    }
  // user called rdrand but it is not available
  else if ( !(rdrand_supported ()) && opts.input == RDRAND)
    {
      fprintf (stderr, "Sorry, hardware does not support rdrand \n");
      exit(1);
    }
  // mrand48_r
  else if (opts.input == MRAND48_R)
    {
      initialize = hardware_mrand48_r_init;
      rand64 = hardware_mrand48_r;
      finalize = hardware_mrand48_r_fini;
    }
  // input file
  else if (opts.input == SLASH_F)
    {
      // set file variable
      size_t file_length = strlen(opts.r_src);
      rsrc = malloc(file_length * sizeof(char));
      if (rsrc == NULL) {
        fprintf (stderr, "Unable to allocate \n");
        exit(1);
      }
      strcpy(rsrc, opts.r_src);

      initialize = software_rand64_init;
      rand64 = software_rand64;
      finalize = software_rand64_fini;
    }
  else {
    fprintf (stderr, "Invalid option. \n");
    exit(1);
  }

  initialize ();

  // output N bytes at a time -- optimizes both memory and i/o
  // block size = N * 1024
  unsigned int block = opts.block_size;
  // allocate dynamic memory 
  char *obuffer = malloc(block * sizeof(char));
  // how many bytes 
  int number = block / 8;
  // number of bytes
  unsigned int totalbytes = opts.nbytes;
  // for loop: print one block at a time until totalbyte is 0
  do {
    // if totalbytes < block: outbytes = totalbytes
    int outbytes = totalbytes < block ? totalbytes : block;
    // for loop: generate and print random numbers until blocksize is 0
      for(int j=0; j < number; j++) {
        unsigned long long x = rand64 ();
        memcpy(&obuffer[j * sizeof(x)], &x, sizeof(x));
      }
      totalbytes -= outbytes;
      writebytes(obuffer, outbytes);
  }
  while(0 < totalbytes);
  
  // free memory
  free(obuffer);

  finalize ();
}
