// -*- C++ -*-
#include "trvgen.h"
#include "sort.h"
using namespace std;
/*
  m_size: number of minimals
  E: array of hyper edges
  me: number of hyper edges
  ele: number of unsigned ints to represent an edge
  nv: number of vertices
 */
unsigned *trvgen(long long &m_size, unsigned *E, long long me, int ele, int nv){
    //printf("\n ne = %d\n", ne);
    if(me <= TRV_BASE)
	return ser_trvgen(m_size, E, me, ele, nv);
    else {
	// ToDo: a better split
	long long half = me/2;
	long long ohalf = me - half;
	unsigned *E1, *E2;
	long long s1, s2;

	E1 = cilk_spawn trvgen(s1, E, half, ele, nv);
	E2 = trvgen(s2, E+half*ele, ohalf, ele, nv);
	cilk_sync;

	return min_sum(m_size, E1, s1, E2, s2, ele, nv);
    }
}

unsigned *ser_trvgen(long long &m_size, unsigned *E, long long me, int ele, int nv){

    if (me == 1){ //{ (x) | x \in E };
      if ( E[0] == 0 ) {
	std::cout <<"number of vertices is needed" << std::endl;
	exit(1);
      }

      unsigned *T = (unsigned *)calloc(E[0]*ele, sizeof(unsigned));

      //TODO: traversal could made more efficient
      unsigned *tp = T;
      for (int pos = 0; pos < nv; ++pos){
	if (is_set(E, pos)) {
	  set(tp, pos);
	  tp[0] = 1;
	  tp += ele;
	}
      }

      m_size = E[0];
      return T;
    }
    if (me == 2){

      unsigned *e2 = E + ele;
      unsigned *A = (unsigned *)calloc(3*ele, sizeof(unsigned));
      unsigned *B = A + ele;
      unsigned *C = B + ele;
      unsigned t = 0;

      if ( E[0] == 0 || e2[0] == 0) {
	std::cout <<"number of vertices is needed" << std::endl;
	exit(1);
      }

      for (int i = 1; i < ele; i++) {
	A[i] = E[i] & e2[i];
	t = ~A[i];
	B[i] = E[i] & t;
	C[i] = e2[i] & t;
      }

      count(A, ele);       //count(B, ele); count(C, ele);
      B[0] = E[0] - A[0];
      C[0] = e2[0] - A[0];

      //cout << "A =" << A[0] << ";B = " << B[0] << "; C = " << C[0] << endl;
      //print_edges(A, 3, nv, ele);
      m_size = A[0] + B[0] * C[0];
      unsigned *T = (unsigned *)calloc(m_size*ele, sizeof(unsigned));

      unsigned *tp = T;
      for (int pos = 0; pos < nv; ++pos){ //T := { {x} | x in A }
	if ( is_set(A, pos) ) {
	  set(tp, pos);
	  tp[0] = 1; // 1 vertice
	  tp += ele;
	}
      }

      for (int i=0; i<nv; ++i) { //{{x,y} | (x,y) \in B \times C }
	if ( is_set(B, i) ) {
	  for (int j=0; j<nv; ++j){
	    if ( is_set(C, j) ) {
	      set(tp, i);
	      set(tp, j);
	      tp[0] = 2; // 2 vertices
	      tp += ele;
	    }
	  }
	}
      }

      free(A);
      return T;
    }else {

      long long half = me/2;
      long long ohalf = me - half;

      long long s1;
      unsigned *E1 = ser_trvgen(s1, E, half, ele, nv);

      long long s2;
      unsigned *E2 = ser_trvgen(s2, E+half*ele, ohalf, ele, nv);

      return ser_min_sum(m_size, E1, s1, E2, s2, ele, nv);
    }
}

unsigned inline *ser_min_sum(long long &m_size, unsigned *E1, long long s1,
			     unsigned *E2, long long s2, int ele, int nv){
  //-------------------------------------------
  if (s1 == 0) {
    m_size = s2;
    free(E1);
    return E2;
  }
  if (s2 == 0) {
    m_size = s1;
    free(E2);
    return E1;
  }
  //cout << "s1 = " << s1 << "; s2 = " << s2 << endl;
  long long s = s1*s2;
  unsigned *T = (unsigned *)calloc(s*ele, sizeof(unsigned));

  unsigned *Tp = T;
  unsigned *E1p = E1;
  unsigned *E2p = E2;

  // compute all e1 union e2, e1 \in E1, e2 \in E2
  for (int i = 0; i < s1; ++i){
    for (int j = 0; j < s2; ++j){
      for (int k = 1; k < ele; ++k){ // union
	Tp[k] = E1p[k] | E2p[k];
      }
      Tp += ele;
      E2p += ele;
    }

    E1p += ele;
    E2p = E2;
  }

  unsigned *Tp1 = T;
  unsigned *Tp2 = T;
  int t;
  long long end = s-1;

  for(int i = 0; i < end; ++i){

    Tp1 = T + i*ele;

    if ( Tp1[0] <= nv ){ //not removed
      for(int j = i+1; j < s; ++j){

	Tp2 = T + j*ele;
	if(Tp2[0] <= nv){ //not removed
	  t = inclusionTest(Tp1, Tp2, ele);
	  if ( t == -1 ) { // Tp1 < Tp2
	    Tp2[0] = nv + 1;
	  }else if ( t == 1 ) { // Tp1 > Tp2
	    Tp1[0] = nv+1;
	    break;
	  }
	}
      }
    }
  }

  int k = 0;
  size_t sv = ele * sizeof(unsigned);
  for(int i = 0; i < s; ++i){
    if ( T[i * ele] <= nv ){
      if ( k != i ) {
	memcpy(T + k * ele, T + i * ele, sv);
      }
      k++;
    }
  }

  m_size = k;
  return T;
}

