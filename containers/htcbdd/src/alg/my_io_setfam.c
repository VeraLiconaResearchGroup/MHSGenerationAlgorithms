/** \file   my_io_setfam.c
 *  \brief  Implementation of IO functions for data files that represent set families.
 *  \author Takahisa Toda
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "my_def.h"
#include "my_io_setfam.h"

/** \brief      Obtain information of a data file, and check consistency of file format.
 *  \param      prow_count    Pointer to the number of rows, including empty rows.
 *  \param      pmaxval       Pointer to the maximum value of entries in a datafile
 *  \param      pentry_count  Pointer to the number of entries with repetition in a datafile
 *  \param      in            Pointer to an input datafile
 *  \return     ST_SUCCESS if successful; otherwise, ST_FAILURE.
 *  \see        create_setfam
 */
int getfileinfo(uintmax_t *prow_count, itemval *pmaxval, uintmax_t *pentry_count, FILE *in)
{
  fseek(in, 0L, SEEK_SET);

  itemval   k = 0;
  itemval   m = 0;
  uintmax_t n = 0;
  char buf[MAX_ROWLEN];

  while (fgets(buf, MAX_ROWLEN, in) != NULL) {
    itemval prev = 0;
    for(char *sp = buf; *sp != '\n' && *sp != '\0';) {
      while(*sp == SF_DELIMITER) {assert(sp+1 < buf+MAX_ROWLEN); sp++;}
      char *ep = sp; while(*ep != SF_DELIMITER && *ep != '\n' && *ep != '\0') {
        assert(ep+1 < buf+MAX_ROWLEN);
        ENSURE_TRUE_MSG(isdigit(*ep), "invalid character");
	      ep++;
      }
      if(*ep == SF_DELIMITER) {assert(ep < buf+MAX_ROWLEN); *ep = '\0'; ep++;}
      if(sp != ep) {
        itemval v = strtoull(sp, NULL, 10); n++;
        ENSURE_TRUE_MSG(0 <= v && v < INFTY, "invalid value");
        ENSURE_TRUE_MSG(prev < v,           "entries not sorted");
        if(m < v) m = v;
	prev = v;
      }
      sp  = ep;
    }
    k++;
  }

  *prow_count   = k;
  *pmaxval      = m;
  *pentry_count = n;

#ifdef MISC_LOG
  printf("maxval\t%jd\n", m);
  printf("#row\t%ju\n",   k);
  printf("#entry\t%ju\n", n);
  printf("\n");
  fflush(stdout);
#endif /*MISC_LOG*/
  return ST_SUCCESS;
}


/** \brief    Read an input datafile, and conver it to a setfam data representation.
 *  \param    row_count     The number of rows in a datafile
 *  \param    entry_count   The number of entries with repetition in a datafile
 *  \param    in            Pointer to a dafafile
 *  \note
 *  - sf->elem[i-1][j-1] corresponds to the j-th entry in the i-th set.
 *  - sf->elem[i-1][k] == INFTY if the i-th set contains exactly k elements.
 *  - The returned data structure must be destroyed when it becomes unnecessary.
 *  \see      getfileinfo, destroy_setfam
 */
struct setfam *create_setfam(uintmax_t row_count, uintmax_t entry_count, FILE *in)
{
  fseek(in, 0L, SEEK_SET);
  char buf[MAX_ROWLEN];

  // Create setfam.
  const uintmax_t len = row_count + entry_count;
  struct setfam   *sf = (struct setfam*)malloc(sizeof(struct setfam));
  ENSURE_TRUE_MSG(sf != NULL, "memory allocation failed");
  sf->elem  = (itemval**)malloc(sizeof(itemval*)*row_count);
  ENSURE_TRUE_MSG(sf->elem  != NULL, "memory allocation failed");
  sf->mem   = (itemval*)malloc(sizeof(itemval)*len);
  ENSURE_TRUE_MSG(sf->mem   != NULL, "memory allocation failed");
  sf->card    = row_count;
  sf->memlen  = len;

  // Convert to setfam.
  itemval **e = sf->elem; itemval *m = sf->mem;
  while (fgets(buf, MAX_ROWLEN, in) != NULL) {
    *e = m; e++;
    for(char *sp = buf; *sp != '\n' && *sp != '\0';) {
      while(*sp == SF_DELIMITER) sp++;
      char *ep = sp; while(*ep != SF_DELIMITER && *ep != '\n' && *ep != '\0') ep++;
      if(*ep == SF_DELIMITER) {*ep = '\0'; ep++;}
      if( sp != ep) {
        *m = (itemval)strtoll(sp, NULL, 10); m++;
      }
      sp = ep;
    }
    *m = INFTY; m++;
  }


  return sf;
}

/** \brief    Free all memory areas used.
 *  \param    sf    Pointer to a setfam
 *  \see      create_setfam
 */
void destroy_setfam(struct setfam *sf)
{
  assert(sf != NULL);

  free(sf->elem); sf->elem  = NULL;
  free(sf->mem);  sf->mem   = NULL;
  free(sf);       sf        = NULL;
}
