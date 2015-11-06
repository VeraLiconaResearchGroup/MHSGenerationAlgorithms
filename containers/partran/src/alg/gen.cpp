// -*- C++ -*-

#include "trvgen.h"
#include "sort.h"
#include "gen.h"
#include <cstdlib>
#include <ctime>
using namespace std;

unsigned *read_files(long long &ne, int &nv, int &elem_size, FILE *fp){

    fscanf(fp, "%d", &nv);
    fscanf(fp, "%Ld", &ne); // note here, ne is read as an int

    if( nv <= 0 )
      elem_size = 1;
    else if( (nv % WORDSIZE) == 0 )
      elem_size = nv/WORDSIZE;
    else
      elem_size = nv/WORDSIZE + 1;
    ++elem_size;


    sfixn *edges = (sfixn*) calloc(ne * elem_size, sizeof(unsigned));
    unsigned *cur = edges;
    char c;
    //    int ncol = nv * 2 + 1;
    int index = 0, count = 0;

    // for(int i = 0; i < ne; ++i){
    //  for(int j = 0; j < ncol; ++j){
    while (fscanf(fp, "%c", &c) != EOF) {
      if(c == '*'){
        //printf(" index = %2d, N  ;", index);
	++index;
      }
      if(c == '0'){
	set(cur, index);
	//printf(" index = %2d, %d ;", index, count);
	++count;
	++index;
      }
      if(index % nv == 0 && index != 0){
	cur[0] = count;
	cur += elem_size;
	count = 0;
	index = 0;
	//cout << endl;
      }
    }

    return edges;
}
/**
 * gen_lovasz
 * given a number "r", generate the lovasz hypergraph of L_r
 * the sets are X_0 = {0}, X_1 = {1,2}, X_2 = {3,4,5}, X_3 = {6,7,8,9},....
 * so each set X_i contains element in range [i(i+1)/2, (i+1)(i+2)/2)
 *
 */

unsigned *gen_lovasz(long long &ne, int &elem_size, int &nv, int r){

  nv = (1 + r) * r / 2;
  if( nv <= 0 )
    elem_size = 1;
  else if( (nv % WORDSIZE) == 0 )
    elem_size = nv/WORDSIZE;
  else
    elem_size = nv/WORDSIZE + 1;

  ++elem_size; // one more unsigned to mark its status

  long long t = 1;
  ne = 1;

  for(int i = r; i > 1; --i){
    t *= i;
    ne += t;
  }

  cout << "ne = " << ne << "; nv = " << nv << endl;

  unsigned *edges = (unsigned *)calloc(ne * elem_size, sizeof(unsigned));
  int *comb = (int *)calloc(r, sizeof(int));
  sfixn *cur = edges;

  for(int k = 0; k < r; k++){
    cur[0] = r;

    comb[0] = k * (k + 1) / 2;
    set(cur, comb[0]);
    for(int i = 1; i < k + 1; ++i){
      comb[i] = comb[i-1] + 1;
      set(cur, comb[i]);
    }

    for(int i = k + 1; i < r; ++i){
      comb[i] = i * (i + 1)/ 2;
      set(cur, comb[i]);
    }

    cur += elem_size;
    while(next_comb_lovasz(comb, r, k)){
      cur[0] = r;
      for(int i = 0; i < r; ++i)
 	set(cur, comb[i]);
      cur += elem_size;
    }
  }

  free(comb);
  return edges;
}

// generating all combination X_k \union {x_{k+1}, ... , x_r}

int next_comb_lovasz(int *comb, int r, int k){
  int i;
  int j;

  //  bool has_next = true;

  for(i = r - 1; i > k; --i){
    if(comb[i] < (i+1) * (i+2) / 2 - 1)
      break;
  }

  //cout << "i == " << i << "; k == " << k << endl;

  if( i == k ){
    //cout << "ever return ??? \n" << endl;
    return 0;
  }

  ++(comb[i]);

  for(j = i + 1; j < r; ++j)
    comb[j] = j * (j + 1) / 2;

  return 1;
}


/**generate_enum
 * given n nodes, generate the hypergraph which contains all the
 * possible edges composing of r nodes.
 * Assumption:
 * r <= n
 */

void generate_enum(unsigned *E, int *comb, int elem_size, int nv, int r){

  // Generate the first combination
  //  int *comb = (int *)calloc(r, sizeof(int));

  // printf("r = %d \n", r);

  for (int i = 0; i < r; ++i){
    comb[i] = i;
    set(E, i);
  }
  E[0] = r;

  //print_edge(E, nv, elem_size);

  /* Generate and print all the other combinations */
  unsigned *cur = E + elem_size;

  //cout << "r = " << r << "; nv = " << nv << endl;

  while (next_comb_new(comb, r, nv)){
    cur[0] = r;
    for(int i = 0; i < r; ++i)
      set(cur, comb[i]);
    cur += elem_size;
  }
  return;
}

void print_comb(int *comb, int r){
  for(int i=0; i<r; ++i)
    cout << " " << comb[i] << " ";
  cout << endl;
}

int next_comb_new(int *comb, int r, int n){
  int i;
  int j;

  if (comb[0] == n-r )
    return 0;
  for(i = r-1; i >= 0; --i){
    if(comb[i] < n-r+i)
      break;
  }

  ++(comb[i]);
  for(j = i+1; j < r; ++j)
    comb[j] = comb[j-1] + 1;
  return 1;
}

/**
 * generate_enum
 * given n nodes, generate the hypergraph which contains all the
 * possible edges composing of r nodes.
 * Assumption:
 * r <= n
 */

unsigned * generate_rand(int &elem_size, int nv, long long ne){

  if( nv <= 0 )
    elem_size = 1;
  else if( (nv % WORDSIZE) == 0 )
    elem_size = nv/WORDSIZE;
  else
    elem_size = nv/WORDSIZE + 1;

  ++elem_size; // one more unsigned to mark its status

  unsigned *edges = (unsigned *)calloc(ne * elem_size, sizeof(unsigned));
  sfixn *edge;

  for(int i = 0; i < ne; i++){
    edge = edges + i * elem_size;
    for(int j = 1; j < elem_size; j++){
      edge[j] = rand();
    }
    // edge[elem_size - 1] = 0;
    //set(edge, nvt - i - 2);
    count(edge, elem_size);
  }

  for(int i = 0; i < ne; i++){
    edge = edges + i * elem_size;
    for(int j = 0; j < elem_size; j++){
      printf("%d, ", edge[j]);
    }
  }
  printf("\n");


  return edges;
}