unsigned* min_sum(long long &m_size, unsigned* E1, long long s1,
                  unsigned* E2, long long s2, int ele, int nv){

  if (s1 == 0) {
    m_size = s2;
    free(E1);
    return E2;
  }
  if (s2 == 0) {
    m_size = s1;
    free(E2);
    return E1;
  }
  if ( ( s1>MIN_SUM_BASE ) && ( s2>MIN_SUM_BASE ) ) {
    long long s11 = s1/2;
    long long s12 = s1 - s11;
    long long s21 = s2/2;
    long long s22 = s2 - s21;
    unsigned *E12 = E1 + s11*ele;
    unsigned *E22 = E2 + s21*ele;
    unsigned *c11, *c12;
    long long r11, r12;

    c11 = cilk_spawn half_min_sum(r11, E1, s11, E2, s21,
				  E12, s12, E22, s22, ele, nv);
    c12 = half_min_sum(r12, E1, s11, E22, s22, E12, s12, E2, s21, ele, nv);

    cilk_sync;

    return merge_min(m_size, c11, r11, c12, r12, ele, nv);

  } else if (( s1>MIN_SUM_BASE ) && ( s2<=MIN_SUM_BASE )){
    long long s11 = s1/2;
    long long s12 = s1 - s11;
    unsigned *E12 = E1 + s11*ele;

    long long r1;
    unsigned *c1 = min_sum(r1, E1, s11, E2, s2, ele, nv);

    long long r2;
    unsigned *c2 = min_sum(r2, E12, s12, E2, s2, ele, nv);

    return merge_min(m_size, c1, r1, c2, r2, ele, nv);

  } else if (( s1<=MIN_SUM_BASE ) && ( s2>MIN_SUM_BASE )){
    long long s21 = s2/2;
    long long s22 = s2 - s21;
    unsigned *E22 = E2 + s21*ele;

    long long r1;
    unsigned *c1 = min_sum(r1, E1, s1, E2, s21, ele, nv);

    long long r2;
    unsigned *c2 = min_sum(r2, E1, s1, E22, s22, ele, nv);

    return merge_min(m_size, c1, r1, c2, r2, ele, nv);
  }
  else {
    return ser_min_sum(m_size, E1, s1, E2, s2, ele, nv);
  }
}

unsigned *half_min_sum(long long &r11, unsigned *E1, long long s11,
		       unsigned *E2, long long s21,
		       unsigned *E12, long long s12,
		       unsigned *E22, long long s22, int ele, int nv){
  //----------------------------------------

    long long r1;
    unsigned *c1;
    c1 = cilk_spawn min_sum(r1, E1, s11, E2, s21, ele, nv);

    long long r2;
    unsigned *c2;
    c2 = min_sum(r2, E12, s12, E22, s22, ele, nv);

    cilk_sync;

    //    cout << "r1 = " << r1 << "; r2 = " << r2 << endl;
    return merge_min(r11, c1, r1, c2, r2, ele, nv);
}

unsigned *merge_min(long long &m_size, unsigned* E1, long long s1,
		    unsigned* E2, long long s2, int ele, int nv){
  //------------------------------------
    long long news1, news2;
    inner_merge_min(E1, s1, news1, E2, s2, news2, ele, nv);

    m_size = news1 + news2;

    if(news1 < news2){
      E2 = (unsigned *)realloc(E2, sizeof(unsigned)*ele*m_size);
      memcpy(E2 + news2 * ele, E1, news1 * ele * sizeof(unsigned));
      free(E1);
      return E2;
    }else{
      E1 = (unsigned *)realloc(E1, sizeof(unsigned)*ele*m_size);
      memcpy(E1 + news1 * ele, E2, news2 * ele * sizeof(unsigned));
      free(E2);
      return E1;
    }
}

