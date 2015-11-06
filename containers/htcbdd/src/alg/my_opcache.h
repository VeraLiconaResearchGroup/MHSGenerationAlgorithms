/** \file  my_opcache.h
 *  \brief  A cache for binary operations
 *  \author  Takahisa Toda
 */
#ifndef MY_OPCACHE_H
#define MY_OPCACHE_H

#include <stdint.h>
#include <stdbool.h>


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/** \brief  my opcache */
typedef struct {
  uintmax_t entry_count;      //!< the number of stored entries
  uintmax_t max_entry_count;  //!< the maximum number of entries that can be stored.
  uintmax_t memlen;           //!< the length of mem
#ifdef OPCACHE_LOG
  uintmax_t hit_count;        //!< the number of hits
  uintmax_t overwrite_count;  //!< the number of overwrites
  uintmax_t search_count;     //!< the number of search calls
  uintmax_t insert_count;     //!< the number of insert calls
#endif /*OPCACHE_LOG*/
  uintptr_t *mem;             //!< memory area (array) of cache entries
} my_opcache;

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/
extern my_opcache *opcache_create    (uintmax_t n);
extern void       opcache_destroy    (my_opcache *ca);
extern void       opcache_insert     (int op, uintptr_t k1, uintptr_t k2, uintptr_t r,   my_opcache *ca);
extern bool       opcache_search     (int op, uintptr_t k1, uintptr_t k2, uintptr_t *pr, my_opcache *ca);
extern void       opcache_enlarge    (my_opcache *ca);
extern double     opcache_loadfactor (const my_opcache *ca);
extern void       opcache_printstat  (const my_opcache *ca);


#endif /*MY_OPCACHE_H*/
