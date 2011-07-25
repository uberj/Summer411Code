/*******************************************
MPI Life 1.0
Copyright 2002, David Joiner and
  The Shodor Education Foundation, Inc.
Updated 2010, Andrew Fitz Gibbon and
  The Shodor Education Foundation, Inc.
Updated 2010, Tiago Sommer Damasceno and
  The Shodor Education Foundation, Inc.
*******************************************/

#ifndef BCCD_LIFE_DEFAULTS_H
#define BCCD_LIFE_DEFAULTS_H

#include <stddef.h>
#include <stdbool.h>
#include <getopt.h>

static const char * opts = "c:r:g:s:i:o:t::xph?";
static const struct option long_opts[] = {
	{ "columns", required_argument, NULL, 'c' },
	{ "rows", required_argument, NULL, 'r' },
	{ "gens", required_argument, NULL, 'g' },
	{ "seed", required_argument, NULL, 's' },
	{ "output", required_argument, NULL, 'o' },
	{ "input", required_argument, NULL, 'i' },
	{ "throttle", optional_argument, NULL, 't' },
	{ "print", no_argument, NULL, 'p' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

// Default parameters for the simulation
const int DEFAULT_THROTTLE = 60;
const int     DEFAULT_SIZE = 105;
const int     DEFAULT_GENS = 1000;
const int     DEFAULT_SEED = 0;
const double     INIT_PROB = 0.25;

// All the data needed by an instance of Life
struct life_t {
	int  rank;
	int  size;
	int  tcols;
	int  lbound;
	int  ubound;
	int  ncols;
	int  nrows;
	int  ** grid;
	int  ** next_grid;
	int  generations;
	char * infile;
	char * outfile;
	bool print;
	int  randseed;
};

enum CELL_STATES {
	DEAD = 0,
	ALIVE
};

// Cells become DEAD with more than UPPER_THRESH 
// or fewer than LOWER_THRESH neighbors
const int UPPER_THRESH = 3;
const int LOWER_THRESH = 2;

// Cells with exactly SPAWN_THRESH neighbors become ALIVE
const int SPAWN_THRESH = 3;

#endif
