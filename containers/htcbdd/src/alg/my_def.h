/** \file    my_def.h
 *  \brief   Common definitions and utilities
 *  \author  Takahisa Toda
 */
#ifndef MY_DEF_H
#define MY_DEF_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/

#define inline   inline   __attribute__((always_inline))

#define ST_SUCCESS (0)
#define ST_FAILURE (-1)

#define INFTY  INT_MAX
typedef intmax_t itemval; //!< item value type, which must be able to contain INFTY and negative integers.

#if UINTPTR_MAX > UINTMAX_MAX
#error "Assumption: UINTPTR_MAX <= UINTMAX_MAX"
#endif


/** \brief    Obtain the current time.
 *  \return   The current time
 *  \note     To enable this function, define the macro TIME_LOG in Makefile.
 *  \see      print_elapsed_time
 */
static inline clock_t current_time(void)
{
#ifdef TIME_LOG
  return clock();
#else
  return 0;
#endif /*TIME_LOG*/
}
/** \brief    Print the elapsed time from a given time.
 *  \param    start   Starting time
 *  \param    s       Message
 *  \note     To enable this function, define the macro TIME_LOG in Makefile.
 *  \see      current_time
 */
static inline void print_elapsed_time(clock_t start, const char *s)
{
#ifdef TIME_LOG
  clock_t end = clock();
  printf("time(%s)\t%.2f\tsec\n", s, (double)(end - start) / CLOCKS_PER_SEC);
#endif /*TIME_LOG*/
}

#if defined(MISC_LOG) || defined(SIZE_LOG)
  extern char *g_basename;  //!< basename of input file
#endif /*defined(MISC_LOG) || defined(SIZE_LOG)*/

#define MAX_RECURSION_DEPTH (8192) //!< Guard against stack overfow.

#ifdef RECDEPTH_GUARD
#define INC_RECDEPTH(v)                                                                                 \
  do {                                                                                                  \
    if (v > MAX_RECURSION_DEPTH)                                                                       \
      {                                                                                                 \
fprintf(stderr, "error:%s:%d:%s: maximum recursion depth exceeded\n", __FILE__, __LINE__, __func__);  \
  exit(EXIT_FAILURE);                                                                                   \
      }                                                                                                 \
    v++;                                                                                                \
  } while(0)

#define DEC_RECDEPTH(v) \
  do {                  \
    assert(v > 0);      \
    v--;                \
  } while(0)
#else
#define INC_RECDEPTH(v)  do{} while(0)
#define DEC_RECDEPTH(v)  do{} while(0)
#endif /*RECDEPTH_GUARD*/

#define ENSURE_TRUE_WARN(v, msg)                                                      \
  do {                                                                                \
    if (!(v))                                                                         \
      {                                                                               \
        fprintf(stderr, "warning:%s:%d:%s:%s\n", __FILE__, __LINE__, __func__, msg);  \
      }                                                                               \
  } while(0)

#define ENSURE_TRUE(v)                                                      \
  do {                                                                      \
    if (!(v))                                                               \
      {                                                                     \
  fprintf(stderr, "error:%s:%d:%s failed\n", __FILE__, __LINE__, __func__); \
  exit(EXIT_FAILURE);                                                       \
      }                                                                     \
  } while(0)

#define ENSURE_TRUE_MSG(v, msg)                                               \
  do {                                                                        \
    if (!(v))                                                                 \
      {                                                                       \
  fprintf(stderr, "error:%s:%d:%s:%s\n", __FILE__, __LINE__, __func__, msg);  \
  exit(EXIT_FAILURE);                                                         \
      }                                                                       \
  } while(0)


#define ENSURE_SUCCESS(v)                                                   \
  do {                                                                      \
    if ((v) != ST_SUCCESS)                                                  \
      {                                                                     \
  fprintf(stderr, "error:%s:%d:%s failed\n", __FILE__, __LINE__, __func__); \
  exit(EXIT_FAILURE);                                                       \
      }                                                                     \
  } while(0)

#define ENSURE_SUCCESS_MSG(v, msg)                                            \
  do {                                                                        \
    if ((v) != ST_SUCCESS)                                                    \
      {                                                                       \
  fprintf(stderr, "error:%s:%d:%s:%s\n", __FILE__, __LINE__, __func__, msg);  \
  exit(EXIT_FAILURE);                                                         \
      }                                                                       \
  } while(0)


#endif /*MY_DEF_H*/
