#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/times.h>
#include <unistd.h>

static int seed;
static int layers;

static int
pick (int from, int to)
{
  assert (from <= to);
  return (rand() % (to - from + 1)) + from;
}

int
main (int argc, char ** argv)
{
  seed = (argc > 1) ? atoi (argv[1]) : abs ((times(0) * getpid ()) >> 1);
  printf ("c seed %d\n", seed);
  srand (seed);
  layers = pick (1, 10);
  printf ("c layers %d\n", layers);
  return 0;
}
