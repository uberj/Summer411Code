/*******************************************
Life 1.0
Copyright 2002, David Joiner and
  The Shodor Education Foundation, Inc.
Updated 2010, Andrew Fitz Gibbon and
  The Shodor Education Foundation, Inc.
Updated Summer 2010, Tiago Sommer Damasceno and
  The Shodor Education Foundation, Inc.
*******************************************/
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#ifndef NO_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

#ifndef BCCD_LIFE_DEFAULTS_H
#define BCCD_LIFE_DEFAULTS_H

// Default parameters for the simulation
#define DEFAULT_THROTTLE 60
#define DEFAULT_SIZE 105
#define DEFAULT_GENS 1000
#define INIT_PROB 0.25
#ifndef NO_X11
#define DEFAULT_DISP true
#else
#define DEFAULT_DISP false
#endif

// Size, in pixels, of the X window(s)
#define DEFAULT_WIDTH 500
#define DEFAULT_HEIGHT 500

// Number of possible shades of gray
#define NUM_GRAYSCALE 10

enum CELL_STATES {
	DEAD = 0,
	ALIVE
};

// Cells become DEAD with more than UPPER_THRESH 
// or fewer than LOWER_THRESH neighbors
#define UPPER_THRESH 3
#define LOWER_THRESH 2

// Cells with exactly SPAWN_THRESH neighbors become ALIVE
#define SPAWN_THRESH 3


static const char * opts = "c:r:g:i:o:t::xh?";
static const struct option long_opts[] = {
	{ "columns", required_argument, NULL, 'c' },
	{ "rows", required_argument, NULL, 'r' },
	{ "gens", required_argument, NULL, 'g' },
	{ "no-display", no_argument, NULL, 0 },
	{ "display", no_argument, NULL, 'x' },
	{ "output", required_argument, NULL, 'o' },
	{ "input", required_argument, NULL, 'i' },
	{ "throttle", optional_argument, NULL, 't' },
	{ "help", no_argument, NULL, 'h' },
	{ NULL, no_argument, NULL, 0 }
};

#endif

#ifndef BCCD_XLIFE_H
#define BCCD_XLIFE_H

// All the data needed for an X display
struct display_t {
	#ifndef NO_X11
	Window    w;
	GC        gc;
	Display * dpy;
	Pixmap    buffer;
	Colormap  theColormap;
	XColor    Xgrayscale[NUM_GRAYSCALE];

	int deadColor;
	int liveColor;
	int width;
	int height;
	#endif
};

#endif

#ifndef BCCD_LIFE_H
#define BCCD_LIFE_H

// All the data needed by an instance of Life
struct life_t {
	int  size;		
	int  throttle;		//Speed in which X11 will show the simulation.
	int  ncols;		
	int  nrows;		
	int  ** grid;		//Current grid.
	int  ** next_grid;	//Grid with updated values
	bool do_display;	//X11 Switch
	int  generations;	
	char * infile;		
	char * outfile;		

	struct display_t disp;
};

#endif

/*
 * Function prototypes
 * defined in XLife.c
 */
void        free_video (struct life_t * life);
void        moveWindow (struct life_t * life);
void       setupWindow (struct life_t * life);
void           do_draw (struct life_t * life);

/*
 * Function prototypes
 * defined in Life.c
 */
int               init (struct life_t * life, int * c, char *** v);
void        eval_rules (struct life_t * life);
void       copy_bounds (struct life_t * life);
void       update_grid (struct life_t * life);
void          throttle (struct life_t * life);
void    allocate_grids (struct life_t * life);
void        init_grids (struct life_t * life);
void        write_grid (struct life_t * life);
void        free_grids (struct life_t * life);
double     rand_double ();
void    randomize_grid (struct life_t * life, double prob);
void           cleanup (struct life_t * life);
void        parse_args (struct life_t * life, int argc, char ** argv);
void             usage ();


