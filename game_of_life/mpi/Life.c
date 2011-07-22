/*******************************************
MPI Life 1.0
Copyright 2002, David Joiner and
  The Shodor Education Foundation, Inc.
Updated 2010, Andrew Fitz Gibbon and
  The Shodor Education Foundation, Inc.
Updated 2010, Tiago Sommer Damasceno and
  The Shodor Education Foundation, Inc.

A C implementation of Conway's Game of Life.

To run:
mpirun -np <num procs> Life [Rows] [Columns] [Generations] [Display]

See the README included in this directory for
more detailed information.
*******************************************/

#include "Life.h"
#include "Defaults.h" // For Life's constants

int main(int argc, char ** argv) {

	int count;
	struct life_t life;


	init(&life, &argc, &argv);

	for (count = 0; count < life.generations; count++) {

		copy_bounds(&life);

		eval_rules(&life);

		update_grid(&life);
	}
	cleanup(&life);
	exit(EXIT_SUCCESS);
}

/*
 * Initialize runtime environment and initializes MPI.
 */
int init (struct life_t * life, int * c, char *** v) 
{
	int argc          = *c;
	char ** argv      = *v;
	life->rank        = 0;
	life->size        = 1;
	life->ncols       = DEFAULT_SIZE;
	life->nrows       = DEFAULT_SIZE;
	life->generations = DEFAULT_GENS;
	life->infile      = NULL;
	life->outfile     = NULL;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &life->rank);
	MPI_Comm_size(MPI_COMM_WORLD, &life->size);

	seed_random(life->rank);

	parse_args(life, argc, argv);

	init_grids(life);

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

	for (i = 1; i <= nrows; i++) {
		for (j = 1; j <= ncols; j++) {
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

	int rank  = life->rank;
	int size  = life->size;
	int ncols = life->ncols;
	int nrows = life->nrows;

	int ** grid = life->grid;

	MPI_Status status;
	int left_rank  = (rank-1+size) % size;
	int right_rank = (rank+1) % size;

	enum TAGS {
		TOLEFT,
		TORIGHT
	};

	//	Some MPIs deadlock if a single process tries 
	//to communicate with itself
	if (size != 1) {
        	// initialize a package for each direction
		int *sendpackage = (int*)calloc(nrows+2, sizeof(int));
		int *recievepackage = (int*)calloc(nrows+2, sizeof(int));

		// Send to left
		for (i = 0; i < nrows+2; i++)
			sendpackage[i] = grid[i][1];

		MPI_Sendrecv(sendpackage, nrows+2, MPI_INT, left_rank, 
			TOLEFT, recievepackage, nrows+2, MPI_INT, right_rank, 
			TOLEFT, MPI_COMM_WORLD, &status);

		// Recieve from right
		for (i = 0; i < nrows+2; i++)
			grid[i][ncols+1] = recievepackage[i];

		// Send to right
		for (i = 0; i < nrows+2; i++)
			sendpackage[i] = grid[i][ncols];

		MPI_Sendrecv(sendpackage, nrows+2, MPI_INT, right_rank,
			TORIGHT, recievepackage, nrows+2, MPI_INT, left_rank,
			TORIGHT, MPI_COMM_WORLD, &status);

		// Recieve from left
		for (i = 0; i < nrows+2; i++)
			grid[i][0] = recievepackage[i];

		free(sendpackage);
		free(recievepackage);
	}

	// Copy sides locally to maintain periodic boundaries
	// when there's only one process
	if (size == 1) {
		for (i = 0; i < nrows+2; i++) {
			grid[i][ncols+1] = grid[i][1];
			grid[i][0] = grid[i][ncols];
		}

		// copy corners
		grid[0][0]             = grid[nrows][0];
		grid[nrows+1][0]       = grid[1][0];
		grid[0][ncols+1]       = grid[nrows][ncols+1];
		grid[nrows+1][ncols+1] = grid[1][ncols+1];
	}

	// copy top and bottom
	for (i = 1; i <= ncols; i++) {
		grid[0][i]       = grid[nrows][i];
		grid[nrows+1][i] = grid[1][i];
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

	for (i = 0; i < nrows+2; i++)
		for (j = 0; j < ncols+2; j++)
			grid[i][j] = next_grid[i][j];
}

/*
 * Allocates memory for a 2D array of integers.
 */
void allocate_grids (struct life_t * life) 
{
	int i,j;
	int ncols = life->ncols;
	int nrows = life->nrows;

	life->grid      = (int **) malloc(sizeof(int *) * (nrows+2));
	life->next_grid = (int **) malloc(sizeof(int *) * (nrows+2));

	for (i = 0; i < nrows+2; i++) {
		life->grid[i]      = (int *) malloc(sizeof(int) * (ncols+2));
		life->next_grid[i] = (int *) malloc(sizeof(int) * (ncols+2));
	}
}

/*
 * Initialize cells based on input file, otherwise all cells
 * are DEAD.
 */
void init_grids (struct life_t * life) 
{
	FILE * fd;
	int i,j;
	int ubound, lbound;

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

	// resize so each process is in charge of a vertical slice of the whole board
	ubound = (((life->rank + 1) * life->ncols / life->size) - 1); // we want 1 col of (overlap?)
	lbound = life->rank * life->ncols / life->size;
	life->ncols = (ubound - lbound) + 1;

	fprintf(stderr, "[Process %d] lower bound is %d upper bound is %d width is %d.\n", life->rank, lbound, ubound, life->ncols);

	allocate_grids(life);

	for (i = 0; i < life->nrows+2; i++) {
		for (j = 0; j < life->ncols+2; j++) {
			life->grid[i][j]      = DEAD;
			life->next_grid[i][j] = DEAD;
		}
	}

	if (life->infile != NULL) {
		while (fscanf(fd, "%d %d\n", &i, &j) != EOF) {
			if (j <= ubound && j >= lbound){
				fprintf(stderr, "[Process %d] %d %d -> %d %d.\n", life->rank, i, j, i, j-lbound);
				life->grid[i+1][j-lbound+1]      = ALIVE;
				life->next_grid[i+1][j-lbound+1] = ALIVE;
			}
		}
		
		fclose(fd);
	} else {
		randomize_grid(life, INIT_PROB);
	}

	// for debugging purposes
	fprintf(stderr,"[Host %d] printing initial slice.\n", life->rank);
	for (i = 1; i <= life->nrows; i++){
		for (j = 1; j <= life->ncols; j++){
			if (life->grid[i][j] == ALIVE)
				fprintf(stderr,"[#]");
			else
				fprintf(stderr,"[ ]");
		}
		fprintf(stderr,"\n");      
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
	MPI_Status status;
	char buffer[20];
	int collect_tag = 123131242;

	// first write the masters data points to file
	if (!life->rank && life->outfile){
		if ((fd = fopen(life->outfile, "w")) == NULL) {
			perror("Failed to open file for output");
			exit(EXIT_FAILURE);
		}

		//fprintf(fd, "%d %d\n", ncols, nrows);

		for (i = 1; i <= nrows; i++) {
			for (j = 1; j <= ncols; j++) {
				if (grid[i][j] != DEAD)
					fprintf(fd, "%d %d\n", i, j);
			}
		}
		fclose(fd);
	}

	// collect datapoints from each node and write them to file
	if (!life->rank && life->outfile){
		if ((fd = fopen(life->outfile, "a")) == NULL) {
			perror("Failed to open file for output");
			exit(EXIT_FAILURE);
		}
		for (i = 1; i < life->size; i++){
			while(1){
				MPI_Recv(buffer, 20, MPI_CHAR, i, collect_tag, MPI_COMM_WORLD, &status);
				if(buffer[0] == '\0') break;
				fprintf(fd, "%s\n", buffer);
			}
		}
		fclose(fd);
	} else if (life->rank && life->outfile){
		for (i = 1; i <= nrows; i++) {
			for (j = 1; j <= ncols; j++) {
				if (grid[i][j] != DEAD){
                                        sprintf(buffer,"%d %d", i, j); 
					MPI_Send(buffer, 20, MPI_CHAR, 0, collect_tag, MPI_COMM_WORLD);
				}
			}
		}
		buffer[0] = '\0';
		MPI_Send(buffer, 20, MPI_CHAR, 0, collect_tag, MPI_COMM_WORLD); //let the master know this process is finished
	}

	// for debugging purposes
	fprintf(stderr,"[Host %d] printing finished slice.\n", life->rank);
	for (i = 1; i <= life->nrows; i++){
		for (j = 1; j <= life->ncols; j++){
			if (grid[i][j] == ALIVE)
				fprintf(stderr,"[#]");
			else
				fprintf(stderr,"[ ]");
		}
		fprintf(stderr,"\n");      
	}			
}

/*
 * Frees memory used by an array that was allocated 
 * with allocate_grids().
 */
void free_grids (struct life_t * life) 
{
	int i;
	int ncols = life->ncols;
	int nrows = life->nrows;

	for (i = 0; i < nrows+2; i++) {
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
 * Initialize a Life grid. Each cell has a [prob] chance
 * of starting alive.
 */
void randomize_grid (struct life_t * life, double prob) 
{
	int i,j;
	int ncols = life->ncols;
	int nrows = life->nrows;

	fprintf(stderr, "This process is randomizing its grid using ncols = %d and nrows = %d.\n", ncols, nrows);
	for (i = 1; i <= nrows; i++) {
		for (j = 1; j <= ncols; j++) {
			if (rand_double() < prob)
				life->grid[i][j] = ALIVE;
		}
	}
}

/*
 * Seed the random number generator based on the
 * process's rank and time. Multiplier is arbitrary.
 * TODO allow the user to specify determinism.
 */
void seed_random (int rank) { srandom(0); }

/*
 * Prepare process for a clean termination.
 */
void cleanup (struct life_t * life) 
{
	write_grid(life);
	free_grids(life);

	MPI_Finalize();
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
			case 'c':
				life->ncols = strtol(optarg, (char**) NULL, 10);
				break;
			case 'r':
				life->nrows = strtol(optarg, (char**) NULL, 10);
				break;
			case 'g':
				life->generations = strtol(optarg, (char**) NULL, 10);
				break;
			case 'i':
				life->infile = optarg;
				break;
			case 'o':
				life->outfile = optarg;
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
	}
}