void inner_merge_min(unsigned* E1, long long s1, long long &news1,
		     unsigned* E2, long long s2, long long &news2, int ele, int nv){
  //-------------------------------------------
  if (s1 == 0 || s2 == 0) {
    news1 = s1;
    news2 = s2;
    return;
  }

  if ((s1 > MERGE_MIN_BASE) && (s2 > MERGE_MIN_BASE)) {
    long long s11 = s1/2;
    long long s12 = s1 - s11;
    long long s21 = s2/2;
    long long s22 = s2 - s21;
    unsigned *E12 = E1 + s11*ele;
    unsigned *E22 = E2 + s21*ele;

    long long r11, r21;
    cilk_spawn inner_merge_min(E1, s11, r11, E2, s21, r21, ele, nv);
    long long r12, r22;
    inner_merge_min(E12, s12, r12, E22, s22, r22, ele, nv);

    cilk_sync;

    long long t11, t22;
    cilk_spawn inner_merge_min(E1, r11, t11, E22, r22, t22, ele, nv);
    long long t12, t21;
    inner_merge_min(E12, r12, t12, E2, r21, t21, ele, nv);

    cilk_sync;

    size_t st = ele*sizeof(unsigned);
    memcpy(E1 + t11*ele, E12, t12*st);
    memcpy(E2 + t21*ele, E22, t22*st);

    news1 = t11 + t12;
    news2 = t21 + t22;
    return;

  }else if ((s1 > MERGE_MIN_BASE) && (s2 <= MERGE_MIN_BASE)){
    long long s11 = s1/2;
    long long s12 = s1 - s11;
    unsigned *E12 = E1 + s11*ele;

    long long r11, r2;
    inner_merge_min(E1, s11, r11, E2, s2, r2, ele, nv);
    long long r12, t2;
    inner_merge_min(E12, s12, r12, E2, r2, t2, ele, nv);

    memcpy(E1+r11*ele, E12, r12*ele*sizeof(unsigned));

    news1 = r11 + r12;
    news2 = t2;
    return;

  }else if ((s1 <= MERGE_MIN_BASE) && (s2 > MERGE_MIN_BASE)){
    long long s21 = s2/2;
    long long s22 = s2 - s21;
    unsigned *E22 = E2 + s21*ele;

    long long r1, r21;
    inner_merge_min(E1, s1, r1, E2, s21, r21, ele, nv);
    long long t1, r22;
    inner_merge_min(E1, r1, t1, E22, s22, r22, ele, nv);

    memcpy(E2+r21*ele, E22, r22*ele*sizeof(unsigned));

    news1 = t1;
    news2 = r21 + r22;
    return;

  } else {
    ser_inner_merge_min(E1, s1, news1, E2, s2, news2, ele, nv);
    return;
  }
}

void inline ser_inner_merge_min(
     unsigned *El, long long l, long long &newl,
     unsigned *Er, long long r, long long &newr, int ele, int nv) {
    //-----------------------------------------
    int lbegin, rbegin, t;
    unsigned *key;

    //cout << "l = " << l << "; r = " << r << endl;
    for( int i = 0; i < l; ++i ){
      key = El + i * ele;
      if(key[0] <= nv){
	for( int j = 0; j < r; ++j ){
	  rbegin = j * ele;
	  if ( Er[rbegin] <= nv ) { //not removed
	    t = inclusionTest(key, Er + rbegin, ele);
	    if ( t == -1 ) { //El[lbegin] < Er[rbegin]
	      Er[rbegin] = nv + 1; //remove
	    }else if ( t == 1 ) {//El[lbegin] > Er[rbegin]
	      key[0] = nv + 1; //remove
	      break;
	    }
	  }
	}
      }
    }

    long long kl = 0;
    size_t sv = ele * sizeof(unsigned);

    for (int i = 0; i < l; ++i){
      if ( El[i * ele] <= nv ){
	if ( kl != i ) {
	  memcpy(El + kl * ele, El + i * ele, sv);
	}
	kl++;
      }
    }

    newl = kl;
    long long kr = 0;

    for (int i = 0; i < r; ++i){
      if ( Er[i * ele] <= nv ){
	if ( kr != i ) {
	  memcpy(Er + kr * ele, Er + i * ele, sv);
	}
	kr++;
      }
    }

    newr = kr;

    //cout << "newl = " << newl << ";newr = " << newr << endl;
}

// -1, e1 < e2
// 1, e1 >= e2
// 0, cannot compare
int inclusionTest(unsigned *e1, unsigned *e2, int ele) {
    //-------------------------------------
    int f = 1, g = 1;
    unsigned c = 0;

    for (int i = 1; i < ele; ++i){

      c = e1[i] & e2[i];
      if ( e1[i] != c )
	f = 0;
      if ( e2[i] != c )
	g = 0;
      if ( f == 0 && g == 0 )
	return 0;
    }

    if ( g == 1 ) return 1; //same, keep e2 or e1 > e2
    if ( f == 1 ) return -1; //e1 < e2
}
