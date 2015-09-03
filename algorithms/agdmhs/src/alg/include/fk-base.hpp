/**
   C++ implementation of the FK algorithms
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
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
}
#endif
