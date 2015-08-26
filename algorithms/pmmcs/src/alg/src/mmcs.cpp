/**
   C++ implementation of the MMCS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "mmcs.hpp"

#include "concurrentqueue.h"
#include "hypergraph.hpp"

#include <omp.h>

#include <boost/dynamic_bitset.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    moodycamel::ConcurrentQueue<bitset> HittingSets;

    Hypergraph mmcs_transversal(const Hypergraph& H, const size_t num_threads, const size_t cutoff_size) {
        // SET UP INTERNAL VARIABLES
        // Number of threads for parallelization
        omp_set_num_threads(num_threads);

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
#pragma omp parallel shared(H)
#pragma omp single
            extend_or_confirm_set(H, S, CAND, crit, uncov, cutoff_size);
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

    void extend_or_confirm_set(Hypergraph H,
                               bitset S,
                               bitset CAND,
                               Hypergraph crit,
                               bitset uncov,
                               size_t cutoff_size){
        // if uncov is empty, S is a hitting set
        if (uncov.none()) {
            HittingSets.enqueue(S);
            return;
        }

        // If CAND is empty or S is too big, S cannot be extended, so we're done
        if (CAND.none() or (cutoff_size > 0 and S.count() >= cutoff_size)) {
            return;
        }

        // Otherwise, get an uncovered edge and remove its elements from CAND
        // TODO: Implement the optimization of Murakami and Uno
        bitset e = H[uncov.find_first()]; // Just use the first set in uncov
        bitset C = CAND & e; // intersection
        CAND = CAND & (~e); // difference

        // Test all the vertices in C
        auto vert_index = C.find_first();
        while (vert_index != bitset::npos) {
            // Update uncov and crit by iterating over edges containing the vertex
            bitset new_uncov = uncov;
            Hypergraph new_crit = crit;
            for (hindex edge_index = 0; edge_index < H.num_edges(); ++edge_index) {
                // If the vertex is in this edge, proceed
                if (H[edge_index][vert_index]) {
                    // Remove e from all crit[v]'s
                    for (hindex v = 0; v < H.num_verts(); ++v) {
                        new_crit[v][edge_index] = false;
                    }

                    // If this edge was new_uncovered, it is no longer, but v is now new_critical for it
                    if (new_uncov[edge_index] == true) {
                        new_uncov[edge_index] = false;
                        new_crit[vert_index][edge_index] = true;
                    }
                }
            }

            // Construct the new candidate hitting set
            bitset newS = S;
            newS[vert_index] = true;

            // Test the minimality condition on newS
            bool is_minimal = true;
            auto v = newS.find_first();
            while (v != bitset::npos and is_minimal) {
                if (new_crit[v].none()) {
                    is_minimal = false;
                }
                v = newS.find_next(v);
            }

            // If we made it this far, minimality holds, so we process newS
            if (is_minimal and new_uncov.none() and (cutoff_size == 0 or newS.count() <= cutoff_size)) {
                // In this case, newS is a valid hitting set, so we store it
                HittingSets.enqueue(newS);
            } else if (is_minimal and CAND.count() > 0 and (cutoff_size == 0 or newS.count() < cutoff_size)) {
                // In this case, newS is not yet a hitting set but is not too large either
#pragma omp task untied
                extend_or_confirm_set(H, newS, CAND, new_crit, new_uncov, cutoff_size);
            }

            // Update CAND and proceed to new vertex
            CAND[vert_index] = true;
            vert_index = C.find_next(vert_index);
        }
    }
}
