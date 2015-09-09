/**
   C++ implementation of the RS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "shd-base.hpp"

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>

namespace agdmhs {
    void update_crit_and_uncov(Hypergraph& crit,
                               bitset& uncov,
                               const Hypergraph& H,
                               const hindex v) {
        for (hindex edge_index = 0; edge_index < H.num_edges(); ++edge_index) {
            // If the vertex is in this edge, proceed
            if (H[edge_index].test(v)) {
                // Remove e from all crit[w]'s
                for (hindex w = 0; w < H.num_verts(); ++w) {
                    crit[w].reset(edge_index);
                }

                // If this edge was uncovered, it is no longer, but v is now critical for it
                if (uncov.test(edge_index)) {
                    uncov.reset(edge_index);
                    crit[v].set(edge_index);
                }
            }
        }
    }
}
