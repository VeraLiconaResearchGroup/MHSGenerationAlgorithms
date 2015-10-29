/** \file     my_hash.h
 *  \brief    A chained-hash table (without delete operation)
 *  \author   Takahisa Toda
 */
#ifndef MY_HASH_H
#define MY_HASH_H

#include <stdint.h>
#include <stdbool.h>

/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/

/** \brief    Key-value pair */
struct ht_entry {
  uintptr_t         key; 
  uintptr_t         val;
  struct  ht_entry  *nx;
};

/** \brief    Hash table (chaining)*/
typedef struct {
  uintmax_t         bucket_count;   //!< the number of buckets
  uintmax_t         entry_count;    //!< the number of hash table entries
  struct ht_entry   **table;        //!< an array of buckets
} my_hash;


/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

extern my_hash    *ht_create      (uintmax_t n);
extern void       ht_destroy      (my_hash *h);
extern void       ht_insert       (uintptr_t k, uintptr_t v, my_hash *h);
extern bool       ht_search       (uintptr_t k, uintptr_t *pv, const my_hash *h);
extern void       ht_enlarge      (my_hash *h);


/** \brief    Compute a load factor.
 *  \param    h     Pointer to a hash table
 *  \return   The computed load factor
 */
static inline double ht_loadfactor(const my_hash *h)
{
  return (double)h->entry_count/h->bucket_count;
}

#endif /*MY_HASH_H*/
