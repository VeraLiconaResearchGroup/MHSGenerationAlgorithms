/**
   C++ implementation of the "fast and simple parallel algorithm" or Boros and Makino
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

   This file is part of MHSGenerationAlgorithms.

   MHSGenerationAlgorithms is free software: you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation, either version 3 of
   the License, or (at your option) any later version.

   MHSGenerationAlgorithms is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
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
