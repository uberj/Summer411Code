/*******************************************
Life 1.0
Copyright 2002, David Joiner and
  The Shodor Education Foundation, Inc.
Updated 2010, Andrew Fitz Gibbon and
  The Shodor Education Foundation, Inc.
Updated Summer 2010, Tiago Sommer Damasceno and
  The Shodor Education Foundation, Inc.
  
A C implementation of Conway's Game of Life.

To run:
./Life [Rows] [Columns] [Generations] [Display]

See the README included in this directory for
more detailed information.
*******************************************/

#include "Life.h"

#include <time.h>    
#include <stdlib.h>   
#include <stdbool.h>  
#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[]) 
{

	int count; // Variable used to perform the for-loop.

	struct life_t life; //	Variable that contains: size, throttle, 
		//ncols, nrows, generations, do_display, infile, outfile.     

	/*
	 * Initializes all life's variables
	 */
	init(&life, &argc, &argv);

	/*
	 * Loop containing instructions to simulate 
	 * the evolution of one generation.
	 */
	for (count = 0; count < life.generations; count++) {
		if (life.do_display)
			do_draw(&life);

		copy_bounds(&life);	// Copy neighbor's states(DEAD or ALIVE) into a two dimensional
					// array which will evaluated by the Life's rules

		eval_rules(&life);	// Finds out how many neighbor's each cell has then updates 
					// the cell's state to DEAD or ALIVE.

		update_grid(&life);	// Updates the old grid with the state value of each cell.

		throttle(&life);	// Slows down the simulation to make X display easier to watch.
	}

	cleanup(&life);			// Writes the coordinates of all the ALIVE cells into the output files
					// and frees all the memory used by the program

	exit(EXIT_SUCCESS);
}

/*
 * Initialize runtime environment.
 */
int init (struct life_t * life, int * c, char *** v) 
{
	int argc          = *c;
	char ** argv      = *v;
	life->size        = 1;
	life->throttle    = -1;
	life->ncols       = DEFAULT_SIZE;
	life->nrows       = DEFAULT_SIZE;
	life->generations = DEFAULT_GENS;
	life->do_display  = DEFAULT_DISP;
	life->infile      = NULL;
	life->outfile     = NULL;

	srandom(time(NULL));

	parse_args(life, argc, argv);

	init_grids(life);

	if (life->do_display) {
		setupWindow(life);
		moveWindow(life);
	}
}

/*
 * Evaluate the rules of Life for each cell; count
 * neighbors and update current state accordingly.
 */
void eval_rules (struct life_t * life) 
{
	int i,j,k,l,neighbors;

	int ncols = life->ncols;
	int nrows = life->nrows;

	int ** grid      = life->grid;
	int ** next_grid = life->next_grid;

	#pragma omp parallel for private(neighbors,i,j,k,l)
	for (i = 1; i <= ncols; i++) {
		for (j = 1; j <= nrows; j++) {
			neighbors = 0;

			// count neighbors
			for (k = i-1; k <= i+1; k++) {
				for (l = j-1; l <= j+1; l++) {
					if (!(k == i && l == j) && grid[k][l] != DEAD)
						neighbors++;
				}
			}

			// update state
			if (neighbors < LOWER_THRESH || neighbors > UPPER_THRESH)
				next_grid[i][j] = DEAD;
			else if (grid[i][j] != DEAD || neighbors == SPAWN_THRESH)
				next_grid[i][j] = grid[i][j]+1;
		}
	}
}

/*
 * Copies sides, top, and bottom to their respective locations.
 * All boundaries are considered periodic.
 * In the MPI model, processes are aligned side-by-side.
 * Left and right sides are sent to neighboring processes.
 * Top and bottom are copied from the process's own grid.
 */
