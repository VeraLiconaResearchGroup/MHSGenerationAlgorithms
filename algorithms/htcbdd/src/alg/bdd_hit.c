/** \file     bdd_hit.c
 *  \brief    Implementation of hitting set generation
 *  \author   Takahisa Toda
 *  \see      T.Toda, Hypergraph Transversal Computation with Binary Decision Diagrams, in Proc. of 12th International Symposium on Experimental Algorithms (SEA2013), LNCS 7933, pp.91-102, Rome, Italy (2013). 
 */

#include "my_def.h"
#include "my_hash.h"
#include "bdd_interface.h"
#include "bdd_hit.h"

static bddp hit_z2b_rec(zddp f, my_hash *h);
static zddp minhit_nonsup_rec(zddp f, my_hash *h);

#ifdef SIZE_LOG
static FILE       *sizelog  = NULL;
static uintmax_t  maxsize   = 0; //!< the maximum size of intermediate BDDs
#endif /*SIZE_LOG*/
 
static uintmax_t recdepth = 0; //!< recursion depth

/** \brief    Construct the BDD that represent all hitting sets from a ZDD.
 *  \param    f     ZDD
 *  \return   The contructed BDD
 *  \see      hit_z2b_rec
 */
bddp hit_z2b(zddp f)
{
  my_hash *h = ht_create(0);
  ENSURE_TRUE_MSG(h != NULL, "hash table creation failed");

#ifdef SIZE_LOG
  char str[BUFSIZ];
  snprintf(str, BUFSIZ, "%s-hit.log", g_basename);
  sizelog = fopen(str, "w");
  ENSURE_TRUE_MSG(sizelog != NULL, "file open failed");
  maxsize = 0;
#endif /*SIZE_LOG*/

  assert(recdepth == 0);
  bddp r = hit_z2b_rec(f, h);
  ENSURE_TRUE(r != BDD_NULL);
  assert(recdepth == 0);

#ifdef SIZE_LOG
  fclose(sizelog);
  printf("max|bdd|\t%ju\n", maxsize);
#endif /*SIZE_LOG*/

  ht_destroy(h); 
  return r;
}


static bddp hit_z2b_rec(zddp f, my_hash *h)
{
  if(f == zdd_top()) return bdd_bot();
  if(f == zdd_bot()) return bdd_top();

  bddp r;
  if(ht_search((uintptr_t)f, (uintptr_t*)&r, h)) return r;

  INC_RECDEPTH(recdepth);  
  bddp r0 = hit_z2b_rec(zdd_lo(f), h);
  bddp r1 = hit_z2b_rec(zdd_hi(f), h);
  DEC_RECDEPTH(recdepth);

  bddp t = bdd_node(zdd_itemval(f), r1, bdd_top());
  ENSURE_TRUE_MSG(t != BDD_NULL, "BDD operation failed");
  r = bdd_and(r0, t);
  ENSURE_TRUE_MSG(r != BDD_NULL, "BDD operation failed");

  ht_insert((uintptr_t)f, (uintptr_t)r, h);

#ifdef SIZE_LOG
  const uintmax_t insize  = zdd_size(f);
  const uintmax_t outsize = bdd_size(r);
  fprintf(sizelog, "%ju\t%ju\n", insize, outsize);
  if(maxsize < outsize) maxsize = outsize;
#endif /*SIZE_LOG*/

  return r;
}



/** \brief    Construct the ZDD that represents all minimal hitting sets for a ZDD.
 *  \param    f     ZDD
 *  \return   The contructed ZDD
 *  \see      minhit_nonsup_rec, nonsup
 *  \note     pp.669-670 in "The Art of Computer Programming, Volume 4a (2011)" by Knuth
 */
zddp minhit_nonsup(zddp f)
{
  my_hash *h = ht_create(0);
  ENSURE_TRUE_MSG(h != NULL, "hash table creation failed");

  assert(recdepth == 0);
  zddp r = minhit_nonsup_rec(f, h);
  ENSURE_TRUE(r != BDD_NULL);
  assert(recdepth == 0);

  ht_destroy(h);
  return r;
}


static zddp minhit_nonsup_rec(zddp f, my_hash *h)
{
  if(f == zdd_bot()) return zdd_top();
  if(f == zdd_top()) return zdd_bot();

  zddp r; 
  if(ht_search((uintptr_t)f, (uintptr_t*)&r, h)) return r;

  zddp t  = zdd_union(zdd_lo(f), zdd_hi(f));
  ENSURE_TRUE_MSG(t != BDD_NULL, "ZDD operation failed");

  INC_RECDEPTH(recdepth);  
  zddp r0 = minhit_nonsup_rec(t, h);
  zddp tt = minhit_nonsup_rec(zdd_lo(f), h);
  DEC_RECDEPTH(recdepth);

  zddp r1 = nonsup(tt, r0);
  ENSURE_TRUE_MSG(r1 != BDD_NULL, "ZDD extra operation failed");
  r = zdd_node(zdd_itemval(f), r0, r1);
  ENSURE_TRUE_MSG(r  != BDD_NULL, "ZDD operation failed");

  ht_insert((uintptr_t)f, (uintptr_t)r, h);
  return r;
}

