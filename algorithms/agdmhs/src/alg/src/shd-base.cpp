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
            if (H[edge_index][v]) {
                // Remove e from all crit[v]'s
                for (hindex w = 0; w < H.num_verts(); ++w) {
                    crit[w][edge_index] = false;
                }

                // If this edge was new_uncovered, it is no longer, but v is now new_critical for it
                if (uncov[edge_index] == true) {
                    uncov[edge_index] = false;
                    crit[v][edge_index] = true;
                }
            }
        }
    }
}