void copy_bounds (struct life_t * life) 
{
	int i,j;

	int size  = life->size;
	int ncols = life->ncols;
	int nrows = life->nrows;

	int ** grid = life->grid;

	// Copy sides locally to maintain periodic boundaries
	// when there's only one process
	if (size == 1) {
		#pragma omp parallel for private(i,j)
		for (j = 0; j < nrows+2; j++) {
			grid[ncols+1][j] = grid[1][j];
			grid[0][j] = grid[ncols][j];
		}
	}

	// copy corners
	grid[0][0]             = grid[0][nrows];
	grid[0][nrows+1]       = grid[0][1];
	grid[ncols+1][0]       = grid[ncols+1][nrows];
	grid[ncols+1][nrows+1] = grid[ncols+1][1];

	// copy top and bottom
	#pragma omp parallel for private(i)
	for (i = 1; i <= ncols; i++) {
		grid[i][0]       = grid[i][nrows];
		grid[i][nrows+1] = grid[i][1];
	}
}

/*
 * Copies temporary values from next_grid into grid.
 */
void update_grid (struct life_t * life) 
{
	int i,j;
	int ncols = life->ncols;
	int nrows = life->nrows;
	int ** grid      = life->grid;
	int ** next_grid = life->next_grid;

	#pragma omp parallel for private(i,j)
	for (i = 0; i < ncols+2; i++)
		for (j = 0; j < nrows+2; j++)
			grid[i][j] = next_grid[i][j];
}

/*
 * Slows down the simulation to make X display easier to watch.
 * Has no effect when run with --no-display.
 */
void throttle (struct life_t * life) 
{
	unsigned int delay;
	int t = life->throttle;

	if (life->do_display && t != -1) {
		delay = 1000000 * 1/t;
		usleep(delay);
	}
}

/*
 * Allocates memory for a 2D array of integers.
 */
void allocate_grids (struct life_t * life) 
{
	int i,j;
	int ncols = life->ncols;
	int nrows = life->nrows;

	life->grid      = (int **) malloc(sizeof(int *) * (ncols+2));
	life->next_grid = (int **) malloc(sizeof(int *) * (ncols+2));

	#pragma omp parallel for private(i,j)
	for (i = 0; i < ncols+2; i++) {
		life->grid[i]      = (int *) malloc(sizeof(int) * (nrows+2));
		life->next_grid[i] = (int *) malloc(sizeof(int) * (nrows+2));
	}
}

/*
 * Initialize cells based on input file, otherwise all cells are DEAD.
 */
void init_grids (struct life_t * life) 
{
	FILE * fd;
	int i,j;

	if (life->infile != NULL) {
		if ((fd = fopen(life->infile, "r")) == NULL) {
			perror("Failed to open file for input");
			exit(EXIT_FAILURE);
		}

		if (fscanf(fd, "%d %d\n", &life->ncols, &life->nrows) == EOF) {
			printf("File must at least define grid dimensions!\nExiting.\n");
			exit(EXIT_FAILURE);
		}
	}

	allocate_grids(life);

	#pragma omp parallel for private(i,j)
	for (i = 0; i < life->ncols+2; i++) {
		for (j = 0; j < life->nrows+2; j++) {
			life->grid[i][j]      = DEAD;
			life->next_grid[i][j] = DEAD;
		}
	}

	if (life->infile != NULL) {
		while (fscanf(fd, "%d %d\n", &i, &j) != EOF) {
			life->grid[i][j]      = ALIVE;
			life->next_grid[i][j] = ALIVE;
		}
		
		fclose(fd);
	} else {
		randomize_grid(life, INIT_PROB);
	}
}

/*
 * Dumps the current state of life.grid to life.outfile.
 * Only outputs the coordinates of !DEAD cells.
 */
void write_grid (struct life_t * life) 
{
	FILE * fd;
	int i,j;
	int ncols   = life->ncols;
	int nrows   = life->nrows;
	int ** grid = life->grid;

	if (life->outfile != NULL) {
		if ((fd = fopen(life->outfile, "w")) == NULL) {
			perror("Failed to open file for output");
			exit(EXIT_FAILURE);
		}

		fprintf(fd, "%d %d\n", ncols, nrows);
                
		#pragma omp parallel for private(i,j)
		for (i = 1; i <= ncols; i++) {
			for (j = 1; j <= nrows; j++) {
				if (grid[i][j] != DEAD)
					fprintf(fd, "%d %d\n", i, j);
			}
		}

		fclose(fd);
	}
}

