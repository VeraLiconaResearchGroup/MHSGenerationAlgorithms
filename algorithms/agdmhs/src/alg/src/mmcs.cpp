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
#include <omp.h>

#include <boost/dynamic_bitset.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    Hypergraph mmcs_transversal(const Hypergraph& H,
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

        // Eligible vertices
        bitset CAND (H.num_verts());
        CAND.set(); // Initially full

        // Which edges each vertex is critical for
        Hypergraph crit (H.num_edges(), H.num_verts());

        // Which edges are uncovered
        bitset uncov (H.num_edges());
        uncov.set(); // Initially full

        // RUN ALGORITHM
        {
#pragma omp parallel shared(H, HittingSets)
#pragma omp single
            mmcs_extend_or_confirm_set(HittingSets, H, S, CAND, crit, uncov, cutoff_size);
#pragma omp taskwait
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        bitset result;
        while (HittingSets.try_dequeue(result)) {
            Htrans.add_edge(result);
        }

        return Htrans;
    }

    void mmcs_extend_or_confirm_set(bsqueue& HittingSets,
                                    const Hypergraph& H,
                                    const bitset S,
                                    const bitset CAND,
                                    const Hypergraph crit,
                                    const bitset uncov,
                                    const size_t cutoff_size){
        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(CAND.any()); // CAND cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, get an uncovered edge and remove its elements from CAND
        // TODO: Implement the optimization of Murakami and Uno
        bitset e = H[uncov.find_first()]; // Just use the first set in uncov
        bitset C = CAND & e; // intersection
        bitset newCAND = CAND & (~e); // difference

        // Test all the vertices in C
        hindex v = C.find_first();
        while (v != bitset::npos) {
            // Update uncov and crit by iterating over edges containing the vertex
            Hypergraph new_crit = crit;
            bitset new_uncov = uncov;
            update_crit_and_uncov(new_crit, new_uncov, H, v);

            // Construct the new candidate hitting set
            bitset newS = S;
            newS[v] = true;

            // Test the minimality condition on newS
            bool is_minimal = true;
            hindex w = newS.find_first();
            while (w != bitset::npos and is_minimal) {
                if (new_crit[w].none()) {
                    is_minimal = false;
                }
                w = newS.find_next(w);
            }

            // If we made it this far, minimality holds, so we process newS
            if (is_minimal and new_uncov.none() and (cutoff_size == 0 or newS.count() <= cutoff_size)) {
                // In this case, newS is a valid hitting set, so we store it
                HittingSets.enqueue(newS);
            } else if (is_minimal and newCAND.count() > 0 and (cutoff_size == 0 or newS.count() < cutoff_size)) {
                // In this case, newS is not yet a hitting set but is not too large either
#pragma omp task untied shared(H, HittingSets)
                mmcs_extend_or_confirm_set(HittingSets, H, newS, newCAND, new_crit, new_uncov, cutoff_size);
            }

            // Update newCAND and proceed to new vertex
            newCAND[v] = true;
            v = C.find_next(v);
        }
    }
}
