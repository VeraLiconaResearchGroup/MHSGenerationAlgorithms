/**
   C++ implementation of the "fast and simple parallel algorithm" or Boros and Makino (library)
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

/**
   All references are to "A fast and simple parallel algorithm for the monotone
   duality problem", E. Boros and K. Makino. doi: 10.1007/978-3-642-02927-1_17.
**/

#include "bm.hpp"
#include "concurrentqueue.h"
#include "fk-algorithm.hpp"
#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"

#include <cassert>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    ParBMAlgorithm::ParBMAlgorithm ():
        num_threads(0)
    {};

    ParBMAlgorithm::ParBMAlgorithm (unsigned num_threads):
        num_threads(num_threads)
    {};

    Hypergraph ParBMAlgorithm::transversal (const Hypergraph& H) const {
        BOOST_LOG_TRIVIAL(debug) << "Starting BM with " << num_threads
                                 << " threads. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";

        // Set up inputs
        Hypergraph Hmin = H.minimization();
        Hypergraph G (H.num_verts());
        Hypergraph::Edge V = Hmin.verts_covered();

        // Initialize using any HS we can find
        Hypergraph::Edge first_hs = FKAlgorithm::minimize_new_hs(Hmin, G, V);
        G.add_edge(first_hs);

        // Grow G until it covers all vertices
        bool G_has_full_coverage = false;
        while (not G_has_full_coverage) {
            Hypergraph::Edge new_hs = V - coverage_condition_check(H, G);
            if (new_hs.is_proper_subset_of(V)) {
                Hypergraph::Edge new_mhs = FKAlgorithm::minimize_new_hs(Hmin, G, new_hs);
                G.add_edge(new_mhs);
            } else {
                G_has_full_coverage = true;
            }
        }

        // Apply the BM algorithm repeatedly, generating new transversals
        // until duality is confirmed
        bool still_searching_for_transversals = true;
        Hypergraph::EdgeQueue new_hses, new_mhses;
#pragma omp parallel shared(Hmin, G, new_hses, new_mhses) num_threads(num_threads)
#pragma omp master
        while (still_searching_for_transversals) {
            find_new_hses(Hmin, G, Hmin.verts_covered(), new_hses);

            if (new_hses.size_approx() == 0) {
                still_searching_for_transversals = false;
            } else {
                minimize_new_hses(Hmin, G, new_hses, new_mhses);

                Hypergraph::Edge new_mhs;
                while (new_mhses.try_dequeue(new_mhs)) {
                    // The results will all be inclusion-minimal, but
                    // there may be some overlap. Thus, we try to add
                    // them...
                    try {
                        G.add_edge(new_mhs, true);
                    }
                    // But ignore any minimality_violated_exception
                    // that is thrown.
                    catch (minimality_violated_exception& e) {}
                }
                BOOST_LOG_TRIVIAL(debug) << "New |G|: " << G.num_edges();
            }
        }

        return G;
    }

    Hypergraph ParBMAlgorithm::l4_full_cover (const Hypergraph& H,
                                              const Hypergraph::Edge& base_edge) {
        /**
           Find a full cover of the dual of H from the given base_edge
           in accordance with lemma 4 of BM.
        **/
        Hypergraph C (H.num_verts());
        Hypergraph::Edge V = H.verts_covered();

        for (auto& edge: H) {
            Hypergraph::Edge intersection = edge & base_edge;
            Hypergraph::EdgeIndex i = intersection.find_first();
            while (i != Hypergraph::Edge::npos) {
                Hypergraph::Edge new_edge = V - edge;
                new_edge.set(i);
                C.add_edge(new_edge, false);
                i = intersection.find_next(i);
            }
        }

        return C;
    }

    Hypergraph ParBMAlgorithm::l5_full_cover (const Hypergraph& H,
                                              const Hypergraph::Edge& base_transversal) {
        /**
           Find a full cover of the dual of H from the given base_transversal
           in accordance with lemma 5 of BM.
        **/
        Hypergraph C (H.num_verts());
        Hypergraph::Edge V = H.verts_covered();

        C.add_edge(base_transversal, false);

        Hypergraph::EdgeIndex i = base_transversal.find_first();
        while (i != Hypergraph::Edge::npos) {
            V.reset(i);
            C.add_edge(V, false);
            V.set(i);
            i = base_transversal.find_next(i);
        }

        return C;
    }

    Hypergraph::Edge ParBMAlgorithm::find_missed_edge (const Hypergraph& H,
                                                       const Hypergraph::Edge& I) {
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
        Hypergraph::Edge empty_edge (H.num_verts());
        return empty_edge;
    }

    Hypergraph::Edge ParBMAlgorithm::find_subset_edge (const Hypergraph& H,
                                                       const Hypergraph::Edge& I) {
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
        Hypergraph::Edge empty_edge (H.num_verts());
        return empty_edge;
    }

    void ParBMAlgorithm::find_new_hses_fork (const Hypergraph& H,
                                             const Hypergraph& G,
                                             const Hypergraph& C,
                                             Hypergraph::EdgeQueue& results) const {
        /**
           Find any new hitting sets of H with respect to G,
           splitting the work over the full cover C of Tr(H) and queueing
           the results.
        **/

        Hypergraph::Edge V = C.verts_covered();

        for (auto c: C) {
#pragma omp task shared(H, G, results)
            {
                assert(c.is_subset_of(V));
                find_new_hses(H, G, c, results);
            }
        }
#pragma omp taskwait
    }

    void ParBMAlgorithm::find_new_hses (const Hypergraph& H,
                                        const Hypergraph& G,
                                        const Hypergraph::Edge& c,
                                        Hypergraph::EdgeQueue& results) const {
        /**
           Find any new hitting sets of H^c with respect to G_c, queueing the
           results.
        **/

        // Construct the reduced hypergraphs
        Hypergraph Hc = H.contraction(c, false);
        Hypergraph Gc = G.restriction(c);

        BOOST_LOG_TRIVIAL(trace) << "Starting transversal build with |Hc| = " << Hc.num_edges()
                                 << " and |Gc| = " << Gc.num_edges();

        // Initialize a candidate hitting set to store intermediate results
        Hypergraph::Edge new_hs = Hc.verts_covered();

        // Step 1: Initialize Gc if it is empty by returning the set of all vertices
        if (Gc.num_edges() == 0) {
            // If any edge of Hc is empty, this is a dead end
            for (const auto& e: Hc) {
                if (e.none()) {
                    return;
                }
            }

            // Otherwise, the support of Hc is a new hitting set
            results.enqueue(new_hs);
            return;
        }

        // Step 2: Handle |Gc| = 1
        if (Gc.num_edges() == 1) {
            assert(Hc.num_edges() != 0);
            // Check whether H has a singleton for every vertex it covers
            Hypergraph::Edge Hc_verts_to_cover = Gc[0];
            for (const auto& e: Hc) {
                if (e.count() == 1) {
                    Hc_verts_to_cover -= e;
                }
            }

            if (Hc_verts_to_cover.none()) {
                // If so, this is a dead end
                return;
            } else {
                // If not, we choose some vertex that was hit by Gc but was
                // not a singleton in Hc
                Hypergraph::EdgeIndex uncovered_vertex = Hc_verts_to_cover.find_first();
                new_hs.reset(uncovered_vertex);
                results.enqueue(new_hs);
                return;
            }

            // We definitely shouldn't ever be here!
            throw std::runtime_error("invalid state in |Gc|=1 case.");
        }

        // Step 3: Split up subcases using a full cover
        // Construct I according to lemmas 7 and 8
        Hypergraph::Edge I = Gc.vertices_with_degree_above_threshold(0.5);

        // Per lemma 7, look for an edge that does not intersect I
        Hypergraph::Edge missed_edge = find_missed_edge(Hc, I);
        // If found, form a full cover
        if (missed_edge.any()) {
            Hypergraph C = l4_full_cover(Hc, missed_edge);
            find_new_hses_fork(H, G, C, results);
            return;
        }

        // Per lemma 8, look for a transversal that covers I
        Hypergraph::Edge subtrans_edge = find_subset_edge(Gc, I);
        // If found, form a full cover
        if (subtrans_edge.any()) {
            Hypergraph C = l5_full_cover(Hc, subtrans_edge);
            find_new_hses_fork(H, G, C, results);
            return;
        }

        // If we reach this point, I itself is a new HS
        results.enqueue(I);
        return;
    }

    Hypergraph::Edge ParBMAlgorithm::minimize_new_hs (const Hypergraph& H,
                                                      Hypergraph::Edge new_hs) {
        /**
           Given a hypergraph H and a new hitting set new_hs, find an
           inclusion-minimal subset of new_hs which is still a hitting
           set of H.
        **/

        BOOST_LOG_TRIVIAL(trace) << "Attempting minimization of"
                                 << "\nS:\t" << new_hs;

        // Input validation
        assert(H.is_transversed_by(new_hs));

        // Iterate through the vertices of new_hs, checking whether they can be
        // removed
        Hypergraph::EdgeIndex v = new_hs.find_first();
        while (v != Hypergraph::Edge::npos) {
            new_hs.reset(v);
            if (not H.is_transversed_by(new_hs)) {
                new_hs.set(v);
            }

            v = new_hs.find_next(v);
        }

        BOOST_LOG_TRIVIAL(trace) << "Minimized to:"
                                 << "\nS:\t" << new_hs;

        return new_hs;
    }

    void ParBMAlgorithm::minimize_new_hses (const Hypergraph& H,
                                            const Hypergraph& G,
                                            Hypergraph::EdgeQueue& new_hses,
                                            Hypergraph::EdgeQueue& new_mhses) const {
        Hypergraph::Edge new_hs;
        while (new_hses.try_dequeue(new_hs)) {
#pragma omp task shared(H, G, new_hses, new_mhses)
            {
                Hypergraph::Edge new_mhs = minimize_new_hs(H, new_hs);
                new_mhses.enqueue(new_mhs);
            }
#pragma omp taskwait
        }
    }
}
