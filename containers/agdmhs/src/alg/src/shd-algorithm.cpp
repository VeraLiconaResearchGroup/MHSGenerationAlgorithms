/**
   C++ implementation of the RS algorithm (library)
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

#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"
#include "shd-algorithm.hpp"

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <cassert>

namespace agdmhs {
    bool SHDAlgorithm::vertex_would_violate (const Hypergraph& crit,
                                             const Hypergraph::Edge& uncov,
                                             const Hypergraph& H,
                                             const Hypergraph& T,
                                             const Hypergraph::Edge& S,
                                             Hypergraph::EdgeIndex v) const {
        /*
          Determine whether addition of v to S would violate any vertices.
          (That is, whether any vertex in S would be redundant in S+v.)
        */

        // Input specification
        assert(not S.test(v));
        assert(crit[v].none());

        // We only consider edges which are hit by v and are not in uncov
        Hypergraph::Edge test_edges = T[v] - uncov;

        // Check whether any w in S would lose all its critical edges
        Hypergraph::EdgeIndex w = S.find_first();
        while (w != Hypergraph::Edge::npos) {
            if (crit[w].is_subset_of(test_edges)) {
                return true;
            }
            w = S.find_next(w);
        }

        // If we make it this far, no vertex was violated
        return false;
    }

    SHDAlgorithm::hsetmap SHDAlgorithm::update_crit_and_uncov (Hypergraph& crit,
                                                 Hypergraph::Edge& uncov,
                                                 const Hypergraph& H,
                                                 const Hypergraph& T,
                                                 const Hypergraph::Edge& S,
                                                 Hypergraph::EdgeIndex v) const {
        /*
          Update crit[] and uncov to reflect S+v.
          (Assumes crit[] and uncov were correct for S.)
          Returns overlay with edges removed from crit.
        */
        // Input specification
        assert(not S.test(v));
        assert(crit[v].none());

        // v is critical for edges it hits which were previously uncovered
        const Hypergraph::Edge& v_edges = T[v];
        crit[v] = v_edges;
        crit[v] &= uncov;

        // Remove anything v hits from uncov
        uncov -= v_edges;

        // Remove anything v hits from the other crit[w]s and record it
        // in critmark[w]s
        hsetmap critmark;
        Hypergraph::EdgeIndex w = S.find_first();
        while (w != Hypergraph::Edge::npos) {
            critmark[w] = crit[w];
            critmark[w] &= v_edges;
            crit[w] -= v_edges;

            w = S.find_next(w);
        }

        return critmark;
    }

    void SHDAlgorithm::restore_crit_and_uncov (Hypergraph& crit,
                                               Hypergraph::Edge& uncov,
                                               const Hypergraph::Edge& S,
                                               const hsetmap& critmark,
                                               Hypergraph::EdgeIndex v) const {
        /*
          Update crit[] and uncov to reflect S no longer containing v.
          (Assumes crit[] and uncov were correct for S with v included.)
        */

        // Input specification
        assert(not S.test(v));
        assert(not uncov.intersects(crit[v]));

        // If v was critical for an edge, it is now uncovered
        uncov |= crit[v];
        crit[v].reset();

        // Restore all other crit vertices using critmark
        Hypergraph::EdgeIndex w = S.find_first();
        while (w != Hypergraph::Edge::npos) {
            try {
                crit[w] |= critmark.at(w);
            }
            catch (std::out_of_range& e) {
                // This may occur if a vertex_violating_exception was thrown.
                // It's not an error, so we just catch the exception and
                // do nothing.
            }
            w = S.find_next(w);
        }
    }
}
