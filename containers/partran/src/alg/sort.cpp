// -*- C++ -*-

#include <cilk/cilk.h>
#include "sort.h"

using namespace std;
/** 
 * compare_edges:
 * function to compare two hyperedges in colex ordering.
 * @edge1: the first input edge
 * @edge2: the second input edge
 * @elem_size: the number of sfixn each edge is encoded with
 * return:
 * LESS: if edge1 < edge2
 * LARGER: if edge1 > edge2
 * EQUAL: if edge1 = edge
 */

inline comp compare_edges(sfixn *edge1, sfixn *edge2, int elem_size){
  
  bool set1, set2;

  for(int i = elem_size - 1; i > 0 ; --i){ // edge[0] is used for mark.
//     cout << "       i = " << i << endl;
//     cout << "edge1[i] = " << edge1[i] << "; edge2[i] = " << edge2[i]<<endl;
    if(edge1[i] != edge2[i]){

      for(int j = WORDSIZE - 1; j >= 0 ; --j){
	set1 = is_set_single(edge1[i], j);
	set2 = is_set_single(edge2[i], j);
// 	cout << "bit " << j << " : set1 = "<< set1 << " & set2 = " <<set2<<endl;
	if(set1 && (! set2))
	  return LARGER;
	if((! set1) && set2)
	  return LESS;
      }
    }
  }
  return EQUAL;
}

/**
 * binary_search:
 * locate a "edge" in an asecendingly sorted edge set "edges[begin, end]". 
 * return:
 * if monomial is found in the set, return true and write its index to
 * "index"; else "index" is the largest index $edge[index-1] < edge$
 *
 */

inline bool binary_search(long long &index, sfixn *edge, sfixn *edges, 
			  long long begin, long long end, int elem_size){
  long long low = begin;
  long long tmp = end+1;
  long long high = (begin > tmp) ? begin : tmp;
  comp comp;
  long long mid;
  while(low < high){
    mid = (low + high)/2;
    comp = compare_edges(edge, edges + mid*elem_size, elem_size);
    if(comp == LESS)
      high = mid;
    else if(comp == LARGER)
      low = mid+1;
    else{
      index = mid;
      return true;
    }
  }
  index = high;
  return false;
}

/**
 * sort_edges:
 * sort a list of hyperedges asecendingly
 * Assumption:
 * work is preallocated space for merge. It has the same size of edges
 *
 */
void sort_edges(sfixn *edges, long long nelem, sfixn *work, int elem_size, int nv){
  if(nelem <= COLEX_SORT_BASE){
    // base case is using insertion sort
    int i;
    sfixn *key = (sfixn *)calloc(elem_size, sizeof(sfixn)), *cur;
    
    for(int j = 1; j < nelem; j++){
      i = j-1;
      memcpy(key, edges + j * elem_size, elem_size*sizeof(sfixn));
      cur = edges + i * elem_size;
      while(i>=0 && compare_edges(cur, key, elem_size) == LARGER){
	memcpy(cur + elem_size, cur, elem_size * sizeof(sfixn));
	cur = edges + (--i) * elem_size;
      }
      memcpy(edges + (i + 1) * elem_size, key, elem_size * sizeof(sfixn));
    }
    free(key);
    
  }else{
    long long half = nelem/2;
    long long ohalf = nelem - half;
    long long offset = half * elem_size;
    sfixn *edges2 = edges + offset;
    cilk_spawn sort_edges(edges, half, work, elem_size, nv);
    sort_edges(edges2, ohalf, work + offset, elem_size, nv);
    cilk_sync;
    
//     printf("===============   edge 1   =============\n");
//     print_edges(edges, half, nv, elem_size);
//     printf("===============   edge 2   =============\n");
//     print_edges(edges2, half, nv, elem_size);

    sort_merge(edges, half, edges2, ohalf, work, elem_size, nv);
    memcpy(edges, work, nelem * elem_size * sizeof(sfixn));
  }
}
 
