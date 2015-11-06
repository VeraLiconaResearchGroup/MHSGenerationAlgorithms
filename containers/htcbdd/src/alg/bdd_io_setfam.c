/** \file   bdd_io_setfam.c
 *  \brief  Implementation of IO functions for data files that represent set families.
 *  \author Takahisa Toda
 *  \note   T.Toda, Fast Compression of Large-scale Hypergraphs for Solving Combinatorial Problems, in Proc. of Sixteenth International Conference on Discovery Science (DS2013), LNAI 8140, LNAI 8140, pp. 281--293, Singapore (2013).
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "my_def.h"
#include "ssort1.h"
#include "bdd_interface.h"
#include "my_io_setfam.h"
#include "bdd_io_setfam.h"

static zddp             zcomp_rec       (itemval **s, uintmax_t n, uintmax_t d);
static inline uintmax_t bsearch_itemval (itemval **s, uintmax_t left, uintmax_t right, itemval v, uintmax_t d);

static uintmax_t recdepth = 0; //!< recursion depth

/** \brief    Compress an input datafile into a ZDD.
 *  \param    row_count     The number of rows in a data file
 *  \param    entry_count   The total number of entries in a data file
 *  \param    in            Pointer to a dafa file
 *  \return   The built ZDD
 *  \see      unzcomp 
 */
zddp zcomp(uintmax_t row_count, uintmax_t entry_count, FILE *in)
{
  struct setfam *sf = create_setfam(row_count, entry_count, in);
  ENSURE_TRUE(sf != NULL);
  ssort1main(sf->elem, row_count);

  assert(recdepth == 0);
  zddp f = zcomp_rec(sf->elem, row_count, 0);
  ENSURE_TRUE(f != BDD_NULL);
  assert(recdepth == 0);

  destroy_setfam(sf);
  return f;
}

/** \brief    Compress a set family into a ZDD.
 *  \param    s     s[i] corresponds to the i-th set of a given set family.
 *  \param    n     The number of sets
 *  \param    d     The position of an element in a set to be examined.
 *  \return   The computed ZDD
 */
static zddp zcomp_rec(itemval **s, uintmax_t n, uintmax_t d)
{ 
  uintmax_t pos; itemval v; zddp lo, hi;
  if (n == 0)     return zdd_bot();
  v = s[0][d];
  if (v == INFTY) return zdd_top();
  INC_RECDEPTH(recdepth);  
  if(s[n-1][d] == v) {pos = n;}
  else {
    for(pos = 1; pos < n && s[pos][d] == v; pos *= 2) ;
    if(pos < n) pos = bsearch_itemval(s, pos/2, pos, v, d);
    else        pos = bsearch_itemval(s, pos/2, n-1, v, d);
  }
  hi = zcomp_rec(s,     pos,   d+1);
  lo = zcomp_rec(s+pos, n-pos, d  );
  zddp f = zdd_node(v, lo, hi);
  ENSURE_TRUE_MSG(f != BDD_NULL, "ZDD operation failed");
  DEC_RECDEPTH(recdepth);
  return f;
}


/** \brief    Find the first position with d-th value not equal to v (by a binary search).
 * \param     s     s[i] corresponds to the i-th set of a set family
 * \param     l     The leftend position
 * \param     r     The rightend position
 * \param     v     Value
 * \param     d     The position of rows to be examined.
 * \return    The found position
 */
static inline uintmax_t bsearch_itemval(itemval **s, uintmax_t l, uintmax_t r, itemval v, uintmax_t d)
{
  while(1) {
    assert(s[l][d] == v); assert(s[r][d] != v); assert(l < r);
    if(r - l <= 1)      return r;
    uintmax_t mid = (l + r) / 2;
    if(s[mid][d] == v)  l = mid;
    else                r = mid;
  }
}


/** \brief    Export ZDD into a datafile.
 *  \param    max   The maximum value of items
 *  \param    f     non-constant ZDD
 *  \param    out   Pointer to a dafafile
 *  \return   ST_SUCCESS if successful; otherwise, ST_FAILURE.
 *  \see      zcomp
 */
int unzcomp(itemval max, zddp f, FILE *out)
{
  if(zdd_isconst(f)) {
    ENSURE_TRUE_WARN(false, "invalid input");
    return ST_FAILURE;
  }
  
  int res = fseek(out, 0L, SEEK_SET);
  ENSURE_SUCCESS(res);

  zddp    *branch = (zddp*)malloc(sizeof(zddp)*(max+1));
  ENSURE_TRUE_MSG(branch != NULL, "memory allocation failed");
  itemval *item   = (itemval*)malloc(sizeof(itemval)*(max+1));
  ENSURE_TRUE_MSG(item != NULL, "memory allocation failed");
  zddp    *bp     = branch;
  itemval *ip     = item;

  while(1) {
    while(f != zdd_top()) {
      zddp f0 = zdd_lo(f);
      zddp f1 = zdd_hi(f);
      fprintf(out, "%ju ", zdd_itemval(f));
      assert(ip <= item+max);
      *ip = zdd_itemval(f); ip++;
      if(f0 != zdd_bot()) {assert(bp <= branch+max); *bp = f; bp++;}
      f = f1;
    }
    fprintf(out, "\n");
    if((bp--) <= branch) break;
    itemval t = zdd_itemval(*bp);
    for(ip = item; *ip < t; ip++) {
      assert(ip <= item+max);
      fprintf(out, "%ju ", *ip);
    }
    f = zdd_lo(*bp);
    assert(f != zdd_bot());
  }
  
  free(branch); free(item);
  branch = NULL; item = NULL;
  return ST_SUCCESS;
}

