/** \file     rng.h
 *  \brief    Functions to manipulate a random number generator.
 *  \author   Takahisa Toda
 *  \note     If the macro SELECT_GSL is defined, then a random number generator provided by GNU GSL is used; otherwise, a default random number generator is used.
 *  \see      http://www.gnu.org/software/gsl/
 */
#ifndef RNG_H
#define RNG_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "my_def.h"

#ifdef SELECT_GSL
/*---------------------------------------------------------------------------*/
/* GNU GSL                                                                   */
/*---------------------------------------------------------------------------*/
#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

typedef gsl_rng my_rng;

extern my_rng *g_rng;

/** \brief    Calculate a random number.
 *  \param    n     Maximum number plus 1
 *  \return   A random number r with 0 <= r < n.
 *  \see      rng_create 
 *  \note     Before execution, call rng_create once.
 */
static inline uintmax_t rng_rand(uintmax_t n)
{
  assert(g_rng != NULL);
  assert(n <= gsl_rng_max(g_rng));
  return (uintmax_t)gsl_rng_get(g_rng)%n;
}


/** \brief    Create and initialize a random number generator.
 *  \see      rng_rand 
 */
static inline void rng_create(void)
{
  if (g_rng == NULL) {
    g_rng = gsl_rng_alloc(gsl_rng_default);
    ENSURE_TRUE_MSG(g_rng != NULL, "a random number generator creation failed");
  } else {
    ENSURE_TRUE_WARN(false, "random number generator already initialized");
  }
  
  const time_t now = time(NULL);
  assert(now != (time_t) -1);
  gsl_rng_set(g_rng, (unsigned long int) now);

#ifdef MISC_LOG
  printf("generator_type\t%s\n", gsl_rng_name(g_rng));
  printf("first_value\t%lu\n",   gsl_rng_get(g_rng)); 
  printf("max_value\t%lu\n",     gsl_rng_max(g_rng));
  printf("seed\t%lu\n",          (unsigned long int)now);
  printf("\n");
#endif /*MISC_LOG*/
}

/** \brief    Free memory area.
 *  \see      rng_create 
 */
static inline void rng_destroy(void)
{
  if (g_rng != NULL) {
    gsl_rng_free(g_rng);
    g_rng = NULL;
  } else {
    ENSURE_TRUE_WARN(false, "random number generator already destroyed");
  }
}

#else /*SELECT_GSL*/
/*---------------------------------------------------------------------------*/
/* DEFAULT RANDOM NUMBER GENERATOR                                           */
/*---------------------------------------------------------------------------*/

/** \brief    Calculate a random number.
 *  \param    n     Maximum number plus 1
 *  \return   A random number r with 0 <= r < n.
 *  \see      rng_create
 *  \note     Before execution, call rng_create once.
 */
static inline uintmax_t rng_rand(uintmax_t n)
{
  assert(n > 0);
  //return (uintmax_t)random()\%n; // this is preferable to the C standard rand(), thus replace if possible.
  return (uintmax_t)rand()%n;
}


/** \brief    Create and initialize a random number generator on your environment.
 *  \see      rng_rand 
 */
static inline void rng_create(void)
{
  const time_t now = time(NULL);
  assert(now != (time_t) -1);
  //srandom((unsigned int) now); // this is preferable to the C standard srand(), thus replace if possible.
  srand((unsigned int) now);
#ifdef MISC_LOG
  printf("generator_type\tdefault\n");
  //printf("first_value\t%d\n", random());  // this is preferable to the C standard rand(), thus replace if possible.
  printf("first_value\t%d\n", rand()); 
  printf("max_value\t%d\n",   RAND_MAX);
  printf("seed\t%u\n",        (unsigned int) now);
  printf("\n");
#endif /*MISC_LOG*/
}


/** \brief    Free memory area.
 *  \see      rng_create 
 */
static inline void rng_destroy(void)
{
}
#endif  /*SELECT_GSL*/

#endif /*RNG_H*/
