/*
    bit-array library
            12/Mar/2002   by Takeaki Uno  e-mail:uno@nii.jp, 
    homepage:   http://research.nii.ac.jp/~uno/index.html  */
/* This program is available for only academic use, basically.
   Anyone can modify this program, but he/she has to write down 
    the change of the modification on the top of the source code.
   Neither contact nor appointment to Takeaki Uno is needed.
   If one wants to re-distribute this code, please 
    refer the newest code, and show the link to homepage of 
    Takeaki Uno, to notify the news about the codes for the users. */

#ifndef _barray_h_
#define _barray_h_

#include"queue.h"


typedef struct {
  unsigned char type;  // mark to identify type of the structure
  char *fname;      // input file name
  int flag;         // flag

  unsigned long *v;
  VEC_ID end;
  VEC_ID t;
  QUEUE_ID xend;
  QUEUE_ID xsiz;
} BARRAY;
extern BARRAY INIT_BARRAY;

#define BARY_SET(a,x) ((a)[(x)/32]|=BITMASK_1[(x)%32])
#define BARY_RESET(a,x) ((a)[(x)/32]&=BITMASK_31[(x)%32])
#define BARY_BIT(a,x) ((a)[(x)/32]&BITMASK_1[(x)%32])
#define BARY_01(a,x) (((a)[(x)/32]&BITMASK_1[(x)%32])/BITMASK_1[(x)%32])
#define BARRAY_SET(A,x,y) ((A).v[(y)*(A).xend+(x)/32]|=BITMASK_1[(x)%32])
#define BARRAY_RESET(A,x,y) ((A).v[(y)*(A).xend+(x)/32]&=BITMASK_31[(x)%32])
#define BARRAY_BIT(A,x,y) ((A).v[(y)*(A).xend+(x)/32]&BITMASK_1[(x)%32])
#define BARRAY_01(A,x,y) (((A).v[(y)*(A).xend+(x)/32]&BITMASK_1[(x)%32])/BITMASK_1[(x)%32])

/* initialization/termination */
void BARRAY_init (BARRAY *A, QUEUE_ID x_end, VEC_ID t);
void BARRAY_end (BARRAY *A);

/* set from x1-th bit to x2-th bit to 1/0. x2 is also set to 1/0 */
void BARRAY_set_interval (unsigned long *a, QUEUE_ID x1, QUEUE_ID x2);
void BARRAY_reset_interval (unsigned long *a, QUEUE_ID x1, QUEUE_ID x2);

/* print */
void BARRAY_print (BARRAY *A);


/* compare one long as bit sequence from k1-th bit to k2-th bit.
  (_suf compares top to k-th bits, _pre compares k-th bits to the tail.
  return minus value if A is small, 0 if same, plus value if B is small (same as "qsort")
  the absolute value of the returned value is the first different bit +1 */
int BARRAY_longcmp_suf (unsigned long a, unsigned long b, int k);
int BARRAY_longcmp_pre (unsigned long a, unsigned long b, int k);
int BARRAY_longcmp (unsigned long a, unsigned long b, int k1, int k2);
/* for "return" */
int BARRAY_cmp_return (unsigned long a, unsigned long b);

/* compare bit sequence in the lexicographical order.
  (compare from k1 to k2-th bits, compare k1-th to (k1+k)th bits of A and 
  k2-th to (k2+k)th bits of B)
  return minus value if A is small, 0 if same, plus value if B is small (same as "qsort")
  the absolute value of the returned value is the first different bit +1 */
int BARRAY_cmp (unsigned long *a, unsigned long *b, QUEUE_ID k);
int BARRAY_subcmp_ (unsigned long *a, unsigned long *b, QUEUE_ID k1, QUEUE_ID k2);
int BARRAY_subcmp (unsigned long *a, unsigned long *b, QUEUE_ID k1, QUEUE_ID k2, QUEUE_ID k);


#endif

