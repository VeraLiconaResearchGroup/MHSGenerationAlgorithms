/**
   C++ implementation of the RS algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "rs.hpp"

#include "hypergraph.hpp"
#include "shd-base.hpp"

#include <cassert>
#include <omp.h>

#include <boost/dynamic_bitset.hpp>

// TODO: Input specifications with <cassert>
namespace agdmhs {
    static bsqueue HittingSets;

    static bool rs_vertex_is_violating(const hindex& v,
                             const Hypergraph& H,
                             const bitset& S,
                             const Hypergraph& crit) {
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

        return minimality_check_failed;
    };

    static bool rs_any_edge_critical_after_i(const hindex& i,
                                   const bitset& S,
                                   const Hypergraph& crit) {
        bool bad_edge_found = false;
        hindex w = S.find_first();
        while (w != bitset::npos and not bad_edge_found) {
            hindex first_crit_edge = crit[w].find_first();
            if (first_crit_edge >= i) {
                bad_edge_found = true;
            }
            w = S.find_next(w);
        }

        return bad_edge_found;
    };

    static void rs_extend_or_confirm_set(const Hypergraph& H,
                                         const bitset S,
                                         const Hypergraph crit,
                                         const bitset uncov,
                                         const size_t cutoff_size) {
        // Input specification
        assert(uncov.any()); // uncov cannot be empty
        assert(cutoff_size == 0 or S.count() < cutoff_size); // If we're using a cutoff, S must not be too large

        // Otherwise, get an uncovered edge
        hindex i = uncov.find_first(); // Just use the first set in uncov

        // Loop over vertices in that edge
        hindex v = H[i].find_first();
        while (v != bitset::npos) {
            // Check preconditions
            if (rs_vertex_is_violating(v, H, S, crit)) {
                v = H[i].find_next(v);
                continue;
            }

            Hypergraph new_crit = crit;
            bitset new_uncov = uncov;
            update_crit_and_uncov(new_crit, new_uncov, H, v);

            if (rs_any_edge_critical_after_i(i, S, new_crit)) {
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
#pragma omp task untied shared(H)
                rs_extend_or_confirm_set(H, newS, new_crit, new_uncov, cutoff_size);
            }

            v = H[i].find_next(v);
        }
    };

    Hypergraph rs_transversal(const Hypergraph& H,
                              const size_t num_threads,
                              const size_t cutoff_size) {
        // SET UP INTERNAL VARIABLES
        // Number of threads for parallelization
        omp_set_num_threads(num_threads);

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
#pragma omp parallel shared(H)
#pragma omp single
            rs_extend_or_confirm_set(H, S, crit, uncov, cutoff_size);
#pragma omp taskwait
        }

        // Gather results
        Hypergraph Htrans(H.num_verts());
        bitset result;
        while (HittingSets.try_dequeue(result)) {
            Htrans.add_edge(result);
        }

        return Htrans;
    };
}