void sort_merge(sfixn *edges1, long long n1, sfixn *edges2, long long n2, 
		sfixn *work, int elem_size, int nv){
  if(n1 <= COLEX_MERGE_BASE && n2 <= COLEX_MERGE_BASE){
    int pos(0), i(0), j(0);
    sfixn *e1, *e2;
    comp result;
    while(i < n1 && j < n2){
      e1 = edges1 + i * elem_size;
      e2 = edges2 + j * elem_size;
      result = compare_edges(e1, e2, elem_size);
      if(result == LESS){
	memcpy(work + pos * elem_size, e1, elem_size*sizeof(sfixn));
	++i;
      }else{
	memcpy(work + pos * elem_size, e2, elem_size*sizeof(sfixn));
	++j;
      }
      pos++;
    }
    
    if(i < n1){
      int left = n1 - i;
      memcpy(work + pos * elem_size, edges1 + i * elem_size, 
	     left * elem_size * sizeof(sfixn));
      pos += left;
      
    }else if(j < n2){
      int left = n2 - j;
      memcpy(work + pos * elem_size, edges2 + j * elem_size, 
	     left * elem_size * sizeof(sfixn));
      pos += left;
    }
  }else if(n1 >= n2){
    
    long long half = n1 / 2, index;
    sfixn *mid = edges1 + half * elem_size;
    binary_search(index, mid, edges2, 0, n2 - 1, elem_size);
    
//     cout << " searching edge " << endl;
//     print_edge(mid, nv, elem_size);
//     cout << " in list edge " << endl;
//     print_edges(edges2, n2, nv, elem_size);
//     cout << " index ==== " << index << endl;

    long long ohalf1 = n1 - half, ohalf2 = n2 - index;

//     if(index = 0){
//       memcpy(work, edges1, half * elem_size * sizeof(sfixn));
//       sort_merge(edges1 + half*elem_size, ohalf1, edges2, n2, 
// 		 work + half * elem_size, elem_size, nv);
//       return;
//     }
//     if(ohalf2 = 0){
//       sort_merge(edges1, half, edges2, n2, work, elem_size, nv);
//       memcpy(work + (half + n2) * elem_size, edges1 + half * elem_size, 
// 	     ohalf1 * elem_size * sizeof(sfixn));
//     }
    cilk_spawn sort_merge(edges1, half, edges2, index, work, elem_size, nv);
    sort_merge(edges1 + half*elem_size, ohalf1, edges2 + index*elem_size, 
	       ohalf2, work + (half + index) * elem_size, elem_size, nv);
    cilk_sync;
  }else{
  
    long long half = n2 / 2, index;
    sfixn *mid = edges2 + half * elem_size;
    binary_search(index, mid, edges1, 0, n1 - 1, elem_size);
    long long ohalf2 = n2 - half, ohalf1 = n1 - index;

//     if(index = 0){
//       memcpy(work, edges2, half * elem_size * sizeof(sfixn));
//       sort_merge(edges1, n1, edges2 + half * elem_size, ohalf2, 
// 		 work + half * elem_size, elem_size, nv);
//       return;
//     }
//     if(ohalf1 = 0){
//       sort_merge(edges1, n1, edges2, half, work, elem_size, nv);
//       memcpy(work + (half + n1) * elem_size, edges2 + half * elem_size, 
// 	     half * elem_size * sizeof(sfixn));
//     }
   
    cilk_spawn sort_merge(edges1, index, edges2, half, work, elem_size, nv);
    sort_merge(edges1 + index*elem_size, ohalf1, edges2 + half*elem_size, 
	       ohalf2, work + (half + index) * elem_size, elem_size, nv);
    cilk_sync;
  }
}
 
//====== support functions =============

void print_edge(sfixn *edge, int nv, int elem_size){
  if(edge[0] > nv)
    printf("  NULL               :");
  else
    printf("NORMAL  %3d bits set :", edge[0]);
  for(int i = 0; i < nv; i++){

    if(i % 4 == 0)
      printf(" ");
    if(is_set(edge, i))
      printf("1");
    else
      printf("0");

  }

  printf(" ==");
  for(int i = 1; i < elem_size; i++)
    printf(" %12d", edge[i]);

  printf("\n");;
}

void print_edges(sfixn *edges, long long ne, int nv, int elem_size){
  for(long long i=0; i<ne; i++){
    printf("%3d :", i);
    print_edge(edges + i*elem_size, nv, elem_size);
  }
}
