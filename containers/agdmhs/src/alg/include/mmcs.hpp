/**
   C++ implementation of the MMCS algorithm
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

#ifndef _MMCS__H
#define _MMCS__H

#include "hypergraph.hpp"
#include "shd-algorithm.hpp"

#include <atomic>

namespace agdmhs {
    struct MMCSCounters {
        std::atomic<unsigned> mhses_found {0};
        std::atomic<unsigned> iterations {0};
        std::atomic<unsigned> violators {0};
        std::atomic<unsigned> update_loops {0};
        std::atomic<unsigned> tasks_waiting {0};
    };

    class MMCSAlgorithm: public SHDAlgorithm {
        unsigned num_threads;
        unsigned cutoff_size;
        bool count_only;

    public:
        MMCSAlgorithm (unsigned num_threads, unsigned cutoff_size, bool count_only = false);
        Hypergraph transversal (const Hypergraph& H) const override;

    private:
        void extend_or_confirm_set (const Hypergraph& H, const Hypergraph& T, MMCSCounters& counters, Hypergraph::EdgeQueue& hitting_sets, Hypergraph::Edge& S, Hypergraph::Edge& CAND, Hypergraph& crit, Hypergraph::Edge& uncov) const;
    };
}

#endif