/*
 * Frees memory used by an array that was allocated with allocate_grids().
 */
void free_grids (struct life_t * life) 
{
	int i;
	int ncols = life->ncols;

	#pragma omp parallel for private(i)
	for (i = 0; i < ncols+2; i++) {
		free(life->grid[i]);
		free(life->next_grid[i]);
	}

	free(life->grid);
	free(life->next_grid);
}

/*
 * Generate a random double between 0 and 1.
 */
double rand_double() { return (double)random()/(double)RAND_MAX; }

/*
 * Initialize a Life grid. Each cell has a [prob] chance of starting alive.
 */
void randomize_grid (struct life_t * life, double prob) 
{
	int i,j;
	int ncols = life->ncols;
	int nrows = life->nrows;

	#pragma omp parallel for private(i,j)
	for (i = 1; i <= ncols; i++) {          
		for (j = 1; j <= nrows; j++) {
			if (rand_double() < prob)
				life->grid[i][j] = ALIVE;
		}
	}
}

/*
 * Prepare process for a clean termination.
 */
void cleanup (struct life_t * life) 
{
	write_grid(life);
	free_grids(life);

	if (life->do_display)
		free_video(life);

}

/*
 * Describes Life's command line option
 */
void usage () 
{
	printf("\nUsage: Life [options]\n");
	printf("  -c|--columns number   Number of columns in grid. Default: %d\n", DEFAULT_SIZE);
	printf("  -r|--rows number      Number of rows in grid. Default: %d\n", DEFAULT_SIZE);
	printf("  -g|--gens number      Number of generations to run. Default: %d\n", DEFAULT_GENS);
	printf("  -i|--input filename   Input file. See README for format. Default: none.\n");
	printf("  -o|--output filename  Output file. Default: none.\n");
	printf("  -h|--help             This help page.\n");
	printf("  -t[N]|--throttle[=N]  Throttle display to N generations/second. Default: %d\n",
		DEFAULT_THROTTLE);
	printf("  -x|--display          Use a graphical display.\n");
	printf("  --no-display          Do not use a graphical display.\n"); 
	printf("     Default: %s\n",
		(DEFAULT_DISP ? "do display" : "no display"));
	printf("\nSee README for more information.\n\n");

	exit(EXIT_FAILURE);
}

/*
 * Make command line arguments useful
 */
void parse_args (struct life_t * life, int argc, char ** argv) 
{
	int opt       = 0;
	int opt_index = 0;
	int i;

	for (;;) {
		opt = getopt_long(argc, argv, opts, long_opts, &opt_index);

		if (opt == -1) break;

		switch (opt) {
			case 0:
				if (strcmp("no-display", long_opts[opt_index].name) == 0)
					life->do_display = false;
				break;
			case 'c':
				life->ncols = strtol(optarg, (char**) NULL, 10);
				break;
			case 'r':
				life->nrows = strtol(optarg, (char**) NULL, 10);
				break;
			case 'g':
				life->generations = strtol(optarg, (char**) NULL, 10);
				break;
			case 'x':
				life->do_display = true;
				break;
			case 'i':
				life->infile = optarg;
				break;
			case 'o':
				life->outfile = optarg;
				break;
			case 't':
				if (optarg != NULL)
					life->throttle = strtol(optarg, (char**) NULL, 10);
				else
					life->throttle = DEFAULT_THROTTLE;
				break;
			case 'h':
			case '?':
				usage();
				break;

			default:
				break;
		}
	}

	// Backwards compatible argument parsing
	if (optind == 1) {
		if (argc > 1)
			life->nrows       = strtol(argv[1], (char**) NULL, 10);
		if (argc > 2)
			life->ncols       = strtol(argv[2], (char**) NULL, 10);
		if (argc > 3)
			life->generations = strtol(argv[3], (char**) NULL, 10);
		if (argc > 4)
			// 0 interpreted as false, all other values true
			life->do_display  = strtol(argv[4], (char**) NULL, 10);
	}
}
