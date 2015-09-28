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
    void update_crit_and_uncov(Hypergraph& crit,
                               bitset& uncov,
                               const Hypergraph& H,
                               const Hypergraph& T,
                               const bitset& S,
                               const hindex v) {
        /*
          Update crit[] and uncov to reflect S+v.
          (Assumes crit[] and uncov were correct for S.)

          NOTE: Raises a vertex_violating_exception if any w in S is not
          critical in S+v. In this case, the state of crit[] and uncov
          is undefined!
         */
        // Input specification
        assert(not S.test(v));
        assert(crit[v].none());

        // v is critical for edges it hits which were previously uncovered
        bitset v_edges = T[v];
        crit[v] = v_edges & uncov;

        // Remove anything v hits from uncov
        uncov -= v_edges;

        // Remove anything v hits from the other crit[w]s
        bool found_violating_vertex = false;
        hindex w = S.find_first();
        while (w != bitset::npos) {
            crit[w] -= v_edges;
            if (crit[w].none()) {
                found_violating_vertex = true;
            }
            w = S.find_next(w);
        }

        if (found_violating_vertex) {
            throw vertex_violating_exception();
        }
    }

    void restore_crit_and_uncov(Hypergraph& crit,
                                bitset& uncov,
                                const Hypergraph& H,
                                const Hypergraph& T,
                                const bitset& S,
                                const hindex v) {
        /*
          Update crit[] and uncov to reflect S no longer containing v.
          (Assumes crit[] and uncov were correct for S with v included.)
        */
        // Input specification
        assert(not S.test(v));
        assert(not uncov.intersects(crit[v]));
        assert(crit[v].is_subset_of(T[v]));

        // If v was critical for an edge, it is now uncovered
        uncov |= crit[v];
        crit[v].reset();

        // If v was in an edge but was not critical, some other vertex
        // may now be.
        bitset check_edges = T[v] - crit[v];

        hindex e = check_edges.find_first();
        while (e != bitset::npos) {
            // Find vertices in S which are also in the edge
            bitset S_verts_in_e = H[e] & S;
            if (S_verts_in_e.count() == 1) {
                hindex crit_vert = S_verts_in_e.find_first();
                crit[crit_vert].set(e);
            }

            e = check_edges.find_next(e);
        }
    }
}
