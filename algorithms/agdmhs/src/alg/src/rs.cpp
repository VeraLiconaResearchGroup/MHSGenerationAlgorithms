/**
   C++ implementation of the RS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "rs.hpp"

#include "hypergraph.hpp"
#include "shd-base.hpp"

#include <atomic>
#include <cassert>
#include <deque>
#include <omp.h>

#include <boost/dynamic_bitset.hpp>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/dynamic_bitset.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    static bsqueue HittingSets;

    std::atomic<unsigned> rs_iterations;
    std::atomic<unsigned> rs_violators;
    std::atomic<unsigned> rs_critical_fails;

    static bool rs_any_edge_critical_after_i(const hindex& i,
                                   const bitset& S,
                                   const Hypergraph& crit) {
        /*
          Return true if any vertex in S has its first critical edge
          after i.
         */
        bool bad_edge_found = false;
        hindex w = S.find_first();
        while (w != bitset::npos and not bad_edge_found) {
            hindex first_crit_edge = crit[w].find_first();
            if (first_crit_edge >= i) {
                bad_edge_found = true;
                break;
            }
            w = S.find_next(w);
        }

        return bad_edge_found;
    };

    static void rs_extend_or_confirm_set(const Hypergraph& H,
                                         const Hypergraph& T,
                                         bitset& S,
                                         Hypergraph& crit,
                                         bitset& uncov,
                                         const bitset& violating_vertices,
                                         const size_t cutoff_size) {
        ++rs_iterations;

        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, get an uncovered edge
        hindex search_edge = uncov.find_first(); // Just use the first set in uncov
        bitset e = H[search_edge];

        // Store the indices in the edge for iteration
        bitset new_violating_vertices (H.num_verts());
        std::deque<hindex> search_indices;
        hindex v = e.find_first();
        while (v != bitset::npos) {
            // If v is already known violating, we skip it entirely
            bool known_violating = violating_vertices.test(v);
            if (not known_violating and vertex_would_violate(crit, uncov, H, T, S, v)) {
                // Otherwise, if it would violate, we mark it
                new_violating_vertices.set(v);
                ++rs_violators;
            } else if (not known_violating) {
                // And if not, we will consider it in the loop
                search_indices.push_front(v);
            }
            v = e.find_next(v);
        }

        // Loop over vertices in that edge
        for (auto& v: search_indices) {
            // Check preconditions
            Hypergraph critmark;
            // This call can throw, but if it does it represents a logic error,
            // so we don't catch it here
            critmark = update_crit_and_uncov(crit, uncov, H, T, S, v);

            if (rs_any_edge_critical_after_i(search_edge, S, crit)) {
                ++rs_critical_fails;
                restore_crit_and_uncov(crit, uncov, S, critmark, v);
                continue;
            }

            // if new_uncov is empty, adding v to S makes a hitting set
            S.set(v);

            if (uncov.none()) {
                // In this case, S is a valid hitting set, so we store it
                HittingSets.enqueue(S);
            } else if (cutoff_size == 0 or S.count() < cutoff_size) {
                // In this case, S is not yet a hitting set but is not too large either
//#pragma omp task untied shared(H, T)
                rs_extend_or_confirm_set(H, T, S, crit, uncov, violating_vertices | new_violating_vertices, cutoff_size);
            }

            // Update crit, uncov, and S, then proceed to the next vertex
            S.reset(v);
            restore_crit_and_uncov(crit, uncov, S, critmark, v);
        }
    };

    Hypergraph rs_transversal(const Hypergraph& H,
                              const size_t num_threads,
                              const size_t cutoff_size) {
        // SET UP INTERNAL VARIABLES
        // Debugging counters
        rs_iterations = 0;
        rs_violators = 0;
        rs_critical_fails = 0;

        // Number of threads for parallelization
        omp_set_num_threads(num_threads);

        // Candidate hitting set
        bitset S (H.num_verts());
        S.reset(); // Initially empty

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are known to be violating
        bitset violating_vertices (H.num_verts());

        // Which edges are uncovered
        bitset uncov (H.num_edges());
        uncov.set(); // Initially full

        // Tranpose of H
        Hypergraph T = H.transpose();

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, T)
#pragma omp single
            rs_extend_or_confirm_set(H, T, S, crit, uncov, violating_vertices, cutoff_size);
#pragma omp taskwait
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        bitset result;
        while (HittingSets.try_dequeue(result)) {
            Htrans.add_edge(result);
        }

        BOOST_LOG_TRIVIAL(info) << "pRS complete: " << rs_iterations << " iterations, " << rs_violators << " violating verts, " << rs_critical_fails << " critical check failures.";

        return Htrans;
    };
}
