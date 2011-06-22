# matrix_multi_cluster/Makefile
# -------------------------------------------------------------------------

SHELL  		= /bin/bash
srcdir		= .
top_scrdir	= ..

MAKE		= make
MPILIBNAME	= mpich
PMPILIBNAME	= pmpich

C_COMPILE	= $(CC) $(DEFS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS)
C_LINK		= $(CC)
CC_SHL		= true
C_COMPILE_SHL	= $(CC_SHL) $(DEFS) $(INCLUDES) $(CFLAGS) $(CPPFLAGS)

VPATH		= .
MPICHDIR	= ..
CC		= $(MPICHDIR)/bin/mpicc
CFLAGS		= 
CXXFLAGS	=
FFLAGS		=
FCFLAGS		=
LDFLAGS		=
CPPFLAGS	=
DEFS		=
INCLUDES 	=
LIBS		=

.SUFFIXES:
.SUFFIXES: .o .lo .c
.c.o:
	@if [ "x$(VERBOSE)" != "x1" -a "x$(V)" != "x1" ] ; then \
		echo "  CC	$<" ; \
	else \
		echo $(C_COMPILE) -c $< ; \
	fi
	@$(C_COMPILE) -c $<
.c.lo:
	@if [ "x$(VERBOSE)" != "x1" -a "x1" -a "x$(V)" != "x1" ] ; then \
		echo "  CC	$<" ; \
	else \
		echo $(C_COMPILE_SHL) -c $< -o _s$*.o ; \
	echo mv -f _s$*.o $*.lo ; \
	fi
	@$(C_COMPILE_SHL) -c $< -o _s$*.o
	@mv -f _s$*.o $*.lo
.c:
	@if [ "x$(VERBOSE)" != "x1" -a "x$(V)" != "x1" ] ; then \
		echo "  CC	$<" ; \
	else \
		echo $(C_COMPILE) -o $* $< $(LDFLAGS) $(LIBS) ; \
	fi
	@$(C_COMPILE) -o $* $< $(LDFLAGS) $(LIBS)
apply:
	$(ACTION) $(ACTION_INPUT)
clean:
	-@rm -f *.o *.lo *.irlog* matmulti 

matmulti: matmulti.o
	$(C_LINK) $(CFLAGS) $(LDFLAGS) -o matmulti matmulti.o -lm $(LIBS)

all: matmulti

dependencies:

SOURCES		= matmulti.c
HEADERFILES	= $(HEADERS)
SOURCEFILES	= $(SOURCES)

.POSIX:

FORCE_TARGET:
