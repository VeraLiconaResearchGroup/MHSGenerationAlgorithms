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

#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"

namespace agdmhs {
    class FKAlgorithm: public MHSAlgorithm {
    protected:
        static Hypergraph::Edge hitting_condition_check (const Hypergraph& F, const Hypergraph& G);
        static Hypergraph::Edge coverage_condition_check (const Hypergraph& F, const Hypergraph& G);
        static Hypergraph::Edge edge_size_check (const Hypergraph& F, const Hypergraph& G);
        static Hypergraph::Edge satisfiability_count_check (const Hypergraph& F, const Hypergraph& G);
        static Hypergraph::Edge small_hypergraphs_check (const Hypergraph& F, const Hypergraph& G);

        static Hypergraph::EdgeIndex most_frequent_vertex (const Hypergraph& F, const Hypergraph& G);

        static Hypergraph minimized_union (const Hypergraph& F, const Hypergraph& G);

        static std::pair<Hypergraph, Hypergraph> split_hypergraph_over_vertex (const Hypergraph& H, Hypergraph::EdgeIndex v);
        static Hypergraph::Edge minimize_new_hs (const Hypergraph& F, const Hypergraph& G, Hypergraph::Edge S);
    };
}

#endif
