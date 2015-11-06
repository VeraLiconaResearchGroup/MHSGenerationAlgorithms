/** \file     my_opcache.c
 *  \brief    Implementation of a simple cache for binary operations
 *  \author   Takahisa Toda
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "my_def.h"
#include "my_opcache.h"

#define DEFAULT_OPCACHE_SIZE        (UINTMAX_C(1) << 24)  //!< The default number of cache entries
#define OPCACHE_ENLARGEMENT_WIDTH   (1)                   //!< the width of bitshift for enlargement
#define HASHCONST                   (31415926525)

enum {
  POS_OPID = 0,
  POS_ARG1,
  POS_ARG2,
  POS_RES,
  OPCACHE_ENTRY_SIZE, /* Insert a new field before OPCACHE_ENTRY_SIZE.*/
};

/** \brief    Convert a cache entry index to its position in mem.
 *  \param    i     The index of a cache entry
 *  \return   A position of a cache entry in mem.
 */
static inline uintmax_t opcache_pos(uintmax_t i)
{
  return i * OPCACHE_ENTRY_SIZE;
}


/** \brief    Convert a position of mem into the corresponding field
 *  \param    pos     Position of mem
 *  \return   A field of a cache entry.
 */
static inline uintmax_t opcache_field(uintmax_t pos)
{
  return pos % OPCACHE_ENTRY_SIZE;
}

/** \brief    Create an operation cache.
 *  \param    n     The maximum number of cache entries that can be stored.
 *  \return   A pointer to a cache if successful; otherwise, NULL.
 *  \see      opcache_destroy, opcache_enlarge
 *  \note     Set a default value to n if n == 0.
 */
my_opcache *opcache_create(uintmax_t n)
{
  n = (n == 0? DEFAULT_OPCACHE_SIZE: n);

  my_opcache *ca = (my_opcache*) malloc(sizeof(my_opcache));
  ENSURE_TRUE_MSG(ca != NULL,"memory allocation failed");

  ca->max_entry_count = n;
  ca->entry_count     = 0;
  ca->memlen          = opcache_pos(n);
  const uintmax_t len = ca->memlen;
  ca->mem = (uintptr_t*) malloc(sizeof(uintptr_t) * len);
  ENSURE_TRUE_MSG(ca->mem != NULL, "memory allocation failed");
  for(uintmax_t i = 0; i < len; i++) {
    ca->mem[i] = ((opcache_field(i) == POS_OPID)? 1:0); //<! odd: empty, even: non-empty
  }

#ifdef OPCACHE_LOG
  ca->hit_count       = 0;
  ca->overwrite_count = 0;
  ca->search_count    = 0;
  ca->insert_count    = 0;
#endif /*OPCACHE_LOG*/

  return ca;
}


/** \brief    Free memory area used by a cache.
 *  \param    ca      Pointer to a cache
 *  \see      opcache_create, opcache_enlarge
 */
void opcache_destroy(my_opcache *ca)
{
  ENSURE_TRUE(ca != NULL);
  free(ca->mem);  ca->mem = NULL;
  free(ca);       ca      = NULL;
}

/** \brief    Compute the load factor of an operation cache.
 *  \param    ca    Cache
 *  \return   The computed load factor
 */
 double opcache_loadfactor(const my_opcache *ca)
{
  return (double)ca->entry_count/ca->max_entry_count;
}

/** \brief    Calculate a hash value.
 *  \param    id    Operation id (>=0)
 *  \param    k1    The first operand  (of poitner size)
 *  \param    k2    The second operand (of pointer size)
 *  \param    n     The total number of cache entries
 *  \return   The calculated hash value
 */
static inline uintmax_t opcache_hash(int id, uintptr_t k1, uintptr_t k2, uintmax_t n)
{
  assert(n > 0);
  uintptr_t hash = 0;
  hash = hash * HASHCONST + (uintptr_t)(id >> 1);
  hash = hash * HASHCONST + (uintptr_t)(k1 >> 3);
  hash = hash * HASHCONST + (uintptr_t)(k2 >> 3);
  return (uintmax_t)(hash%n);
}

/** \brief    Insert a new entry.
 *  \param    id    Operation id (>= 0)
 *  \param    k1    The first operand   (of pointer size)
 *  \param    k2    The second operand  (of pointer size)
 *  \param    r     An operation result (of pointer size)
 *  \see      opcache_search
 *  \note     Overwrite if the position is already filled.
 */
void opcache_insert(int id, uintptr_t k1, uintptr_t k2, uintptr_t r, my_opcache *ca)
{
#ifdef OPCACHE_LOG
  ca->insert_count++;
#endif /*OPCACHE_LOG*/
  assert(id >= 0); assert(id < (INT_MAX >> 1));
  id = (id << 1);
  const uintmax_t i = opcache_pos(opcache_hash(id, k1, k2, ca->max_entry_count));
  if(ca->mem[i + POS_OPID] & 1) ca->entry_count++;
#ifdef OPCACHE_LOG
  if(!(ca->mem[i + POS_OPID] & 1)) ca->overwrite_count++;
#endif /*OPCACHE_LOG*/
  ca->mem[i + POS_OPID] =  id;
  ca->mem[i + POS_ARG1] =  k1;
  ca->mem[i + POS_ARG2] =  k2;
  ca->mem[i + POS_RES ] =   r;
}


