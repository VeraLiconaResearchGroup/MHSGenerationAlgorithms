/**
   C++ implementation of the MMCS algorithm (library)
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
#include "mmcs.hpp"
#include "shd-algorithm.hpp"

#include <cassert>
#include <deque>
#include <omp.h>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    MMCSAlgorithm::MMCSAlgorithm (unsigned num_threads,
                                  unsigned cutoff_size,
                                  bool count_only):
        num_threads(num_threads), cutoff_size(cutoff_size), count_only(count_only)
    {}

    Hypergraph MMCSAlgorithm::transversal (const Hypergraph& H) const {
        // SET UP INTERNAL VARIABLES
        // Debugging counters
        MMCSCounters counters;

        // Candidate hitting set
        Hypergraph::Edge S (H.num_verts());
        S.reset(); // Initially empty

        // Eligible vertices
        Hypergraph::Edge CAND (H.num_verts());
        CAND.set(); // Initially full

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are uncovered
        Hypergraph::Edge uncov (H.num_edges());
        uncov.set(); // Initially full

        // Tranpose of H
        Hypergraph T = H.transpose();

        // Queue to store hitting sets as they are generated
        Hypergraph::EdgeQueue hitting_sets;

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, T, hitting_sets) num_threads(num_threads) // Don't create thread-local copies of H and T
#pragma omp master // Only spawn the computation once
            extend_or_confirm_set(H, T, counters, hitting_sets, S, CAND, crit, uncov);
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

        BOOST_LOG_TRIVIAL(info) << "pMMCS complete: " << counters.mhses_found << " MHSes found, " << counters.iterations << " iterations, " << counters.violators << " violating vertices, " << counters.update_loops << " update loops.";

        return Htrans;
    }

    void MMCSAlgorithm::extend_or_confirm_set (const Hypergraph& H,
                                               const Hypergraph& T,
                                               MMCSCounters& counters,
                                               Hypergraph::EdgeQueue& hitting_sets,
                                               Hypergraph::Edge& S,
                                               Hypergraph::Edge& CAND,
                                               Hypergraph& crit,
                                               Hypergraph::Edge& uncov) const {
        ++counters.iterations;

        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(CAND.any()); // CAND cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, we prune the vertices to search
        // Per M+U, we find the edge e with smallest intersection with CAND
        Hypergraph::EdgeIndex search_edge = uncov.find_first();
        Hypergraph::Edge e = H[search_edge];
        while (search_edge != Hypergraph::Edge::npos) {
            if ((H[search_edge] & CAND).count() < (e & CAND).count()) {
                e = H[search_edge];
            }
            search_edge = uncov.find_next(search_edge);
        }

        // Then consider vertices lying in the intersection of e with CAND
        Hypergraph::Edge C = CAND & e; // intersection
        CAND -= e; // difference

        // Store the indices in C in descending order for iteration
        std::deque<Hypergraph::EdgeIndex> Cindices;
        Hypergraph::EdgeIndex v = C.find_first();
        while (v != Hypergraph::Edge::npos) {
            Cindices.push_front(v);
            v = C.find_next(v);
        }

        // Record vertices of C which were violating for S
        Hypergraph::Edge violators (H.num_verts());

        // Test all the vertices in C (in descending order)
        for (auto& v: Cindices) {
            ++counters.update_loops;
            // Update uncov and crit by iterating over edges containing the vertex
            if (vertex_would_violate(crit, uncov, H, T, S, v)) {
                // Update CAND and proceed to new vertex
                ++counters.violators;
                violators.set(v);
                continue;
            }

            hsetmap critmark = update_crit_and_uncov(crit, uncov, H, T, S, v);

            // Construct the new candidate hitting set
            S.set(v);

            // If we made it this far, S is a new candidate, which we process
            if (uncov.none() and (cutoff_size == 0 or S.count() <= cutoff_size)) {
                // In this case, S is a valid hitting set, so we store it
                ++counters.mhses_found;
                if (not count_only) {
                    hitting_sets.enqueue(S);
                }
            } else if (CAND.count() > 0 and (cutoff_size == 0 or S.count() < cutoff_size)) {
                // In this case, S is not yet a hitting set but is not too large either
                if (counters.tasks_waiting < 4 and uncov.size() > 2) {
                    // Spawn a new task if the queue is getting low, but
                    // don't waste time with small jobs.
                    // Each one gets its own copy of S, CAND, crit, and uncov
                    ++counters.tasks_waiting;
                    Hypergraph::Edge new_S = S;
                    Hypergraph::Edge new_CAND = CAND;
                    Hypergraph new_crit = crit;
                    Hypergraph::Edge new_uncov = uncov;
#pragma omp task shared(H, T, counters, hitting_sets) // Start a new task
                        {
                            --counters.tasks_waiting;
                            extend_or_confirm_set(H, T, counters, hitting_sets, new_S, new_CAND, new_crit, new_uncov);
                        }
                } else {
                    // Stay in this thread otherwise
                    extend_or_confirm_set(H, T, counters, hitting_sets, S, CAND, crit, uncov);
                }
            }

            // Update CAND, crit, uncov, and S, then proceed to new vertex
            CAND.set(v);
            S.reset(v);
            SHDAlgorithm::restore_crit_and_uncov(crit, uncov, S, critmark, v);
        }

        // Return the violators to CAND before any other run uses it
        CAND |= violators;
    }
}
