/**
   C++ implementation of the FK algorithms
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

#ifndef _FK_BASE__H
#define _FK_BASE__H

#include <vector>
#include <utility>

#include <boost/dynamic_bitset.hpp>

#include "hypergraph.hpp"

namespace agdmhs {
    bitset fk_hitting_condition_check(const Hypergraph& F, const Hypergraph& G);
    bitset fk_coverage_condition_check(const Hypergraph& F, const Hypergraph& G);
    bitset fk_edge_size_check(const Hypergraph& F, const Hypergraph& G);
    bitset fk_satisfiability_count_check(const Hypergraph& F, const Hypergraph& G);
    bitset fk_small_hypergraphs_check(const Hypergraph& F, const Hypergraph& G);

    hindex fk_most_frequent_vertex(const Hypergraph& F, const Hypergraph& G);

    Hypergraph fk_minimized_union(const Hypergraph& F, const Hypergraph& G);

    std::pair<Hypergraph, Hypergraph> fk_split_hypergraph_over_vertex(const Hypergraph& H, const hindex& v);
    bitset fk_minimize_new_hs(const Hypergraph& F, const Hypergraph& G, bitset S);
}
#endif