/** \brief    Find a cache entry. If found, copy the operation result to pr.
 *  \param    id    Operation id (>= 0)
 *  \param    k1    The first operand  (of pointer size)
 *  \param    k2    The second operand (of pointer size)
 *  \param    pr    A pointer to an operation result (of pointer size)
 *  \param    ca    Pointer to a cache
 *  \return   true if found; otherwise, false.
 *  \see      opcache_insert
 *  \note
 *  - Result is not copied if pr == NULL.
 */
bool opcache_search(int id, uintptr_t k1, uintptr_t k2, uintptr_t *pr, my_opcache *ca)
{
#ifdef OPCACHE_LOG
  ca->search_count++;
#endif /*OPCACHE_LOG*/
  assert(id >= 0); assert(id < (INT_MAX >> 1));
  id = (id << 1);
  const uintmax_t i = opcache_pos(opcache_hash(id, k1, k2, ca->max_entry_count));
  if(   ca->mem[i + POS_OPID] == id
     && ca->mem[i + POS_ARG1] == k1 
     && ca->mem[i + POS_ARG2] == k2) {
    if(pr != NULL) *pr = ca->mem[i + POS_RES];
#ifdef OPCACHE_LOG
    ca->hit_count++;
#endif /*OPCACHE_LOG*/
    return true;
  } else {return false;}
}


/** \brief     Enlarge cache size.
 *  \param     ca      A pointer to a cache
 *  \note      To enable this function, define the macro OPCACHE_LOG in Makefile.
 *  \see       opcache_create, opcache_destroy
 */
void opcache_enlarge(my_opcache *ca)
{
#ifdef OPCACHE_LOG 
  const double    oldlf  = opcache_loadfactor(ca);
#endif /*OPCACHE_LOG*/
  const uintmax_t oldcnt = ca->max_entry_count;
  ENSURE_TRUE_MSG(oldcnt < (UINTMAX_MAX >> OPCACHE_ENLARGEMENT_WIDTH), "cache entry count overflow");
  const uintmax_t newcnt = oldcnt << OPCACHE_ENLARGEMENT_WIDTH;
  const uintmax_t newlen = opcache_pos(newcnt);
#ifdef OPCACHE_LOG
  printf("opcache_resizing\t%ju\t%ju\n", oldcnt, newcnt);
#endif /*OPCACHE_LOG*/
  uintptr_t *oldmem = ca->mem;
  uintptr_t *newmem = (uintptr_t*) malloc(sizeof(uintptr_t) * newlen);
  ENSURE_TRUE_MSG(newmem != NULL, "memory allocation failed");
  for(uintmax_t i = 0; i < newlen; i++) {
    newmem[i] = ((opcache_field(i) == POS_OPID)? 1:0); //<! odd: empty, even: non-empty
  }
  for(uintmax_t i = 0; i < oldcnt; i++) {
    const uintmax_t j = opcache_pos(i);
    if(!(oldmem[j+POS_OPID] & 1)) {
      const uintmax_t k = opcache_pos(opcache_hash(oldmem[j+POS_OPID], 
                                                   oldmem[j+POS_ARG1], 
                                                   oldmem[j+POS_ARG2], 
                                                   newcnt));
      if(!(newmem[k+POS_OPID] & 1)) {assert(ca->entry_count > 0); ca->entry_count--;}
      newmem[k+POS_OPID] = oldmem[j+POS_OPID];
      newmem[k+POS_ARG1] = oldmem[j+POS_ARG1];
      newmem[k+POS_ARG2] = oldmem[j+POS_ARG2];
      newmem[k+POS_RES]  = oldmem[j+POS_RES];
    }
  }
  free(oldmem); oldmem = NULL;
  ca->max_entry_count  = newcnt;
  ca->memlen           = opcache_pos(newcnt);
  ca->mem              = newmem;

#ifdef OPCACHE_LOG 
  const double newlf  = opcache_loadfactor(ca);
  printf("opcache_loadfactor\t%.2f\t%.2f\n", oldlf, newlf);
  ca->hit_count        = 0;
  ca->overwrite_count  = 0;
  ca->search_count     = 0;
  ca->insert_count     = 0;
#endif /*OPCACHE_LOG*/
}


void opcache_printstat(const my_opcache *ca)
{
#ifdef OPCACHE_LOG
  if(ca->search_count != 0) printf("opcache_hit_rate\t%.2f\n", (double)ca->hit_count/ca->search_count);
  if(ca->insert_count != 0) printf("opcache_overwrite_rate\t%.2f\n", (double)ca->overwrite_count/ca->insert_count);
#endif /*OPCACHE_LOG*/
}
