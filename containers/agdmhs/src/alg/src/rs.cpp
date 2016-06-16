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
#include "rs.hpp"
#include "mhs-algorithm.hpp"
#include "shd-algorithm.hpp"

#include <cassert>
#include <deque>
#include <omp.h>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/dynamic_bitset.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    RSAlgorithm::RSAlgorithm (unsigned num_threads,
                              unsigned cutoff_size,
                              bool count_only):
        num_threads(num_threads), cutoff_size(cutoff_size), count_only(count_only)
    {};

    Hypergraph RSAlgorithm::transversal (const Hypergraph& H) const {
        // SET UP INTERNAL VARIABLES
        // Debugging counters
        RSCounters counters;

        // Candidate hitting set
        Hypergraph::Edge S (H.num_verts());
        S.reset(); // Initially empty

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are uncovered
        Hypergraph::Edge uncov (H.num_edges());
        uncov.set(); // Initially full

        // Which vertices are known to be violating
        Hypergraph::Edge violating_vertices (H.num_verts());

        // Tranpose of H
        Hypergraph T = H.transpose();

        // Queue to store hitting sets as they are generated
        Hypergraph::EdgeQueue hitting_sets;

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, T) num_threads(num_threads) // Don't create thread-local copies of H and T
#pragma omp master // Only spawn the computation once
            extend_or_confirm_set(H, T, counters, hitting_sets, S, crit, uncov, violating_vertices);
#pragma omp taskwait // Don't proceed until all the tasks are complete
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        if (not count_only) {
            Hypergraph::Edge result;
            while (hitting_sets.try_dequeue(result)) {
                Htrans.add_edge(result);
            }
        }

        BOOST_LOG_TRIVIAL(info) << "pRS complete: " << counters.mhses_found << " MHSes found, " << counters.iterations << " iterations, " << counters.violators << " violating verts, " << counters.critical_fails << " critical check failures, " << counters.update_loops << " update loops.";

        return Htrans;
    };

    bool RSAlgorithm::any_edge_critical_after_i(Hypergraph::EdgeIndex i,
                                                const Hypergraph::Edge& S,
                                                const Hypergraph& crit) {
        /*
          Return true if any vertex in S has its first critical edge
          after i.
        */
        Hypergraph::EdgeIndex w = S.find_first();
        while (w != Hypergraph::Edge::npos) {
            Hypergraph::EdgeIndex first_crit_edge = crit[w].find_first();
            if (first_crit_edge >= i) {
                return true;
            }
            w = S.find_next(w);
        }

        return false;
    };

    void RSAlgorithm::extend_or_confirm_set(const Hypergraph& H,
                                            const Hypergraph& T,
                                            RSCounters& counters,
                                            Hypergraph::EdgeQueue& hitting_sets,
                                            Hypergraph::Edge& S,
                                            Hypergraph& crit,
                                            Hypergraph::Edge& uncov,
                                            const Hypergraph::Edge& violating_vertices) const {
        ++counters.iterations;

        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, get an uncovered edge
        Hypergraph::EdgeIndex search_edge = uncov.find_first(); // Just use the first set in uncov
        Hypergraph::Edge e = H[search_edge];

        // Store the indices in the edge for iteration
        Hypergraph::Edge new_violating_vertices (H.num_verts());
        std::deque<Hypergraph::EdgeIndex> search_indices;
        Hypergraph::EdgeIndex v = e.find_first();
        while (v != Hypergraph::Edge::npos) {
            // If v is already known violating, we skip it entirely
            bool known_violating = violating_vertices.test(v);
            if (not known_violating and vertex_would_violate(crit, uncov, H, T, S, v)) {
                // Otherwise, if it would violate, we mark it
                new_violating_vertices.set(v);
                ++counters.violators;
            } else if (not known_violating) {
                // And if not, we will consider it in the loop
                search_indices.push_front(v);
            }
            v = e.find_next(v);
        }

        // Loop over vertices in that edge (in reverse order!)
        for (auto& v: search_indices) {
            ++counters.update_loops;
            hsetmap critmark = update_crit_and_uncov(crit, uncov, H, T, S, v);

            // Check whether any vertex in S has its first critical
            // edge after the search edge
            if (any_edge_critical_after_i(search_edge, S, crit)) {
                ++counters.critical_fails;
                SHDAlgorithm::restore_crit_and_uncov(crit, uncov, S, critmark, v);
                continue;
            }

            // if new_uncov is empty, adding v to S makes a hitting set
            S.set(v);

            if (uncov.none()) {
                // In this case, S is a valid hitting set, so we store it
                ++counters.mhses_found;
                if (not count_only) {
                    hitting_sets.enqueue(S);
                }
            } else if (cutoff_size == 0 or S.count() < cutoff_size) {
                // In this case, S is not yet a hitting set but is not too large either
                if (counters.tasks_waiting < 4 and uncov.size() > 2) {
                    // Spawn a new task if the queue is getting low, but
                    // don't waste time with small jobs.
                    // Each one gets its own copy of S, CAND, crit, and uncov
                    ++counters.tasks_waiting;
                    Hypergraph::Edge new_S = S;
                    Hypergraph new_crit = crit;
                    Hypergraph::Edge new_uncov = uncov;
                    Hypergraph::Edge new_viol = violating_vertices | new_violating_vertices;
#pragma omp task shared(H, T, counters, hitting_sets) // Start a new task
                    {
                    --counters.tasks_waiting;
                    extend_or_confirm_set(H, T, counters, hitting_sets, new_S, new_crit, new_uncov, new_viol);
                }
            } else {
                    extend_or_confirm_set(H, T, counters, hitting_sets, S, crit, uncov, violating_vertices | new_violating_vertices);
                }
        }

            // Update crit, uncov, and S, then proceed to the next vertex
            S.reset(v);
            SHDAlgorithm::restore_crit_and_uncov(crit, uncov, S, critmark, v);
    }
};
}
