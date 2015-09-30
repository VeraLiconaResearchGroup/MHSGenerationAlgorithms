/**
   C++ implementation of the FK-A algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "fka.hpp"

#include "fk-base.hpp"
#include "hypergraph.hpp"

#include <cassert>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    Hypergraph fka_transversal(const Hypergraph& H) {
        BOOST_LOG_TRIVIAL(debug) << "Starting FKA. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";
        Hypergraph G (H.num_verts());

        Hypergraph Hmin = H.minimization();

        bool still_searching_for_transversals = true;
        while (still_searching_for_transversals) {
            bitset omit_set = fka_find_omit_set(Hmin, G);

            if (omit_set.none() and G.num_edges() > 0) {
                BOOST_LOG_TRIVIAL(debug) << "Received empty omit_set, so we're done.";
                still_searching_for_transversals = false;
            } else {
                bitset new_mhs = fka_transform_omit_set_to_mhs(H, G, omit_set);
                BOOST_LOG_TRIVIAL(trace) << "Received witness."
                                         << "\nomit_set:\t" << omit_set
                                         << "\nMHS:\t\t" << new_mhs;
                G.add_edge(new_mhs, true);
                BOOST_LOG_TRIVIAL(debug) << "New G size: " << G.num_edges();
            }
        }

        return G;
    }

    bitset fka_find_omit_set(const Hypergraph& F, const Hypergraph& G) {
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
        bitset omit_set (F.num_verts());

        // FK step 1: initialize if G is empty
        if (G.num_edges() == 0) {
            BOOST_LOG_TRIVIAL(trace) << "Returning empty omit_set since G is empty.";
            return omit_set;
        }

        // FK step 2: consistency checks
        // Check 1.1: hitting condition
        omit_set = fk_hitting_condition_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // Check 1.2: same vertices covered
        omit_set = fk_coverage_condition_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // Check 1.3: neither F nor G has edges too large
        omit_set = fk_edge_size_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // Check 2.1: satisfiability count condition
        omit_set = fk_satisfiability_count_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // FK step 3: Check whether F and G are small
        // If either hypergraph is empty, they cannot be dual
        omit_set = fk_small_hypergraphs_check(F, G);
        if (omit_set.any()) {
            return omit_set;
        }

        // FK step 4: Recurse

        // Find the most frequently occurring vertex
        hindex max_freq_vert = fk_most_frequent_vertex(F, G);

        // Then we compute the split hypergraphs F0, F1, G0, and G1
        std::pair<Hypergraph, Hypergraph> Fsplit, Gsplit;
        Hypergraph F0, F1, G0, G1;

        Fsplit = fk_split_hypergraph_over_vertex(F, max_freq_vert);
        F0 = Fsplit.first;
        F1 = Fsplit.second;

        Gsplit = fk_split_hypergraph_over_vertex(G, max_freq_vert);
        G0 = Gsplit.first;
        G1 = Gsplit.second;

        // We will also need the unions F0∪F1 and G0∪G1
        Hypergraph Fnew = fk_minimized_union(F0, F1);
        Hypergraph Gnew = fk_minimized_union(G0, G1);

        // And, finally, fire up the two recursions
        if (F1.num_edges() > 0 and Gnew.num_edges() > 0) {
            BOOST_LOG_TRIVIAL(trace) << "Side 1 recursion.";

            bitset omit_set = fka_find_omit_set(F1, Gnew);
            if (omit_set.any()) {
                return omit_set;
            }
        }

        if (Fnew.num_edges() > 0 and G1.num_edges() > 0) {
            BOOST_LOG_TRIVIAL(trace) << "Side 2 recursion.";

            bitset omit_set = fka_find_omit_set(Fnew, G1);
            if (omit_set.any()) {
                omit_set.set(max_freq_vert);
                return omit_set;
            }
        }

        // If we make it this far, we did not find a nonempty
        // omit_set, so the pair is dual
        return omit_set;
    }

    bitset fka_transform_omit_set_to_mhs(const Hypergraph& F, const Hypergraph& G, const bitset& O) {
        /**
           Given a hypergraph F, a collection of MHSs G, and a witness omit_set
           O, find a new inclusion-minimal HS to add to G.
        **/

        // Generate the (not necessarily minimal) HS corresponding to O
        bitset S = F.verts_covered() - O;

        BOOST_LOG_TRIVIAL(trace) << "Attempting minimization"
                                 << "\nfrom O:\t\t" << O
                                 << "\nobtained S:\t" << S;

        // Input validation
        assert(F.is_transversed_by(S));

        // We have to be careful when we construct this minimum, because
        // we don't want to find something already in G.

        // Iterate through the vertices in S
        // NOTE: This is a sneaky hack! S.count() is recomputed each time.
        for (size_t i = 0; i < S.count(); ++i) {
            // Find the ith variable of S
            hindex vertex = S.find_first();
            for (size_t j = 0; j < i; ++j) {
                vertex = S.find_next(vertex);
            }

            // Remove this vertex from S
            S.reset(vertex);

            BOOST_LOG_TRIVIAL(trace) << "Considering inclusion/exclusion of vertex " << vertex
                                     << "\nyields:\t" << S;

            // Record whether S is a hitting set and whether it
            // has any subsets already in G
            bool is_hitting_set = true;
            bool has_no_known_subsets = true;

            // Check whether the result is a hitting set
            for (auto& edge: F) {
                if (not edge.intersects(S)) {
                    BOOST_LOG_TRIVIAL(trace) << "Result is not a hitting set!"
                                             << "\nmissed:\t" << edge;

                    is_hitting_set = false;
                    break;
                }
            }

            if (is_hitting_set) {
                // If so, check whether any subset is already in G
                for (auto const& edge: G) {
                    if (edge.is_subset_of(S)) {
                        BOOST_LOG_TRIVIAL(trace) << "Result has a known subset!"
                                                 << "\nedge:\t" << edge;

                        has_no_known_subsets = false;
                        break;
                    }
                }
            }

            // If the resulting set is a hitting set an9d does not have any
            // subsets already in G, it is a viable candidate.
            // Thus, we backtrack.
            if (is_hitting_set and has_no_known_subsets) {
                BOOST_LOG_TRIVIAL(trace) << "Vertex is removable.";
                --i;
            } else {
                // Otherwise, we put this vertex back and move on.
                BOOST_LOG_TRIVIAL(trace) << "Vertex is not removable.";
                S.set(vertex);
            }
        }

        // What's left in S is an inclusion-minimal hitting set
        assert(F.is_transversed_by(S));
        return S;
    }
}
