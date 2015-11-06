/** \file   ssort1.c
 *  \brief  Implementation of a slight modified version of multikey sort
 *  \note   The original code was obtained from http://www.cs.princeton.edu/~rs/strings/ 
 */


#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#include "my_def.h"
#include "ssort1.h"
#include "rng.h"


// MULTIKEY QUICKSORT

#ifdef i2c
#error "The macro i2c is already defined."
#else
#define i2c(i) x[i][depth]
#endif /*i2c*/

static inline uintmax_t min(uintmax_t a, uintmax_t b)
{
  return ((a)<=(b) ? (a) : (b));
}

static inline void swap(uintmax_t a, uintmax_t b, itemval **x)
{
  itemval *t=x[a];
  x[a]=x[b]; x[b]=t;
}

static inline void vecswap(uintmax_t i, uintmax_t j, uintmax_t n, itemval **x)
{   while (n-- > 0) {
        swap(i, j, x);
        i++;
        j++;
    }
}

static uintmax_t recdepth = 0; //!< recursion depth

static void ssort1(itemval **x, uintmax_t n, uintmax_t depth)
{   
  uintmax_t a, b, c, d, m;
  itemval   r, v;
  if (n <= 1)
    return;
  INC_RECDEPTH(recdepth);  
  a = rng_rand(n);
  swap(0, a, x);
  v = i2c(0);
  a = b = 1;
  c = d = n-1;
  for (;;) {
    while (b <= c && (r = i2c(b)-v) <= 0) {
      if (r == 0) { swap(a, b, x); a++; }
      b++;
    }
    while (b <= c && (r = i2c(c)-v) >= 0) {
      if (r == 0) { swap(c, d, x); d--; }
      c--;
    }
    if (b > c) break;
    swap(b, c, x);
    b++;
    c--;
  }
  m = min(a, b-a);     vecswap(0, b-m, m, x);
  m = min(d-c, n-d-1); vecswap(b, n-m, m, x);
  m = b-a; ssort1(x, m, depth);
  assert(i2c(m) == v);
  if (i2c(m) != INFTY) {
    ssort1(x + m, a + n-d-1, depth+1);
    m = d-c; ssort1(x + n - m, m, depth);
  } else {
    assert(d-c == 0);
  }
  DEC_RECDEPTH(recdepth);
}

void ssort1main(itemval **x, uintmax_t n)
{
  assert(recdepth == 0);
  ssort1(x, n, 0);
  assert(recdepth == 0);
}

