/**
   C++ implementation of the FK-A algorithm (library)
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

#include "fk-algorithm.hpp"
#include "fka.hpp"
#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"

#include <cassert>
#include <vector>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    FKAlgorithmA::FKAlgorithmA () {};

    Hypergraph FKAlgorithmA::transversal (const Hypergraph& H) const {
        BOOST_LOG_TRIVIAL(debug) << "Starting FKA. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";
        Hypergraph G (H.num_verts());

        Hypergraph Hmin = H.minimization();
        Hypergraph::Edge V = Hmin.verts_covered();

        bool still_searching_for_transversals = true;
        while (still_searching_for_transversals) {
            Hypergraph::Edge omit_set = find_omit_set(Hmin, G);

            if (omit_set.none() and G.num_edges() > 0) {
                BOOST_LOG_TRIVIAL(debug) << "Received empty omit_set, so we're done.";
                still_searching_for_transversals = false;
            } else {
                Hypergraph::Edge new_hs = V - omit_set;
                Hypergraph::Edge new_mhs = minimize_new_hs(H, G, new_hs);
                BOOST_LOG_TRIVIAL(trace) << "Received witness."
                                         << "\nomit_set:\t" << omit_set
                                         << "\nMHS:\t\t" << new_mhs;
                G.add_edge(new_mhs, true);
                BOOST_LOG_TRIVIAL(debug) << "New G size: " << G.num_edges();
            }
        }

        return G;
    }

    Hypergraph::Edge FKAlgorithmA::find_omit_set (const Hypergraph& F,
                                                  const Hypergraph& G) {
        /**
           Test whether F and G are dual.

           If so, return an empty edge as a notification.
           If not, return a omit_set as per FK.
        **/

        BOOST_LOG_TRIVIAL(trace) << "Beginning run with "
                                 << "|F| = " << F.num_edges()
                                 << " and |G| = " << G.num_edges();

        // Input specification
        assert(F.num_verts() == G.num_verts());

        // Create an empty omit_set to use as temporary storage
        Hypergraph::Edge omit_set (F.num_verts());

        // FK step 1: initialize if G is empty
        if (G.num_edges() == 0) {
            BOOST_LOG_TRIVIAL(trace) << "Returning empty omit_set since G is empty.";
            return omit_set;
        }

        // FK step 2: consistency checks
        // Check 1.1: hitting condition
        omit_set = hitting_condition_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // Check 1.2: same vertices covered
        omit_set = coverage_condition_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // Check 1.3: neither F nor G has edges too large
        omit_set = edge_size_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // Check 2.1: satisfiability count condition
        omit_set = satisfiability_count_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // FK step 3: Check whether F and G are small
        // If either hypergraph is empty, they cannot be dual
        omit_set = small_hypergraphs_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // FK step 4: Recurse

        // Find the most frequently occurring vertex
        Hypergraph::EdgeIndex max_freq_vert = most_frequent_vertex(F, G);

        // Then we compute the split hypergraphs F0, F1, G0, and G1
        std::pair<Hypergraph, Hypergraph> Fsplit, Gsplit;
        Hypergraph F0, F1, G0, G1;

        Fsplit = split_hypergraph_over_vertex(F, max_freq_vert);
        F0 = Fsplit.first;
        F1 = Fsplit.second;

        Gsplit = split_hypergraph_over_vertex(G, max_freq_vert);
        G0 = Gsplit.first;
        G1 = Gsplit.second;

        // We will also need the unions F0∪F1 and G0∪G1
        Hypergraph Fnew = minimized_union(F0, F1);
        Hypergraph Gnew = minimized_union(G0, G1);

        // And, finally, fire up the two recursions
        if (F1.num_edges() > 0 and Gnew.num_edges() > 0) {
            BOOST_LOG_TRIVIAL(trace) << "Side 1 recursion.";

            Hypergraph::Edge omit_set = find_omit_set(F1, Gnew);
            if (omit_set.any()) {
                return omit_set;
            }
        }

        if (Fnew.num_edges() > 0 and G1.num_edges() > 0) {
            BOOST_LOG_TRIVIAL(trace) << "Side 2 recursion.";

            Hypergraph::Edge omit_set = find_omit_set(Fnew, G1);
            if (omit_set.any()) {
                omit_set.set(max_freq_vert);
                return omit_set;
            }
        }

        // If we make it this far, we did not find a nonempty
        // omit_set, so the pair is dual
        return omit_set;
    }
}
