/** \file     bdd_interface.h
 *  \brief    Functions to access BDD/ZDD libraries.
 *  \author   Takahisa Toda
 */
#ifndef BDD_INTERFACE_H
#define BDD_INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "my_def.h"

#if defined(SELECT_CUDD)
/*---------------------------------------------------------------------------*/
/* COLORADO BDD/ZDD PACKAGE                                                  */
/*---------------------------------------------------------------------------*/
#include "util.h"
#include "cudd.h"
#include "cuddInt.h"

#define BDD_PACKAGE "CU Decision Diagram Package Release 2.5.0"
#define BDD_NULL  NULL
#define BDD_MAXITEMVAL  CUDD_MAXINDEX //!< maximum value that a BDD library can handle.

typedef DdNode *bddp; //!< pointer to a BDD node
typedef DdNode *zddp; //!< pointer to a ZDD node

extern DdManager *dd_mgr;

/* BDD/ZDD Extra Operations */
extern int        bdd_ex_init     (uintmax_t n);
extern int        bdd_ex_quit     (void);
extern zddp       nonsup          (zddp f, zddp g);

/* Common Operations */
static inline int bdd_init(itemval maxval, uintmax_t n)
{
  if(dd_mgr == NULL) {
    dd_mgr = Cudd_Init((DdHalfWord)maxval, (DdHalfWord)maxval, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, UINTMAX_C(1) << 34);
    //dd_mgr = Cudd_Init((DdHalfWord)maxval, (DdHalfWord)maxval, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
    ENSURE_TRUE_MSG(dd_mgr != NULL, "BDD manager initialization failed.");
    Cudd_DisableGarbageCollection(dd_mgr);// disable GC since we never do dereference of nodes.
#ifdef MISC_LOG
    if(Cudd_GarbageCollectionEnabled(dd_mgr)) printf("GC\tenabled\n");
    else                                      printf("GC\tdisabled\n");    
#endif /*MISC_LOG*/

    bdd_ex_init(n);
    return ST_SUCCESS;
  } else {
    ENSURE_TRUE_WARN(false, "BDD manager already initialized.");
    return ST_FAILURE;
  }
}

static inline int bdd_quit(void)
{
  if(dd_mgr != NULL) {
    bdd_ex_quit();
    Cudd_Quit(dd_mgr);
    dd_mgr = NULL;
    return ST_SUCCESS;
  } else {
    ENSURE_TRUE_WARN(false, "BDD manager does not exist.");
    return ST_FAILURE;
  }
}

/* BDD Operations*/
static inline itemval bdd_itemval(bddp f)
{  
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);

  bddp t = Cudd_Regular(f);
  return (itemval)Cudd_NodeReadIndex(t);
}

static inline bddp bdd_top(void)
{
  assert(dd_mgr != NULL);

  return Cudd_ReadOne(dd_mgr);
}

static inline bddp bdd_bot(void)
{
  assert(dd_mgr != NULL);

  return Cudd_ReadLogicZero(dd_mgr);
}

static inline int bdd_isconst(bddp f)
{
  assert(f != BDD_NULL);

  return (f==bdd_top() || f==bdd_bot());
}

static inline uintmax_t bdd_size(bddp f)
{
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);

  return (uintmax_t)Cudd_DagSize(f);
}

static inline bddp bdd_hi(bddp f)
{
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);
  assert(!bdd_isconst(f));

  bddp t = Cudd_Regular(f);
  if (Cudd_IsComplement(f)) return Cudd_Not(cuddT(t));
  else                      return cuddT(t);
}

static inline bddp bdd_lo(bddp f)
{
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);
  assert(!bdd_isconst(f));

  bddp t = Cudd_Regular(f);
  if (Cudd_IsComplement(f)) return Cudd_Not(cuddE(t));
  else                      return cuddE(t);
}

static inline bddp bdd_and(bddp f, bddp g)
{
  assert(dd_mgr != NULL);
  assert(f != BDD_NULL && g != BDD_NULL);

  return Cudd_bddAnd(dd_mgr, f, g);
}

static inline bddp bdd_node(itemval i, bddp lo, bddp hi)
{
  assert(dd_mgr != NULL);
  assert(!bdd_isconst(hi)? i < bdd_itemval(hi): true);
  assert(!bdd_isconst(lo)? i < bdd_itemval(lo): true);

  bddp f;
  if(lo == hi) {
    f = hi;
  } else {
    if (Cudd_IsComplement(hi)) {
      f = cuddUniqueInter(dd_mgr,(int)i,Cudd_Not(hi),Cudd_Not(lo));
      ENSURE_TRUE_MSG(f != BDD_NULL, "BDD operation failed");
      f = Cudd_Not(f);
    } else {
      f = cuddUniqueInter(dd_mgr,(int)i, hi, lo);
      ENSURE_TRUE_MSG(f != BDD_NULL, "BDD operation failed");
    }
  }
  return f;
}


/* ZDD Operations */
static inline itemval zdd_itemval(zddp f)
{  
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);

  return (itemval)Cudd_NodeReadIndex(f);
}


static inline uintmax_t zdd_size(zddp f)
{
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);

  return (uintmax_t)Cudd_DagSize(f);
}

static inline zddp zdd_bot(void)
{
  assert(dd_mgr != NULL);

  return Cudd_ReadZero(dd_mgr);
}

static inline zddp zdd_top(void)
{
  assert(dd_mgr != NULL);

  return Cudd_ReadOne(dd_mgr);
}

static inline int zdd_isconst(zddp f)
{
  assert(BDD_NULL != f);

  return (f == zdd_top()) || (f == zdd_bot());
}

static inline zddp zdd_hi(zddp f)
{
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);
  assert(!zdd_isconst(f));

  return cuddT(f);
}

static inline zddp zdd_lo(zddp f)
{
  assert(dd_mgr != NULL);
  assert(f      != BDD_NULL);
  assert(!zdd_isconst(f));

  return cuddE(f);
}

static inline uintmax_t zdd_card(zddp f)
{
  assert(dd_mgr != NULL);

  if(     f == zdd_top()) return 1;
  else if(f == zdd_bot()) return 0;
  else                    return (uintmax_t)Cudd_zddCountDouble(dd_mgr, f);
}

static inline zddp zdd_union(zddp f, zddp g)
{
  assert(dd_mgr != NULL);
  assert(f != BDD_NULL && g != BDD_NULL);

  return Cudd_zddUnion(dd_mgr, f, g);
}

static inline zddp zdd_intersect(zddp f, zddp g)
{
  assert(dd_mgr != NULL);
  assert(f != BDD_NULL && g != BDD_NULL);
  
  return Cudd_zddIntersect(dd_mgr, f, g);
}

static inline zddp zdd_diff(zddp f, zddp g)
{
  assert(dd_mgr != NULL);
  assert(f != BDD_NULL && g != BDD_NULL);
  
  return Cudd_zddDiff(dd_mgr, f, g);
}

static inline zddp zdd_node(itemval i, zddp lo, zddp hi)
{
  assert(dd_mgr != NULL);
  assert(!zdd_isconst(hi)? i < zdd_itemval(hi): true);
  assert(!zdd_isconst(lo)? i < zdd_itemval(lo): true);

  return cuddZddGetNode(dd_mgr, (int)i, hi, lo);
}


#else
#error "Specify BDD/ZDD package"
#endif /*defined(SELECT_CUDD)*/

#endif /*BDD_INTERFACE_H*/
