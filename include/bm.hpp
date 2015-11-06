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

    void bm_find_new_hses(const Hypergraph& H,
                          const Hypergraph& G,
                          const bitset& c,
                          bsqueue& results);

    void bm_find_new_hses_fork(const Hypergraph& H,
                               const Hypergraph& G,
                               const Hypergraph& C,
                               bsqueue& results);
}

#endif
