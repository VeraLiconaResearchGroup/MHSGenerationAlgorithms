/** \file     bdd_min.c
 *  \brief    Implementation of min operation
 *  \author   Takahisa Toda
 */

#include "my_def.h"
#include "my_hash.h"
#include "bdd_interface.h"
#include "bdd_min.h"

static zddp min_b2z_rec(bddp f, my_hash *h);

#ifdef SIZE_LOG
static FILE       *sizelog = NULL;
static uintmax_t  maxsize  = 0; //!< maximum size of ZDDs
#endif /*SIZE_LOG*/

static uintmax_t recdepth = 0; //!< recursion depth

/** \brief    Construct the ZDD that represents all minimal sets from a BDD, where the Boolean function represented by an input BDD must be monotone.
 *  \param    f     BDD
 *  \return   the contructed ZDD.
 *  \see      min_b2z_rec
 *  \note     pp.255-256 in "The Art of Computer Programming, Volume 4a (2011)" by Knuth
 */
zddp min_b2z(bddp f)
{
  my_hash *h = ht_create(0);
  ENSURE_TRUE_MSG(h != NULL, "hash table creation failed");

#ifdef SIZE_LOG
  char str[BUFSIZ];
  snprintf(str, BUFSIZ, "%s-min.log", g_basename);
  sizelog = fopen(str, "w");
  ENSURE_TRUE_MSG(sizelog != NULL, "file open failed");
  maxsize = 0;
#endif /*SIZE_LOG*/

  assert(recdepth == 0);
  zddp r = min_b2z_rec(f, h);
  ENSURE_TRUE(r != BDD_NULL);
  assert(recdepth == 0);

#ifdef SIZE_LOG
  fclose(sizelog);
  printf("max|zdd|\t%ju\n", maxsize);
#endif /*SIZE_LOG*/

  ht_destroy(h); 
  return r;
}


static zddp min_b2z_rec(bddp f, my_hash *h)
{
  if(f == bdd_bot()) return zdd_bot();
  if(f == bdd_top()) return zdd_top();

  zddp r;
  if(ht_search((uintptr_t)f, (uintptr_t*)&r, h)) return r;

  INC_RECDEPTH(recdepth);  
  zddp m1 = min_b2z_rec(bdd_hi(f), h);
  zddp m0 = min_b2z_rec(bdd_lo(f), h);
  DEC_RECDEPTH(recdepth);

  zddp t = zdd_diff(m1, m0);
  ENSURE_TRUE_MSG(t != BDD_NULL, "ZDD operation failed");
  r = zdd_node(bdd_itemval(f), m0, t);
  ENSURE_TRUE_MSG(r != BDD_NULL, "ZDD operation failed");

  ht_insert((uintptr_t)f, (uintptr_t)r, h);

#ifdef SIZE_LOG
  const uintmax_t insize  = bdd_size(f);
  const uintmax_t outsize = zdd_size(r);
  fprintf(sizelog, "%ju\t%ju\n", insize, outsize);
  if(maxsize < outsize) maxsize = outsize;
#endif /*SIZE_LOG*/

  return r;
}

