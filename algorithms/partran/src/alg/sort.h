#ifndef _SORT_H
#define _SORT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum comp {
  EQUAL,
  LESS,
  LARGER,
  INCOMPARABLE
};

#define sfixn unsigned
#define WORDSIZE (sizeof(sfixn)*8)
#define COLEX_SORT_BASE 32
#define COLEX_MERGE_BASE 32

/**
 * check if a certain vertex is included in a hyperedge
 */
bool inline is_set(sfixn *edge, int pos){
  int i = pos / WORDSIZE;
  int r = pos % WORDSIZE;
  int m = 1U << r;
  sfixn c = edge[i+1];
  sfixn n = (c | m);
  return (n == c);
}

/**
 * check if a certain bit is set in a number:
 * pos should be in range [0, 32)
 */
bool inline is_set_single(sfixn edge, int pos){
  int m = 1U << pos;
  sfixn n = (edge | m);
  return (n == edge);
}

/**
 * set a certain bit in an edge. Equivalently, set a certain bit in
 * the bitvector edge + 1:
 * pos should be in range [0, nv)
 */

void inline set(sfixn *edge, int pos){
  int i = pos / WORDSIZE; // the i-th unsigned in the data array is used
  int r = pos % WORDSIZE;
  int m = (1U << r);
  edge[i+1] |= m;
}

//code from <<hacker's delight>>
static inline int pop(unsigned x) {

  x = x - ((x >> 1) & 0x55555555);

  x = (x & 0x33333333) + ((x >> 2) & 0x33333333);

  x = (x + (x >> 4)) & 0x0F0F0F0F;

  x = x + (x >> 8);

  x = x + (x >> 16);

  return x & 0x0000003F;

}

void inline count(sfixn *edge, int elem_size){
  edge[0] = 0;
  for (int i = 1; i < elem_size; i++) {
    edge[0] += pop(edge[i]);
  }
}

//=====================================================
void sort_merge(sfixn *edges1, long long n1, sfixn *edges2, long long n2,
		sfixn *work, int elem_size, int nv);
void sort_edges(sfixn *edges, long long nelem, sfixn *work, int elem_size, int nv);
void print_edge(sfixn *edge, int nv, int elem_size);
void print_edges(sfixn *edges, long long ne, int nv, int elem_size);
inline comp compare_edges(sfixn *edge1, sfixn *edge2, int elem_size);
#endif
