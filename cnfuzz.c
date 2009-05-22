#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>

#define MAX 20
static int seed, nlayers, ** layers, *width, * low, * high, * clauses;
static int clause[MAX + 1];
static char * mark;

static int
pick (int from, int to)
{
  assert (from <= to);
  return (rand() % (to - from + 1)) + from;
}

int
main (int argc, char ** argv)
{
  int i, j, k, l, m, n, sign, lit, layer;
  seed = (argc > 1) ? atoi (argv[1]) : abs ((times(0) * getpid ()) >> 1);
  printf ("c seed %d\n", seed);
  srand (seed);
  nlayers = pick (1, 20);
  printf ("c layers %d\n", nlayers);
  layers = calloc (nlayers, sizeof *layers);
  width = calloc (nlayers, sizeof *width);
  low = calloc (nlayers, sizeof *low);
  high = calloc (nlayers, sizeof *high);
  clauses = calloc (nlayers, sizeof *clauses);
  for (i = 0; i < nlayers; i++)
    {
      width[i] = pick (10, 70);
      low[i] = i ? high[i-1] + 1 : 1;
      high[i] = low[i] + width[i] - 1;
      m = width[i];
      if (i) m += width[i-1];
      n = (pick (350, 500) * m) / 100;
      clauses[i] = n;
      printf ("c layer[%d] = [%d..%d] w=%d v=%d c=%d r=%.2f\n",
              i, low[i], high[i], width[i], m, n, n / (double) m);
    }
  n = 0;
  m = high[nlayers-1];
  mark = malloc (m + 1);
  memset (mark, 0, m);
  for (i = 0; i < nlayers; i++)
    n += clauses[i];
  printf ("p cnf %d %d\n", m, n);
  for (i = 0; i < nlayers; i++)
    {
      for (j = 0; j < clauses[i]; j++)
	{
	  l = 3;
	  while (l < MAX && pick (17, 19) != 17)
	    l++;

	  for (k = 0; k < l; k++)
	    {
	      layer = pick ((i ? i-1 : i), i);
	      sign = (pick (31, 32) == 31) ? 1 : -1;
	      lit = pick (low[layer], high[layer]);
	      if (mark[lit]) continue;
	      clause[k] = lit;
	      mark[lit] = 1;
	      lit *= sign;
	      printf ("%d ", lit);
	    }
	  printf ("0\n");
	  for (k = 0; k < l; k++)
	    mark[clause[k]] = 0;
	}
    }
  free (mark);
  free (clauses);
  free (high);
  free (low);
  free (width);
  for (i = 0; i < nlayers; i++)
    free (layers[i]);
  free (layers);
  return 0;
}
