/**
   C++ implementation of the "fast and simple parallel algorithm" or Boros and Makino (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

   All references are to "A fast and simple parallel algorithm for the monotone
   duality problem", E. Boros and K. Makino. doi: 10.1007/978-3-642-02927-1_17.
**/

#include "bm.hpp"

#include "fk-base.hpp"
#include "hypergraph.hpp"

#include <atomic>
#include <cassert>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    // ugh globals
    std::atomic<bool> found_new_hs;

    Hypergraph l4_full_cover(const Hypergraph& H,
                             const bitset& base_edge) {
        /**
           Find a full cover of the dual of H from the given base_edge
           in accordance with lemma 4 of BM.
         **/
        Hypergraph C (H.num_verts());
        bitset V = H.verts_covered();

        for (auto& edge: H) {
            bitset intersection = edge & base_edge;
            hindex i = intersection.find_first();
            while (i != bitset::npos) {
                bitset new_edge = V - edge;
                new_edge.set(i);
                C.add_edge(new_edge, false);
                i = intersection.find_next(i);
            }
        }

        return C;
    }

    Hypergraph l5_full_cover(const Hypergraph& H,
                             const bitset& base_transversal) {
        /**
           Find a full cover of the dual of H from the given base_transversal
           in accordance with lemma 5 of BM.
         **/
        Hypergraph C (H.num_verts());
        bitset V = H.verts_covered();

        C.add_edge(base_transversal, false);

        hindex i = base_transversal.find_first();
        while (i != bitset::npos) {
            V.reset(i);
            C.add_edge(V, false);
            V.set(i);
            i = base_transversal.find_next(i);
        }

        return C;
    }

    bitset find_missed_edge(const Hypergraph& H,
                            const bitset& I) {
        /**
           Search for an edge of H which does not intersect I.
           If found, return it; if not, return an empty edge as a signal.
         **/

        for (auto& edge: H) {
            if (not edge.intersects(I)) {
                return edge;
            }
        }

        // If we make it this far, no edge was found
        bitset empty_edge (H.num_verts());
        return empty_edge;
    }

    bitset find_subset_edge(const Hypergraph& H,
                            const bitset& I) {
        /**
           Search for an edge of H which is a subset of I.
           If found, return it; if not, return an empty edge as a signal.
         **/

        for (auto& edge: H) {
            if (edge.is_subset_of(I)) {
                return edge;
            }
        }

        // If we make it this far, no edge was found
        bitset empty_edge (H.num_verts());
        return empty_edge;
    }

    bitset bm_find_new_hs_fork(const Hypergraph& H,
                               const Hypergraph& G,
                               const Hypergraph& C) {
        /**
           Find a new hitting set of H with respect to G if possible,
           splitting the work over the full cover C of Tr(H).
         **/

        bitset new_hs (H.num_verts());
        bitset V = C.verts_covered();

        for (auto c: C) {
            // TODO: Some un-aborted tasks may waste time on deep
            // call trees :sadface:
#pragma omp task shared(H, G, found_new_hs, new_hs)
            {
                assert(c.is_subset_of(V));
#pragma omp flush (found_new_hs)
                if (not found_new_hs) {
                    bitset task_result = bm_find_new_hs(H, G, c);
#pragma omp critical (BM_fork)
                    if (task_result.any()) {
                        found_new_hs = true;
#pragma omp flush (found_new_hs)

                        new_hs = task_result;
                    }
                }
            }
        }
#pragma omp taskwait
        return new_hs;
    }

    bitset bm_find_new_hs(const Hypergraph& H,
                          const Hypergraph& G,
                          const bitset& c) {
        /**
           Find a new hitting set of H^c with respect to G_c if possible.
           If not, return an empty edge as a signal.
         **/

        // Construct the reduced hypergraphs
        Hypergraph Hc = H.contraction(c, false);
        Hypergraph Gc = G.restriction(c);

        BOOST_LOG_TRIVIAL(trace) << "Starting transversal build with |Hc| = " << Hc.num_edges()
                                 << " and |Gc| = " << Gc.num_edges();

        // Initialize a candidate hitting set to store intermediate results
        bitset new_hs = Hc.verts_covered();

        // Step 1: Initialize Gc if it is empty by returning the set of all vertices
        if (Gc.num_edges() == 0) {
            // If any edge of Hc is empty, this is a dead end
            for (const auto& e: Hc) {
                if (e.none()) {
                    new_hs.reset();
                    return new_hs;
                }
            }

            // Otherwise, the support of Hc is a new hitting set
            return new_hs;
        }

        // Step 2: Handle |Gc| = 1
        if (Gc.num_edges() == 1) {
            assert(Hc.num_edges() != 0);
            // Check whether H has a singleton for every vertex it covers
            bitset Hc_verts_to_cover = Gc[0];
            for (const auto& e: Hc) {
                if (e.count() == 1) {
                    Hc_verts_to_cover -= e;
                }
            }

            if (Hc_verts_to_cover.none()) {
                // If so, this is a dead end
                return bitset(Hc.num_verts());
            } else {
                // If not, we choose some vertex that was hit by Gc but was
                // not a singleton in Hc
                hindex uncovered_vertex = Hc_verts_to_cover.find_first();
                new_hs.reset(uncovered_vertex);
                return new_hs;
            }

            // We definitely shouldn't ever be here!
            throw std::runtime_error("invalid state in |Gc|=1 case.");
        }

        // Step 3: Split up subcases using a full cover
        // Construct I according to lemmas 7 and 8
        bitset I = Gc.vertices_with_degree_above_threshold(0.5);

        // Per lemma 7, look for an edge that does not intersect I
        bitset missed_edge = find_missed_edge(Hc, I);
        // If found, form a full cover
        if (missed_edge.any()) {
            Hypergraph C = l4_full_cover(Hc, missed_edge);
            new_hs = bm_find_new_hs_fork(H, G, C);
            return new_hs;
        }

        // Per lemma 8, look for a transversal that covers I
        bitset subtrans_edge = find_subset_edge(Gc, I);
        // If found, form a full cover
        if (subtrans_edge.any()) {
            Hypergraph C = l5_full_cover(Hc, subtrans_edge);
            new_hs = bm_find_new_hs_fork(H, G, C);
            return new_hs;
        }

        // If we reach this point, I itself is a new HS
        return I;
    }

    Hypergraph bm_transversal(const Hypergraph& H,
                              const size_t num_threads) {
        BOOST_LOG_TRIVIAL(debug) << "Starting BM with " << num_threads
                                 << " threads. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";

        // Set up inputs
        Hypergraph Hmin = H.minimization();
        Hypergraph G (H.num_verts());
        bitset V = Hmin.verts_covered();

        // Initialize using any HS we can find
        bitset first_hs = fk_minimize_new_hs(Hmin, G, V);
        G.add_edge(first_hs);

        // Grow G until it covers all vertices
        bool G_has_full_coverage = false;
        while (not G_has_full_coverage) {
            bitset new_hs = V - fk_coverage_condition_check(H, G);
            if (new_hs.is_proper_subset_of(V)) {
                bitset new_mhs = fk_minimize_new_hs(Hmin, G, new_hs);
                G.add_edge(new_mhs);
            } else {
                G_has_full_coverage = true;
            }
        }

        // Apply the BM algorithm repeatedly, generating new transversals
        // until duality is confirmed
        bool still_searching_for_transversals = true;
#pragma omp parallel shared(Hmin, G, found_new_hs) num_threads(num_threads)
#pragma omp master
        while (still_searching_for_transversals) {
            bitset new_hs = bm_find_new_hs(Hmin, G, Hmin.verts_covered());

            if (new_hs.none()) {
                still_searching_for_transversals = false;
            } else {
                found_new_hs = false;
                bitset new_mhs = fk_minimize_new_hs(Hmin, G, new_hs);
                BOOST_LOG_TRIVIAL(trace) << "New hitting set:"
                                         << "\nS:\t" << new_hs
                                         << "\nMHS:\t" << new_mhs;
                G.add_edge(new_mhs, true);
                BOOST_LOG_TRIVIAL(debug) << "New |G|: " << G.num_edges();
            }
        }

        return G;
    }
}
