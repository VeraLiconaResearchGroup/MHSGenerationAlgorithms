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
                               const bitset& S,
                               const hindex v) {
        bitset edges = H.edges_containing_vertex(v);
        hindex e = edges.find_first();
        while (e != bitset::npos) {
            // If this edge was uncovered, it is no longer, but v is now critical for it
            if (uncov.test(e)) {
                uncov.reset(e);
                crit[v].set(e);
            } else {
                // Remove e from all crit[w]'s
                hindex w = S.find_first();
                while (w != bitset::npos) {
                    crit[w].reset(e);
                    w = S.find_next(w);
                }
            }

            e = edges.find_next(e);
        }
    }

    void restore_crit_and_uncov(Hypergraph& crit,
                                bitset& uncov,
                                const Hypergraph& H,
                                const bitset& S,
                                const hindex v) {
        crit[v].reset();

        bitset edges = H.edges_containing_vertex(v);
        hindex e = edges.find_first();
        while (e != bitset::npos) {
            // If the vertex is in this edge, proceed
            bitset hitting_vertices = H[e] & S;
            hindex first_hit_vertex = hitting_vertices.find_first();
            if (first_hit_vertex == bitset::npos) {
                // No other vertex in S hits this edge
                uncov.set(e);
            } else if (hitting_vertices.find_next(first_hit_vertex) == bitset::npos) {
                // Exactly one other vertex in S hits this edge
                crit[first_hit_vertex].set(e);
            } // Otherwise, no change required

            e = edges.find_next(e);
        }
    }
}
