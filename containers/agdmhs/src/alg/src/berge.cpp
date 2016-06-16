/**
   C++ implementation of Berge's algorithm (library)
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

#include "berge.hpp"
#include "mhs-algorithm.hpp"
#include "hypergraph.hpp"

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    BergeAlgorithm::BergeAlgorithm (unsigned cutoff_size):
        cutoff_size(cutoff_size)
    {};


    /**
     * Update a set of transversals to reflect a new edge
     *
     * @param T  the known transversals (modified in-place!)
     * @param edge  the new edge
     * @return the new transversals
     **/
    Hypergraph BergeAlgorithm::update_transversals_with_edge (const Hypergraph& transversals,
                                                              const Hypergraph::Edge& edge) const {
        assert(transversals.num_verts() == edge.size());

        // Generate a new hypergraph with a singleton edge
        // for each vertex in the given edge
        Hypergraph new_edges (edge.size());

        Hypergraph::EdgeIndex vertex = edge.find_first();
        while (vertex != Hypergraph::Edge::npos) {
            Hypergraph::Edge newedge (edge.size());
            newedge.set(vertex);
            new_edges.add_edge(newedge);
            vertex = edge.find_next(vertex);
        }

        // Build the new transversals
        if (transversals.num_edges() == 0) {
            // If G is empty, these new edges are all we need
            return new_edges;
        } else {
            // Otherwise, take the wedge of G with the new edges and minimize
            if (cutoff_size != 0) {
                return transversals.edge_wedge_cutoff(new_edges, cutoff_size, true);
            } else {
                return transversals.edge_wedge(new_edges, true);
            }
        }
    }

    Hypergraph BergeAlgorithm::transversal (const Hypergraph& H) const {
        BOOST_LOG_TRIVIAL(debug) << "Starting Berge. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";

        Hypergraph transversals (H.num_verts());
        for (unsigned i = 0; i < H.num_edges(); ++i) {
            BOOST_LOG_TRIVIAL(debug) << "Considering edge " << i;
            Hypergraph::Edge edge = H[i];
            transversals = update_transversals_with_edge(transversals, edge);
            BOOST_LOG_TRIVIAL(debug) << "|T| = " << transversals.num_edges();
        }

        return transversals;
    }
}
