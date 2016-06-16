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

#include "hypergraph.hpp"
#include "fk-algorithm.hpp"

namespace agdmhs {
    class ParBMAlgorithm: public FKAlgorithm {
        unsigned num_threads;

    public:
        ParBMAlgorithm ();
        ParBMAlgorithm (unsigned num_threads);
        Hypergraph transversal (const Hypergraph& H) const override;

    private:
        void find_new_hses(const Hypergraph& H, const Hypergraph& G, const Hypergraph::Edge& c, Hypergraph::EdgeQueue& results) const;
        void find_new_hses_fork(const Hypergraph& H, const Hypergraph& G, const Hypergraph& C, Hypergraph::EdgeQueue& results) const;
        void minimize_new_hses (const Hypergraph& H, const Hypergraph& G, Hypergraph::EdgeQueue& new_hses, Hypergraph::EdgeQueue& new_mhses) const;

        static Hypergraph l4_full_cover (const Hypergraph& H, const Hypergraph::Edge& edge);
        static Hypergraph l5_full_cover (const Hypergraph& H, const Hypergraph::Edge& base_transversal);
        static Hypergraph::Edge find_missed_edge (const Hypergraph& H, const Hypergraph::Edge& I);
        static Hypergraph::Edge find_subset_edge (const Hypergraph& H, const Hypergraph::Edge& I);
        static Hypergraph::Edge minimize_new_hs (const Hypergraph& H, Hypergraph::Edge new_hs);
    };
}

#endif
