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

#ifndef _barray_c_
#define _barray_c_

#include"barray.h"

BARRAY INIT_BARRAY = {TYPE_BARRAY, NULL, 0, NULL, 0, 0, 0, 0}; 

/* initialization */
void BARRAY_init (BARRAY *A, QUEUE_ID xsiz, VEC_ID t){
  A->xsiz = xsiz;
  A->xend = (xsiz-1)/32+1;
  A->end = A->t = t;
  calloc2 (A->v, (A->xend)*t, EXIT);
}

/* termination */
void BARRAY_end (BARRAY *A){
  free2 (A->v);
}

/* set from x1-th bit to x2-th bit to 1. x2 is also set to 1 */
void BARRAY_set_interval (unsigned long *a, QUEUE_ID x1, QUEUE_ID x2){
  QUEUE_ID x=x1/32, i, xx=x2/32;
  if ( x == xx ){
    for (i=x1%32 ; i<=x2%32 ; i++)  BARY_SET (a, i);
  } else {
    a[x] |= BITMASK_UPPER1[x1%32];
    x++;
    for ( ; x<xx ; x++) a[x] = 0xffffffff;
    a[xx] |= BITMASK_LOWER1_[x2%32];
  }
}

/* set from x1-th bit to x2-th bit to 0. x2 is also set to 0 */
void BARRAY_reset_interval (unsigned long *a, QUEUE_ID x1, QUEUE_ID x2){
  QUEUE_ID x=x1/32, i, xx=x2/32;
  if ( x == xx ){
    for (i=x1%32 ; i<=x2%32 ; i++) BARY_RESET (a, i);
  } else {
    a[x] &= BITMASK_LOWER1[x1%32];
    x++;
    for ( ; x<xx ; x++) a[x] = 0;
    a[xx] &= BITMASK_UPPER1_[x2%32];
  }
}

/* print */
void BARRAY_print (BARRAY *A){
  VEC_ID i;
  QUEUE_ID j;
  
  printf ("xsiz=%d t=%d\n", A->xsiz, A->t);
  FLOOP (i, 0, A->t){
    FLOOP (j, 0, A->xsiz) printf (BARRAY_BIT (*A, j, i)?"1":"0");
    printf ("\n");
  }
}

/* compare one long as bit sequence from k1-th bit to k2-th bit.
  (_suf compares top to k-th bits, _pre compares k-th bits to the tail.
  return minus value if A is small, 0 if same, plus value if B is small (same as "qsort")
  the absolute value of the returned value is the first different bit +1 */
int BARRAY_longcmp_suf (unsigned long a, unsigned long b, int k){
  unsigned long m = a^b;
  for ( ; k<32 ; k++){
    if ( m&BITMASK_1[k] ){
      if ( a&BITMASK_1[k] ) return (k+1);
      else return (-k-1);
    }
  }
  return(0);
}
int BARRAY_longcmp_pre (unsigned long a, unsigned long b, int k ){
  unsigned long j, m = a^b;
  FLOOP (j, 0, k+1){
    if ( m&BITMASK_1[j] ){
      if ( a&BITMASK_1[j] ) return (j+1);
      else return (-j-1);
    }
  }
  return(0);
}
int BARRAY_longcmp ( unsigned long a, unsigned long b, int k1, int k2){
  unsigned long m = a^b;
  for ( ; k1<=k2 ; k1++){
    if ( m&BITMASK_1[k1] ){
      if ( a&BITMASK_1[k1] ) return (k1+1);
      else return (-k1-1);
    }
  }
  return(0);
}

/* for return  */
int BARRAY_cmp_return (unsigned long a, unsigned long b){
  return ( b + (b<0?-a*32:b>0?a*32:0) );
}

int BARRAY_cmp (unsigned long *a, unsigned long *b, QUEUE_ID k){
  QUEUE_ID i=0;
  while ( a[i] == b[i] ){
    i++;
    if ( i > (k-1)/32 ) return (0);  /* two are the same until the last */ 
  }
  return ( BARRAY_cmp_return ( i, BARRAY_longcmp_pre (a[i], b[i], k%32)));
}

/* set 1 for all positions in QUEUE Q */
void BARRAY_set_subset (unsigned long *a, QUEUE *Q){
  QUEUE_INT *x;
  MQUE_FLOOP (*Q, x) BIT_SET(a,*x);
}

