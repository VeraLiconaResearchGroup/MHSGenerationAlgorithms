/** \file     bdd_extra.c
 *  \brief    Implementation of BDD/ZDD extra operations
 *  \author   Takahisa Toda
 */

#include "my_def.h"
#include "my_opcache.h"
#include "bdd_interface.h"

static my_opcache *bdd_ex_opcache = NULL;

enum {
  BDD_EX_NONSUP_OP = 0,
};

static uintmax_t recdepth = 0; //!< recursion depth

/** \brief    Initialize BDD/ZDD extra operations.
 *  \param    n     The number of cache entries for binary operations
 *  \return   ST_SUCCESS if successful; otherwise, ST_FAILURE.
 */
int bdd_ex_init(uintmax_t n)
{
  if(bdd_ex_opcache == NULL) {
    bdd_ex_opcache = opcache_create(n);
    ENSURE_TRUE_MSG(bdd_ex_opcache != NULL, "cache creation failed");
    return ST_SUCCESS;
  } else {
    ENSURE_TRUE_WARN(false, "Initialization for extra operations already done");
    return ST_FAILURE;
  }
}

/** \brief    Destroy memory for BDD/ZDD extra operations
 *  \return   ST_SUCCESS if successful; otherwise, ST_FAILURE.
 */
int bdd_ex_quit(void)
{
  if(bdd_ex_opcache != NULL) {
    opcache_destroy(bdd_ex_opcache);
    bdd_ex_opcache = NULL;
    return ST_SUCCESS;
  } else {
    ENSURE_TRUE_WARN(false, "memory for extra operations already destroyed");
    return ST_FAILURE;
  }
}

/** \brief    Construct the ZDD that represents all sets U such that U in f but U is not superset for any set in g.
 * \param     f   ZDD
 * \param     g   ZDD
 * \return    The ZDD contructed
 * \see       pp.669-670 in "The Art of Computer Programming, Volume 4a (2011)" by Knuth
 */
zddp nonsup(zddp f, zddp g)
{
  if(g == zdd_bot())                                    return f;
  if(f == zdd_bot() || g == zdd_top() || f == g)        return zdd_bot();
  if(f == zdd_top() || zdd_itemval(f) > zdd_itemval(g)) {
     INC_RECDEPTH(recdepth);  
    zddp r = nonsup(f, zdd_lo(g));
    INC_RECDEPTH(recdepth);
    return r;
  }

  zddp r;
  if(opcache_search(BDD_EX_NONSUP_OP,
                    (uintptr_t)f,
                    (uintptr_t)g,
                    (uintptr_t*)&r,
                    bdd_ex_opcache))      return r;

  itemval v = zdd_itemval(f);
  zddp r0; zddp r1;

  INC_RECDEPTH(recdepth);  
  if(v < zdd_itemval(g)) {
    r0 = nonsup(zdd_lo(f), g);
    r1 = nonsup(zdd_hi(f), g);
  } else {
    r0 = nonsup(zdd_hi(f), zdd_hi(g));
    zddp t = nonsup(zdd_hi(f), zdd_lo(g));
    r1 = zdd_intersect(t, r0);
    ENSURE_TRUE_MSG(r1 != BDD_NULL, "ZDD operation failed");
    r0 = nonsup(zdd_lo(f), zdd_lo(g));
  }
  DEC_RECDEPTH(recdepth);

  r = zdd_node(v, r0, r1);
  ENSURE_TRUE_MSG(r != BDD_NULL, "ZDD operation failed");

  opcache_insert(BDD_EX_NONSUP_OP,
                (uintptr_t)f,
                (uintptr_t)g,
                (uintptr_t)r,
                bdd_ex_opcache);
  return r;
}


