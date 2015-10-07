/**
   C++ implementation of the "fast and simple parallel algorithm" or Boros and Makino
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _PARBM__H
#define _PARBM__H

#include <vector>

#include "hypergraph.hpp"

namespace agdmhs {
    Hypergraph bm_transversal(const Hypergraph& H,
                              const size_t num_threads);

    bitset bm_find_new_hs(const Hypergraph& H,
                          const Hypergraph& G,
                          const bitset& c);

    bitset bm_find_new_hs_fork(const Hypergraph& H,
                               const Hypergraph& G,
                               const Hypergraph& C);
}

#endif
