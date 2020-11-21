#include <time.h>
#include <stdlib.h>

struct drand48_data buf;
long int a, b;

/* Initialize the hardware mrand48_r implementation.  */
void
hardware_mrand48_r_init (void)
{
    long int seed = time(NULL);
    srand48_r(seed, &buf);
}

/* Return a random value, using hardware operations.  */
unsigned long long
hardware_mrand48_r (void)
{
    mrand48_r(&buf, &a);
    mrand48_r(&buf, &b);
    return ((((unsigned long long) a) << 32) | ((unsigned long long) b & 0x00000000FFFFFFFF));
}

/* Finalize the hardware mrand48_r implementation.  */
void
hardware_mrand48_r_fini (void)
{
}