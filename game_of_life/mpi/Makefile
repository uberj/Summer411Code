###############################################################################
# Makefile for the version of Game of Life with no paralleism as included on 
# the BCCD (http://bccd.net)
#
# By default, include X display.
#
# Add NO_X11=1 to omit X libraries.
###############################################################################

#
# Variables and Flags
#

CC        = mpicc 


LIBS     += -lm
CFLAGS   += 
LDFLAGS  += $(LIBS)
PROGRAM   = Life
SRCS      = Life.c

###### MPI OPTIONS##
CFLAGS += -DHAS_MPI
LIBS += -lmpi
#################### 

ifndef NO_X11
LIBS     += -lX11
LDFLAGS  += -L/usr/X11R6/lib
else
CFLAGS   += -DNO_X11
endif

OBJS      = $(SRCS:.c=.o)		# object file

#
# Targets
#

default: all

all: $(PROGRAM) 

$(PROGRAM): $(OBJS)
	$(CC) -o $(PROGRAM) $(SRCS) $(CFLAGS) $(LDFLAGS)

clean:
	/bin/rm -f $(OBJS) $(PROGRAM)

