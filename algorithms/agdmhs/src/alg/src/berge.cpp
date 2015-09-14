/**
   C++ implementation of Berge's algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "berge.hpp"

#include "hypergraph.hpp"

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/dynamic_bitset.hpp>

#include <iostream>

namespace agdmhs {
    void berge_update_transversals_with_edge(Hypergraph& G,
                                             const bitset& edge,
                                             const size_t cutoff_size) {
        // Update transversals in G to reflect the given edge
        // Note: this updates G in place!
        assert(G.num_verts() == edge.size());

        // Generate a new hypergraph with a singleton edge
        // for each vertex in the given edge
        Hypergraph newedges (edge.size());

        hindex vertex = edge.find_first();
        while (vertex != bitset::npos) {
            bitset newedge (edge.size());
            newedge.set(vertex);
            newedges.add_edge(newedge);
            vertex = edge.find_next(vertex);
        }

        if (G.num_edges() == 0) {
            // If G is empty, these new edges are all we need
            G = newedges;
        } else {
            // Otherwise, take the wedge of G with the new edges and minimize
            if (cutoff_size != 0) {
                G = G.edge_wedge_cutoff(newedges, cutoff_size, true);
            } else {
                G = G.edge_wedge(newedges, true);
            }
        }
    }

    Hypergraph berge_transversal(const Hypergraph& H,
                                 const size_t cutoff_size) {
        BOOST_LOG_TRIVIAL(debug) << "Starting Berge. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";

        Hypergraph G (H.num_verts());
        for (hindex i = 0; i < H.num_edges(); ++i) {
            BOOST_LOG_TRIVIAL(debug) << "Considering edge " << i;
            bitset edge = H[i];
            berge_update_transversals_with_edge(G, edge, cutoff_size);
            BOOST_LOG_TRIVIAL(debug) << "|G| = " << G.num_edges();
        }

        return G;
    }
}