/*
int BARRAY_subcmp_ (unsigned long *a, unsigned long *b, QUEUE_ID k1, QUEUE_ID k2 ){
  int i=k1/32, m;
  if ( i == k2/32 )
    return (BARRAY_cmp_return(i, BARRAY_longcmp(A->v[i], B->v[i], k1%32, k2%32)));
  m = BARRAY_longcmp_suf ( A->v[i], B->v[i], k1%32 );
  if ( m != 0 ) return ( BARRAY_cmp_return ( i, m ) );

  while ( A->v[i] == B->v[i] ){
    i++;
    if ( i == k2/32 ) break;
  }
  return( BARRAY_cmp_return( i, BARRAY_longcmp_pre( A->v[i], B->v[i], k2%32)));
}
int BARRAY_subcmp (unsigned long *a, unsigned long *b, QUEUE_ID k1, QUEUE_ID k2, QUEUE_ID k){
#ifdef ERROR_CHECK
  if ( k1<0 || k1>=A->end ){
    printf ("BARRAY_subcmp: k1 is out of range. %d\n", k1 );
    exit(1);
  }
  if ( k2<0 || k2>=B->end ){
    printf ("BARRAY_subcmp: k2 is out of range. %d\n", k2 );
    exit(1);
  }
  if ( k+k1 >= A->end ) k = A->end - k1;
  if ( k+k2 >= B->end ) k = B->end - k2;
#endif
  while ( BARRAY_01(*A,k1) == BARRAY_01(*B,k2) ){
    if ( k1 == k ) return (0);
    k1++;
    k2++;
  }
  return ( BARRAY_BIT(*A,k1)? k1+1: -k1-1 );
}
*/

/* load barray matrix, from 01 matrix file */
/* if cr code is middle of a line, the rest is trancated (not filled), 
 and the following line will be read to a new line. */
/* 0, -, ' ' will be 0, and the others will be 1 */
/* support LOAD_TPOSE */
void BARRAY_load (BARRAY *A){
  FILE2 fp;
  size_t xmax=0, x=0, y=0, i=0;
  int c, flag=0, f, ff;
  
  FILE2_open (fp, A->fname, "r", EXIT);
  while ( (c = FILE2_getc (&fp)) >=0 ){
    if ( c=='\n' ){
      if ( flag == 2 ) flag = 0;
      else flag = 1;
    } else if ( c=='\r' ){ 
      if ( flag == 1 ) flag = 0;
      else flag = 2;
    } else { x++; flag = 0; }
    if ( flag ){ i++; ENMAX (xmax, x); x=0; }
  }
  if ( A->flag & LOAD_TPOSE ) { y=xmax; xmax=i; i=y; } 
  BARRAY_init (A, xmax, i);
  x = i = 0;
  FILE2_reset (&fp);
  while ( (c=FILE2_getc(&fp)) >=0 ){
    if ( c=='\n' ){
      if ( flag == 2 ) flag = 0;
      else flag = 1;
    } else if ( c=='\r' ){
      if ( flag == 1 ) flag = 0;
      else flag = 2;
    } else {
      flag = 0;
      f = (c=='0' || c==' ' || c=='-')?1:0;
      ff = (A->flag&LOAD_COMP)?1:0;
      if ( f^ff ){
        if ( A->flag & LOAD_TPOSE ) BARRAY_RESET (*A, i, x);
        else BARRAY_RESET (*A, x, i);
      } else if ( A->flag & LOAD_TPOSE ) BARRAY_SET (*A, i, x);
      else BARRAY_SET (*A, x, i);
      x++;
    }
    if ( flag ){ i++; x=0; }
  }
  FILE2_close (&fp);     if(ERROR_MES) EXIT;
}


/*
main (){
  BARRAY A, B;
  int i, j;

  BARRAY_init ( &A, 70 );
  BARRAY_init ( &B, 70 );
  A.a[0] = 0x1111ffff;
  B.a[0] = 0x1111ffff;
  A.a[1] = 0x1111f1ff;
  B.a[1] = 0x1111ff1f;
  printf ("%d\n", BARRAY_subcmp ( &A, &B, 26,30,40) );

  BARRAY_set_interval ( &A, 0, 69);
  BARRAY_reset_interval ( &A, 0, 63 );
  BARRAY_SET ( A,10 );
  BARRAY_SET ( A,20 );
  BARRAY_SET ( A,30 );
  BARRAY_SET ( A,40 );
  BARRAY_SET ( A,50 );
  BARRAY_SET ( A,60 );
  BARRAY_print ( &A );
}
*/


#endif

