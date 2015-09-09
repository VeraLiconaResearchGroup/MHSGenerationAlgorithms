/**
   C++ implementation of the RS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "rs.hpp"

#include "hypergraph.hpp"
#include "shd-base.hpp"

#include <cassert>
#include <iostream>
#include <omp.h>

#include <boost/dynamic_bitset.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    Hypergraph rs_transversal(const Hypergraph& H,
                              const size_t num_threads,
                              const size_t cutoff_size) {
        // SET UP INTERNAL VARIABLES
        // Number of threads for parallelization
        omp_set_num_threads(num_threads);

        // Results queue
        bsqueue HittingSets;

        // Candidate hitting set
        bitset S (H.num_verts());
        S.reset(); // Initially empty

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are uncovered
        bitset uncov (H.num_edges());
        uncov.set(); // Initially full

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, HittingSets)
#pragma omp single
            rs_extend_or_confirm_set(HittingSets, H, S, crit, uncov, cutoff_size);
#pragma omp taskwait
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        bitset result;
        while (HittingSets.try_dequeue(result)) {
            // TODO: remove for speed
            assert(H.is_transversed_by(result));
            Htrans.add_edge(result);
        }

        return Htrans;
    }

    void rs_extend_or_confirm_set(bsqueue& HittingSets,
                                  const Hypergraph& H,
                                  const bitset S,
                                  const Hypergraph crit,
                                  const bitset uncov,
                                  const size_t cutoff_size,
                                  const unsigned call_depth) {
        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, get an uncovered edge
        hindex i = uncov.find_first(); // Just use the first set in uncov

        // Loop over vertices in that edge
        hindex v = H[i].find_first();
        while (v != bitset::npos) {
            // Test whether v is violating: specifically, whether there is any
            // vertex in S which would lose all its critical edges to v
            bitset v_edges (H.num_edges());
            for (hindex edge_index = 0; edge_index < H.num_edges(); ++edge_index) {
                if (H[edge_index].test(v)) {
                    v_edges.set(edge_index);
                }
            }

            bool minimality_check_failed = false;
            hindex other_vertex = S.find_first();
            while (other_vertex != bitset::npos and not minimality_check_failed) {
                if (crit[other_vertex].is_subset_of(v_edges)) {
                    minimality_check_failed = true;
                }
                other_vertex = S.find_next(other_vertex);
            }

            if (minimality_check_failed) {
                v = H[i].find_next(v);
                continue;
            }

            // Otherwise, update uncov and crit
            Hypergraph new_crit = crit;
            bitset new_uncov = uncov;
            update_crit_and_uncov(new_crit, new_uncov, H, v);

            // Test the criticality condition
            bool all_edges_crit_before_i = true;
            hindex w = S.find_first();
            while (w != bitset::npos and all_edges_crit_before_i) {
                hindex first_crit_edge = new_crit[w].find_first();
                if (first_crit_edge >= i) {
                    all_edges_crit_before_i = false;
                }
                w = S.find_next(w);
            }

            if (not all_edges_crit_before_i) {
                v = H[i].find_next(v);
                continue;
            }

            // if new_uncov is empty, adding v to S makes a hitting set
            bitset newS = S;
            newS.set(v);

            if (new_uncov.none()) {
                HittingSets.enqueue(newS);
                v = H[i].find_next(v);
                continue;
            }

            // After this point, we'll be considering extending newS even more.
            // If we're using a cutoff, this requires more room.
            if (cutoff_size == 0 or newS.count() < cutoff_size) {
#pragma omp task untied shared(H, HittingSets)
                rs_extend_or_confirm_set(HittingSets, H, newS, new_crit, new_uncov, cutoff_size, call_depth + 1);
            }

            v = H[i].find_next(v);
        }

    }
}
