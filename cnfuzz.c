/* Copyright (c) 2009, Armin Biere, Johannes Kepler University. */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/times.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX 20
static int clause[MAX + 1];

static int
pick (int from, int to)
{
  assert (from <= to);
  return (rand() % (to - from + 1)) + from;
}

static int
numstr (const char * str)
{
  const char * p;
  for (p = str; *p; p++)
    if (!isdigit (*p))
      return 0;
  return 1;
}

int
main (int argc, char ** argv)
{
  int i, j, k, l, m, n, o, p, sign, lit, layer, w, val, min, max, ospread;
  int ** unused, * nunused, allmin, allmax, qbf, *quant, scramble, * map;
  int seed, nlayers, ** layers, *width, * low, * high, * clauses, fp; 
  const char * options;
  char option[100];
  FILE * file;
  char * mark;

  qbf = 0;
  seed = -1;
  options = 0;

  for (i = 1; i < argc; i++) 
    {
      if (!strcmp (argv[i], "-h")) 
	{
	  printf (
"usage: cnfuzz [-h][-q][<seed>][<option-file>]\n"
"\n"
"  -h   print command line option help\n"
"  -q   generate quantified CNF in QDIMACS format\n"
"\n"
"If the seed is not specified it is calculated from the process id\n"
"and the current system time (in seconds).\n"
"\n"
"The optional <option-file> lists integer options with their ranges,\n"
"one option in the format '<opt> <lower> <upper> per line.\n"
"Those options are fuzzed and embedded into the generated input\n"
"in comments before the 'p cnf ...' header.\n"
);
	  exit (0);
	}
      if (!strcmp (argv[i], "-q")) 
	qbf = 1;
      else if (numstr (argv[i])) 
	{
	  if (seed >= 0) 
	    {
	      fprintf (stderr, "*** cnfuzz: multiple seeds\n");
	      exit (1);
	    }
	  seed = atoi (argv[i]);
	  if (seed < 0) 
	    {
	      fprintf (stderr, "*** cnfuzz: seed overflow\n");
	      exit (1);
	    }
	}
      else if (options) 
	{
	  fprintf (stderr, "*** cnfuzz: multiple option files\n");
	  exit (1);
	}
      else
	options = argv[i];
    }

  if (seed < 0) seed = abs ((times(0) * getpid ()) >> 1);

  srand (seed);
  printf ("c seed %d\n", seed);
  if (qbf) 
    {
      printf ("c qbf\n");
      fp = pick (0, 3);
      if (fp)
	printf ("c but forced to be propositional\n");
    }
  if (options)
    {
      file = fopen (options, "r");
      ospread = pick (0, 10);
      if ((allmin = pick (0, 1)))
	printf ("c allmin\n");
      else if ((allmax = pick (0, 1)))
	printf ("c allmax\n");
      printf ("c %d ospread\n", ospread);
      if (!file)
	{
	  fprintf (stderr, "*** cnfuzz: can not read '%s'\n", options);
	  exit (1);
	}
      while (fscanf (file, "%s %d %d %d", option, &val, &min, &max) == 4)
	{
	  if (!pick (0, ospread)) 
	    {
	      if (allmin) val = min;
	      else if (allmax) val = max;
	      else val = pick (min, max);
	    }
	  printf ("c --%s=%d\n", option, val);
	}
      fclose (file);
    }
  srand (seed);
  w = pick (10, 70);
  printf ("c width %d\n", w);
  scramble = pick (-1, 1);
  printf ("c scramble %d\n", scramble);
  nlayers = pick (1, 20);
  printf ("c layers %d\n", nlayers);
  layers = calloc (nlayers, sizeof *layers);
  quant = calloc (nlayers, sizeof *quant);
  width = calloc (nlayers, sizeof *width);
  low = calloc (nlayers, sizeof *low);
  high = calloc (nlayers, sizeof *high);
  clauses = calloc (nlayers, sizeof *clauses);
  unused = calloc (nlayers, sizeof *unused);
  nunused = calloc (nlayers, sizeof *nunused);
  for (i = 0; i < nlayers; i++)
    {
      width[i] = pick (10, w);
      quant[i] = (qbf && !fp) ? pick (-1, 1) : 0;
      low[i] = i ? high[i-1] + 1 : 1;
      high[i] = low[i] + width[i] - 1;
      m = width[i];
      if (i) m += width[i-1];
      n = (pick (300, 450) * m) / 100;
      clauses[i] = n;
      printf ("c layer[%d] = [%d..%d] w=%d v=%d c=%d r=%.2f q=%d\n",
              i, low[i], high[i], width[i], m, n, n / (double) m, quant[i]);

      nunused[i] = 2 * (high[i] - low[i] + 1);
      unused[i] = calloc (nunused[i], sizeof *unused[i]);
      k = 0;
      for (j = low[i]; j <= high[i]; j++)
	for (sign = -1; sign <= 1; sign += 2)
	  unused[i][k++] = sign * j;
      assert (k == nunused[i]);
    }
  n = 0;
  m = high[nlayers-1];
  mark = calloc (m + 1, 1);
  for (i = 0; i < nlayers; i++)
    n += clauses[i];
  printf ("p cnf %d %d\n", m, n);
  map = calloc (2*m + 1, sizeof *map);
  map += m;
  if (qbf && !fp) 
    for (i = 0; i < nlayers; i++)
      {
	if (!i && !quant[0]) continue;
	fputc (quant[i] < 0 ? 'a' : 'e', stdout);
	for (j = low[i]; j <= high[i]; j++)
	  printf (" %d", j);
	fputs (" 0\n", stdout);
      }
  for (i = 0; i < nlayers; i++)
    {
      for (j = 0; j < clauses[i]; j++)
	{
	  l = 3;
	  while (l < MAX && pick (17, 19) != 17)
	    l++;

	  for (k = 0; k < l; k++)
	    {
	      layer = i;
	      while (layer && pick (3, 4) == 3)
		layer--;
	      if (nunused[layer])
		{
		  o = nunused[layer] - 1;
		  p = pick (0, o);
		  lit = unused[layer][p];
		  if (mark [abs (lit)]) continue;
		  nunused[layer] = o;
		  if (p != o) unused[layer][p] = unused[layer][o];
		}
	      else
		{
		  lit = pick (low[layer], high[layer]);
		  if (mark[lit]) continue;
		  sign = (pick (31, 32) == 31) ? 1 : -1;
		  lit *= sign;
		}
	      clause[k] = lit;
	      mark[abs (lit)] = 1;
	      printf ("%d ", lit);
	    }
	  printf ("0\n");
	  for (k = 0; k < l; k++)
	    mark[abs (clause[k])] = 0;
	}
    }
  map -= m;
  free (map);
  free (mark);
  free (clauses);
  free (high);
  free (low);
  free (width);
  free (nunused);
  free (quant);
  for (i = 0; i < nlayers; i++)
    free (layers[i]), free (unused[i]);
  free (layers);
  return 0;
}
