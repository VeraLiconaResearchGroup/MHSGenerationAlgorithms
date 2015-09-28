/**
   C++ implementation of the RS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "shd-base.hpp"

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/dynamic_bitset.hpp>

#include <cassert>

namespace agdmhs {
    bool vertex_would_violate(const Hypergraph& crit,
                              const bitset& uncov,
                              const Hypergraph& H,
                              const Hypergraph& T,
                              const bitset& S,
                              const hindex v) {
        /*
          Determine whether addition of v to S would violate any vertices.
          (That is, whether any vertex in S would be redundant in S+v.)
         */

        // Input specification
        assert(not S.test(v));
        assert(crit[v].none());

        // We only consider edges which are hit by v and are not in uncov
        bitset test_edges = T[v] - uncov;

        // Check whether any w in S would lose all its critical edges
        hindex w = S.find_first();
        while (w != bitset::npos) {
            if ((crit[w] - test_edges).none()) {
                return true;
            }
            w = S.find_next(w);
        }

        // If we make it this far, no vertex was violated
        return false;
    }

    Hypergraph update_crit_and_uncov(Hypergraph& crit,
                               bitset& uncov,
                               const Hypergraph& H,
                               const Hypergraph& T,
                               const bitset& S,
                               const hindex v) {
        /*
          Update crit[] and uncov to reflect S+v.
          (Assumes crit[] and uncov were correct for S.)
          Returns overlay with edges removed from crit.

          NOTE: Raises a vertex_violating_exception if any w in S is not
          critical in S+v. In this case, crit[] and uncov are restored.
         */
        // Input specification
        assert(not S.test(v));
        assert(crit[v].none());

        // v is critical for edges it hits which were previously uncovered
        bitset v_edges = T[v];
        crit[v] = v_edges & uncov;

        // Remove anything v hits from uncov
        uncov -= v_edges;

        // Remove anything v hits from the other crit[w]s and record it
        // in critmark[w]s
        Hypergraph critmark(H.num_edges(), H.num_verts());

        hindex w = S.find_first();
        while (w != bitset::npos) {
            critmark[w] = crit[w] & v_edges;
            crit[w] -= v_edges;

            if (crit[w].none()) {
                restore_crit_and_uncov(crit, uncov, S, critmark, v);
                throw vertex_violating_exception();
            }
            w = S.find_next(w);
        }

        return critmark;
    }

    void restore_crit_and_uncov(Hypergraph& crit,
                                bitset& uncov,
                                const bitset& S,
                                const Hypergraph& critmark,
                                const hindex v) {
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
        hindex w = S.find_first();
        while (w != bitset::npos) {
            crit[w] |= critmark[w];
            w = S.find_next(w);
        }
    }
}
