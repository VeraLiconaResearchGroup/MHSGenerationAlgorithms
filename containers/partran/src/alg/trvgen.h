// -*- C++ -*-
#ifndef _BUILD_SUPPORT_H
#define _BUILD_SUPPORT_H

#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <iostream>

// #define cilk_sync
// #define cilk_spawn 
// #define cilk_for for

#define TRV_BASE 32
#define MIN_SUM_BASE 16
#define MERGE_MIN_BASE 128

unsigned *trvgen(long long &m_size, unsigned *E, long long me, int ele, int nv);

unsigned *ser_trvgen(long long &m_size, unsigned *E, long long me, int ele, int nv);

unsigned inline *ser_min_sum(
          long long &m_size, unsigned *E1, long long s1, 
          unsigned *E2, long long s2, int ele, int nv);

unsigned* min_sum(long long &m_size, unsigned* E1, long long s1, 
                  unsigned* E2, long long s2, int ele, int nv);

unsigned *half_min_sum(
          long long &r11, unsigned *E1, long long s11, 
          unsigned *E2, long long s21, 
          unsigned *E12, long long s12, 
          unsigned *E22, long long s22, int ele, int nv);

unsigned *merge_min(
          long long &m_size, unsigned* E1, long long s1, 
          unsigned* E2, long long s2, int ele, int nv);

void inner_merge_min(
     unsigned* E1, long long s1, long long &news1, 
     unsigned* E2, long long s2, long long &news2, int ele, int nv);

void inline ser_inner_merge_min(
     unsigned *El, long long l, long long &newl, 
     unsigned *Er, long long r, long long &newr, int ele, int nv);

int inclusionTest(unsigned *e1, 
                  unsigned *e2, int ele);

#endif
