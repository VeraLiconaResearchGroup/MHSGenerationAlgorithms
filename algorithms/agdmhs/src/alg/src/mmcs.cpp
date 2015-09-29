/**
   C++ implementation of the MMCS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "mmcs.hpp"

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
    std::atomic<unsigned> mmcs_iterations;
    std::atomic<unsigned> mmcs_violators;

    std::atomic<unsigned> mmcs_tasks_waiting;

    static void mmcs_extend_or_confirm_set(const Hypergraph& H,
                                           const Hypergraph& T,
                                           bitset& S,
                                           bitset& CAND,
                                           Hypergraph& crit,
                                           bitset& uncov,
                                           const size_t cutoff_size) {
        ++mmcs_iterations;

        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(CAND.any()); // CAND cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, we prune the vertices to search
        // Per M+U, we find the edge e with smallest intersection with CAND
        hindex search_edge = uncov.find_first();
        bitset e = H[search_edge];
        while (search_edge != bitset::npos) {
            if ((H[search_edge] & CAND).count() < (e & CAND).count()) {
                e = H[search_edge];
            }
            search_edge = uncov.find_next(search_edge);
        }

        // Then consider vertices lying in the intersection of e with CAND
        bitset C = CAND & e; // intersection
        CAND -= e; // difference

        // Store the indices in C in descending order for iteration
        std::deque<hindex> Cindices;
        hindex v = C.find_first();
        while (v != bitset::npos) {
            Cindices.push_front(v);
            v = C.find_next(v);
        }

        // Test all the vertices in C (in descending order)
        for (auto& v: Cindices) {
            // Update uncov and crit by iterating over edges containing the vertex
            hsetmap critmark;
            try {
                critmark = update_crit_and_uncov(crit, uncov, H, T, S, v);
            }
            catch (vertex_violating_exception& e) {
                // Update CAND and proceed to new vertex
                ++mmcs_violators;
                CAND.set(v);
                continue;
            }

            // Construct the new candidate hitting set
            S.set(v);

            // If we made it this far, S is a new candidate, which we process
            if (uncov.none() and (cutoff_size == 0 or S.count() <= cutoff_size)) {
                // In this case, S is a valid hitting set, so we store it
                HittingSets.enqueue(S);
            } else if (CAND.count() > 0 and (cutoff_size == 0 or S.count() < cutoff_size)) {
                // In this case, S is not yet a hitting set but is not too large either
                if (mmcs_tasks_waiting < 4 and uncov.size() > 2) {
                    // Spawn a new task if the queue is getting low, but
                    // don't waste time with small jobs.
                    // Each one gets its own copy of S, CAND, crit, and uncov
                    ++mmcs_tasks_waiting;
                    bitset new_S = S;
                    bitset new_CAND = CAND;
                    Hypergraph new_crit = crit;
                    bitset new_uncov = uncov;
#pragma omp task shared(H, T) // Start a new task
                    {
                    --mmcs_tasks_waiting;
                    mmcs_extend_or_confirm_set(H, T, new_S, new_CAND, new_crit, new_uncov, cutoff_size);
                    }
                } else {
                    // Stay in this thread otherwise
                    mmcs_extend_or_confirm_set(H, T, S, CAND, crit, uncov, cutoff_size);
                }
            }

            // Update CAND, crit, uncov, and S, then proceed to new vertex
            CAND.set(v);
            S.reset(v);
            restore_crit_and_uncov(crit, uncov, S, critmark, v);
        }
    }

    Hypergraph mmcs_transversal(const Hypergraph& H,
                                const size_t num_threads,
                                const size_t cutoff_size) {
        // SET UP INTERNAL VARIABLES
        // Debugging counters
        mmcs_iterations = 0;
        mmcs_violators = 0;

        // Candidate hitting set
        bitset S (H.num_verts());
        S.reset(); // Initially empty

        // Eligible vertices
        bitset CAND (H.num_verts());
        CAND.set(); // Initially full

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are uncovered
        bitset uncov (H.num_edges());
        uncov.set(); // Initially full

        // Tranpose of H
        Hypergraph T = H.transpose();

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, T) num_threads(num_threads) // Don't create thread-local copies of H and T
#pragma omp master // Only spawn the computation once
            mmcs_extend_or_confirm_set(H, T, S, CAND, crit, uncov, cutoff_size);
#pragma omp taskwait // Don't proceed until all the tasks are complete
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        bitset result;
        while (HittingSets.try_dequeue(result)) {
            Htrans.add_edge(result);
        }

        BOOST_LOG_TRIVIAL(info) << "pMMCS complete: " << mmcs_iterations << " iterations, " << mmcs_violators << " violating vertices.";

        return Htrans;
    }
}
