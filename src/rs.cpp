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
    std::atomic<unsigned> rs_update_loops;

    std::atomic<unsigned> rs_tasks_waiting;

    static bool rs_any_edge_critical_after_i(const hindex& i,
                                   const bitset& S,
                                   const Hypergraph& crit) {
        /*
          Return true if any vertex in S has its first critical edge
          after i.
         */
        hindex w = S.find_first();
        while (w != bitset::npos) {
            hindex first_crit_edge = crit[w].find_first();
            if (first_crit_edge >= i) {
                return true;
            }
            w = S.find_next(w);
        }

        return false;
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

        // Loop over vertices in that edge (in reverse order!)
        for (auto& v: search_indices) {
            ++rs_update_loops;
            hsetmap critmark = update_crit_and_uncov(crit, uncov, H, T, S, v);

            // Check whether any vertex in S has its first critical
            // edge after the search edge
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
                if (rs_tasks_waiting < 4 and uncov.size() > 2) {
                    // Spawn a new task if the queue is getting low, but
                    // don't waste time with small jobs.
                    // Each one gets its own copy of S, CAND, crit, and uncov
                    ++rs_tasks_waiting;
                    bitset new_S = S;
                    Hypergraph new_crit = crit;
                    bitset new_uncov = uncov;
                    bitset new_viol = violating_vertices | new_violating_vertices;
#pragma omp task shared(H, T) // Start a new task
                    {
                    --rs_tasks_waiting;
                    rs_extend_or_confirm_set(H, T, new_S, new_crit, new_uncov, new_viol, cutoff_size);
                    }
                } else {
                    rs_extend_or_confirm_set(H, T, S, crit, uncov, violating_vertices | new_violating_vertices, cutoff_size);
                }
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

        // Candidate hitting set
        bitset S (H.num_verts());
        S.reset(); // Initially empty

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are uncovered
        bitset uncov (H.num_edges());
        uncov.set(); // Initially full

        // Which vertices are known to be violating
        bitset violating_vertices (H.num_verts());

        // Tranpose of H
        Hypergraph T = H.transpose();

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, T) num_threads(num_threads) // Don't create thread-local copies of H and T
#pragma omp master // Only spawn the computation once
            rs_extend_or_confirm_set(H, T, S, crit, uncov, violating_vertices, cutoff_size);
#pragma omp taskwait // Don't proceed until all the tasks are complete
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        bitset result;
        while (HittingSets.try_dequeue(result)) {
            Htrans.add_edge(result);
        }

        BOOST_LOG_TRIVIAL(info) << "pRS complete: " << rs_iterations << " iterations, " << rs_violators << " violating verts, " << rs_critical_fails << " critical check failures, " << rs_update_loops << " update loops.";

        return Htrans;
    };
}
