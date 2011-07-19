/*************************************************
MPI Life 1.0
Copyright 2002, David Joiner and
  The Shodor Education Foundation, Inc.
Updated 2010, Andrew Fitz Gibbon and
  The Shodor Education Foundation, Inc.
Updated Summer 2010, Tiago Sommer Damasceno and
  The Shodor Education Foundation, Inc.
*************************************************/

#ifndef BCCD_LIFE_H
#define BCCD_LIFE_H

#include <mpi.h>

#include "Defaults.h" // For Life's constants

#include <time.h>     // For seeding random
#include <stdlib.h>   // For malloc et al.
#include <stdbool.h>  // For true/false
#include <getopt.h>   // For argument processing
#include <stdio.h>    // For file i/o


int               init (struct life_t * life, int * c, char *** v);
void        eval_rules (struct life_t * life);
void       copy_bounds (struct life_t * life);
void       update_grid (struct life_t * life);
void    allocate_grids (struct life_t * life);
void        init_grids (struct life_t * life);
void        write_grid (struct life_t * life);
void        free_grids (struct life_t * life);
double     rand_double ();
void    randomize_grid (struct life_t * life, double prob);
void       seed_random (int rank);
void           cleanup (struct life_t * life);
void        parse_args (struct life_t * life, int argc, char ** argv);
void             usage ();

#endif
