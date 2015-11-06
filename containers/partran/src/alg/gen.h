// -*- C++ -*-
#ifndef _GEN_H
#define _GEN_H

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <iostream>

void generate_enum(unsigned *E, int *comb, int elem_size, int nv, int r);
unsigned * generate_rand(int &elem_size, int nv, long long ne);
int next_comb_new(int *comb, int r, int n);
void print_comb(int *comb, int r);
unsigned * gen_lovasz(long long &ne, int &elem_size, int &nv, int r);
int next_comb_lovasz(int *comb, int r, int k);
unsigned *read_files(long long &ne, int &nv, int &elem_size, FILE *fp);
#endif
