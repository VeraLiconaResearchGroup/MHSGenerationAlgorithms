CC = gcc
CFLAGS = -std=c99 -pedantic-errors -Wall -O3 -Winline -DNDEBUG

MYFLAGS = -DSELECT_CUDD
MYFLAGS += -DRECDEPTH_GUARD        # prevent recursive function calls from stack overf low.
MYFLAGS += -DMISC_LOG -DTIME_LOG   # provide basic information on program behavior and various settings in stdout.
#MYFLAGS += -DSIZE_LOG              # output intermediate decision diagram sizes as files.
MYFLAGS += -DOPCACHE_LOG           # provide detailed log of operation cache in stdout.

CUDD_DIR  = cudd-2.5.0
CUDD_INCLUDE = -I$(CUDD_DIR)/include
CUDD_LIB = $(CUDD_DIR)/dddmp/libdddmp.a $(CUDD_DIR)/cudd/libcudd.a $(CUDD_DIR)/mtr/libmtr.a \
	    $(CUDD_DIR)/st/libst.a $(CUDD_DIR)/util/libutil.a $(CUDD_DIR)/epd/libepd.a

# Uncomment the following two lines if you like a random number generator provided by GSL (installed?).
# MYFLAGS += -DSELECT_GSL
# RAND_LIB = -lgsl -lgslcblas

# If you want to use other BDD libraries, register the locations of their header files and libraries below, change SELECT_CUDD so that your library is selected, and modify bdd_interface.h, where all functions to access BDD libraries are defined.
INCLUDE   = $(CUDD_INCLUDE)
LIB       = $(CUDD_LIB) $(RAND_LIB) -lm

OBJ0 = bdd_hit.o bdd_min.o my_hash.o my_opcache.o my_io_setfam.o bdd_io_setfam.o ssort1.o bdd_extra.o
OBJ1 = htcbdd.o $(OBJ0)

.c.o:
	$(CC) $(CFLAGS) $(MYFLAGS) $(INCLUDE) -c $<

all: $(CUDD_DIR) htcbdd

htcbdd: $(OBJ1)
	$(CC) $(CFLAGS) $(MYFLAGS) -o $@ $(OBJ1) $(LIB)

clean:
	rm -f *.o *.exe *~ mon.out gmon.out htcbdd
	$(MAKE) -C $(CUDD_DIR) distclean

htcbdd.o:					my_io_setfam.h bdd_io_setfam.h bdd_interface.h bdd_min.h bdd_hit.h my_hash.h ssort1.h my_def.h rng.h
my_io_setfam.o:		my_io_setfam.h my_def.h
bdd_io_setfam.o:	bdd_io_setfam.h my_io_setfam.h bdd_interface.h my_def.h ssort1.h
ssort1.o:					ssort1.h rng.h my_def.h
my_hash.o:				my_hash.h my_def.h
my_opcache.o:			my_opcache.h my_def.h
bdd_extra.o:			bdd_interface.h my_opcache.h my_def.h
bdd_min.o:				bdd_min.h my_hash.h bdd_interface.h my_def.h
bdd_hit.o:				bdd_hit.h my_hash.h bdd_interface.h my_def.h

.PHONY: $(CUDD_DIR)
$(CUDD_DIR):
	$(MAKE) -C $@
