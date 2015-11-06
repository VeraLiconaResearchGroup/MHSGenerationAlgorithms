/** \file     my_hash.c
 *  \brief    A simple implementation of a chained-hash table (without delete operation)
 *  \author   Takahisa Toda
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "my_def.h"
#include "my_hash.h"


#define DEFAULT_HASH_SIZE         (UINTMAX_C(1) << 24)  //!< The default number of buckets
#define HASH_ENLARGEMENT_WIDTH    (2)                   //!< the width of bitshift for enlargement
#define HASHCONST                 (31415926525)


/** \brief    Create a hash table.
 *  \param    n     The number of buckets to be created
 *  \return   Pointer to a hash table if successful; otherwise, NULL.
 *  \note     Set a default value to n if n == 0. 
 *  \see      ht_destroy
 */
my_hash *ht_create(uintmax_t n)
{
  n = (n == 0? DEFAULT_HASH_SIZE: n);

  my_hash *h = (my_hash*) malloc(sizeof(my_hash));
  ENSURE_TRUE_MSG(h != NULL,"memory allocation failed");

  h->bucket_count = n;
  h->entry_count  = 0;
  h->table        = (struct ht_entry**) malloc(sizeof(struct ht_entry*) * n);
  ENSURE_TRUE_MSG(h->table != NULL, "memory allocation failed");
  for(uintmax_t i = 0; i < n; i++) h->table[i] = NULL;

  return h;
}


/** \brief    Free memory area used by a hash table.
 *  \param    h     Pointer to a hash table
 *  \see      ht_create
 */
void ht_destroy(my_hash *h)
{
  assert(h != NULL);
  const uintmax_t n = h->bucket_count;
  for(uintmax_t i = 0; i < n; i++) {
      for(struct ht_entry *e = h->table[i]; e != NULL; ) {
        struct ht_entry *t = e->nx;
        free(e);
        e = t;
      }
  }
  free(h->table); h->table  = NULL;
  free(h);        h         = NULL;
}

/** \brief    Calculate a hash value.
 *  \param    k     Key (of pointer size)
 *  \param    n     The number of buckets
 *  \return   The calculated hash value
 */
static inline uintmax_t ht_hash(uintptr_t k, uintmax_t n)
{
  assert(n > 0);
  return (uintmax_t)(HASHCONST * (k >> 3))%n;
}

/** \brief    Insert a new entry to the head of a linked list.
 *  \param    k     Key (of pointer size)
 *  \param    v     Value associated with key (of pointer size)
 *  \param    h     Pointer to a hash table
 *  \see      ht_search
 */
void ht_insert(uintptr_t k, uintptr_t v, my_hash *h)
{
  assert(h != NULL);
  struct ht_entry *e = (struct ht_entry*)malloc(sizeof(struct ht_entry));
  ENSURE_TRUE_MSG(e != NULL, "memory allocation failed");
  e->key = k;
  e->val = v;

  const uintmax_t i = ht_hash(k, h->bucket_count);
  e->nx             = h->table[i];
  h->table[i]       = e;

  h->entry_count++;
}


/** \brief    Find a hash entry. If found, copy the associated value to the location pointed to by pv.
 *  \param    k     key (of pointer size)
 *  \param    pv    Pointer to a variable, in which the associated value (of pointer size) is copied.
 *  \param    h     Pointer to a hash table
 *  \return   true if found; otherwise, false.
 *  \see      ht_insert
 *  \note
 *  - Value will not be copied if pd == NULL.
 */
bool ht_search(uintptr_t k, uintptr_t *pv, const my_hash *h)
{
  assert(h != NULL);
  const uintmax_t i = ht_hash(k, h->bucket_count);
  for(struct ht_entry *e = h->table[i]; e != NULL; e = e->nx) {
    if(e->key == k) {
      if(pv != NULL) *pv = e->val;
      return true;
    }
  }
  return false;
}


/** \brief   Enlarge a hash table.
 *  \param   h     Pointer to a hash table
 *  \see     ht_create, ht_destroy
 */
void ht_enlarge(my_hash *h)
{
  const uintmax_t oldcnt = h->bucket_count;
  ENSURE_TRUE_MSG(oldcnt < (UINTMAX_MAX >> HASH_ENLARGEMENT_WIDTH), "bucket count overflow");
  const uintmax_t newcnt = oldcnt << HASH_ENLARGEMENT_WIDTH;
#ifdef HT_LOG
  printf("hashtable_resizing\t%ju\t%ju\n", oldcnt, newcnt);
#endif /*HT_LOG*/
  struct ht_entry **oldtable = h->table;
  struct ht_entry **newtable = (struct ht_entry**) malloc(sizeof(struct ht_entry*) * newcnt);
  ENSURE_TRUE_MSG(newtable != NULL, "memory allocation failed");
  for(uintmax_t i = 0; i < newcnt; i++) newtable[i] = NULL;
  for(uintmax_t i = 0; i < oldcnt; i++) {
    for(struct ht_entry *e = oldtable[i]; e != NULL;) {
      struct ht_entry *t  = e->nx;
      const uintmax_t j   = ht_hash(e->key, newcnt);
      e->nx       = newtable[j];
      newtable[j] = e;      
      e           = t;
    }
  }
  free(oldtable); oldtable = NULL;
  h->bucket_count = newcnt;
  h->table        = newtable;
}
