/** \file   htcbdd.c
 *  \brief  Hypergraph Transversal Computation (Minimal Hitting Set Generation) Based on Binary Decision Diagrams
 *  \author Takahisa Toda
 *  \see    T.Toda, Hypergraph Transversal Computation with Binary Decision Diagrams, in Proc. of     12th International Symposium on Experimental Algorithms (SEA2013), LNCS 7933, pp.91-102, Rome, Italy (2013). 
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "my_def.h"
#include "rng.h"
#include "my_opcache.h"
#include "bdd_io_setfam.h"
#include "bdd_hit.h"
#include "bdd_min.h"

#define PROGNAME "HTCBDD"
#define PROGVERS "1.2.2"

#ifdef SELECT_CUDD
DdManager *dd_mgr = NULL; //!< BDD/ZDD manager for CUDD
#endif /*SELECT_CUDD*/

#ifdef SELECT_GSL
my_rng *g_rng = NULL; //!< random number generator for GSL
#endif /*SELECT_GSL*/

#define PRINT_USAGE(p) do{fprintf(stderr, "Usage: %s [option] input-file [output-file]\n", (p)); \
    fprintf(stderr, "\t-t Toda method (default)\n");			\
    fprintf(stderr, "\t-k Knuth method\n");				\
  } while(0)

#if defined(MISC_LOG) || defined(SIZE_LOG)
  char *g_basename; //!< basename of input file
#endif /*defined(MISC_LOG) || defined(SIZE_LOG)*/


#define T_METHOD 't'
#define K_METHOD 'k'

int main(int argc, char *argv[])
{   
  int res       = ST_SUCCESS;
  char method   = T_METHOD;
  char *infile  = NULL;
  char *outfile = NULL;

  /*** INPUT ARGUMENT ***/  
  for(int i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      switch (argv[i][1]){
      case T_METHOD: case K_METHOD:
        method = argv[i][1];
	      break; 
      case 'v': case '?': case 'h': default:
	      PRINT_USAGE(argv[0]); return  ST_SUCCESS;
      }
    } else {
      if     (infile  == NULL)  {infile = argv[i];}
      else if(outfile == NULL)  {outfile = argv[i];}
      else                      {PRINT_USAGE(argv[0]); return ST_SUCCESS;}
    }
  }
  if(infile == NULL) {PRINT_USAGE(argv[0]); return ST_SUCCESS;}
#if defined(MISC_LOG) || defined(SIZE_LOG)
  time_t timer        = time(NULL);
  struct tm *execdate = localtime(&timer);
  g_basename = infile;
  for(int i = 0; infile[i] != '\0'; i++) {if(infile[i] == '/') g_basename = infile+i+1;}
#endif /*defined(MISC_LOG) || defined(SIZE_LOG)*/
#ifdef MISC_LOG
  printf("date\t%s",          asctime(execdate));
  printf("program\t%s-%s\n",  PROGNAME, PROGVERS);
  printf("package\t%s\n",     BDD_PACKAGE);
  printf("input\t%s\n",       infile);
  if(outfile != NULL)     printf("output\t%s\n", outfile);
  if(method == T_METHOD)  printf("method\tToda\n");
  else                    printf("method\tKnuth\n");
  printf("\n");
  fflush(stdout);
#endif /*MISC_LOG*/

  FILE *in = fopen(infile, "r");
  ENSURE_TRUE_MSG(in != NULL, "file open failed\n");

  /*** SETUP BDD PACKAGE ***/
  rng_create();
  uintmax_t row_count, entry_count;
  itemval maxval;
  res = getfileinfo(&row_count, &maxval, &entry_count, in);
  ENSURE_SUCCESS(res);
  ENSURE_TRUE_MSG(maxval <= BDD_MAXITEMVAL, "the number of variable too large");
  bdd_init(maxval, 0);


  /*** CONSTRUCT ZDD FROM DATAFILE ***/  
  clock_t startingtime = current_time();
  zddp f = zcomp(row_count, entry_count, in);
  ENSURE_TRUE_MSG(f != BDD_NULL, "ZDD construction failed");
  print_elapsed_time(startingtime, "INPUT");
#ifdef MISC_LOG
  printf("|zdd|\t%ju\n",      zdd_size(f));
  printf("#sets\t%ju\n",      zdd_card(f));
  printf("\n");
  fflush(stdout);
#endif /*MISC_LOG*/
  
  /*** HITTING SET GENERATION PART ***/
  zddp g;
  switch(method) {
  case T_METHOD:
    startingtime = current_time();
    bddp t = hit_z2b(f);
    ENSURE_TRUE(t != BDD_NULL);
    print_elapsed_time(startingtime, "HIT");
#ifdef MISC_LOG
    printf("|bdd|\t%ju\n",      bdd_size(t));
    printf("\n");
    fflush(stdout);
#endif /*MISC_LOG*/
    
  /*** MINIMIZATION PART ***/
    startingtime = current_time();
    g = min_b2z(t);
    ENSURE_TRUE(g != BDD_NULL);
    print_elapsed_time(startingtime, "MIN");
#ifdef MISC_LOG
    printf("|zdd|\t%ju\n",      zdd_size(g));
    printf("#sets\t%ju\n",      zdd_card(g));
    printf("\n");
    fflush(stdout);
#endif /*MISC_LOG*/
    break;
    
  case K_METHOD:
    startingtime = current_time();
    g = minhit_nonsup(f);
    ENSURE_TRUE(g != BDD_NULL);
    print_elapsed_time(startingtime, "MINHIT");
#ifdef MISC_LOG
    printf("|zdd|\t%ju\n",      zdd_size(g));
    printf("#sets\t%ju\n",      zdd_card(g));
    printf("\n");
    fflush(stdout);
#endif /*MISC_LOG*/
    break;

  default:
    ENSURE_TRUE_WARN(false, "invalid mode");
    exit(EXIT_FAILURE);
    break;
  }

  /*** EXPORT ZDD ***/  
  if(outfile != NULL) {
    FILE *out = fopen(outfile, "w");
    startingtime = current_time();
    unzcomp(maxval, g, out);
    print_elapsed_time(startingtime, "OUTPUT");
    fclose(out);
  }

  bdd_quit(); 
  rng_destroy();
  fclose(in);

  return ST_SUCCESS;
}


