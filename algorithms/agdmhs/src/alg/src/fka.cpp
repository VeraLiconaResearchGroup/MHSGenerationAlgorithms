/**
   C++ implementation of the FK-A algorithm (library)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "fka.hpp"

#include "hypergraph.hpp"

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/dynamic_bitset.hpp>

#include <cassert>
#include <iostream>
#include <vector>

namespace agdmhs {
    Hypergraph fka_transversal(const Hypergraph& H) {
        BOOST_LOG_TRIVIAL(debug) << "Starting FKA. Hypergraph has "
                                 << H.num_verts() << " vertices and "
                                 << H.num_edges() << " edges.";
        Hypergraph G (H.num_verts());

        bool still_searching_for_transversals = true;
        while (still_searching_for_transversals) {
            BOOST_LOG_TRIVIAL(debug) << "Starting a run.";

            bitset omit_set = fka_find_omit_set(H, G);

            if (omit_set.none() and G.num_edges() > 0) {
                BOOST_LOG_TRIVIAL(debug) << "Received empty omit_set, so we're done.";
                still_searching_for_transversals = false;
            } else {
                bitset new_mhs = fka_transform_omit_set_to_mhs(H, G, omit_set);
                BOOST_LOG_TRIVIAL(debug) << "Received witness."
                                         << "\nomit_set:\t" << omit_set
                                         << "\nMHS:\t\t" << new_mhs;
                G.add_edge(new_mhs, true);
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
                                 << "and |G| = " << G.num_edges();

        // Input specification
        assert(F.num_verts() == G.num_verts());

        // FK step 0: minimize F and G
        Hypergraph Fmin = F.minimization();
        Hypergraph Gmin = G.minimization();

        BOOST_LOG_TRIVIAL(trace) << "Minimized, so now "
                                 << "|F| = " << Fmin.num_edges()
                                 << " and |G| = " << Gmin.num_edges();

        // FK step 1: initialize if G is empty
        if (Gmin.num_edges() == 0) {
            BOOST_LOG_TRIVIAL(trace) << "Returning empty omit_set since G is empty.";
            bitset omit_set(Fmin.num_verts());
            return omit_set;
        }

        // FK step 2: consistency checks
        // Check 1.1: hitting condition
        for (auto const& Fedge: Fmin) {
            for (auto const& Gedge: Gmin) {
                if (not Fedge.intersects(Gedge)) {
                    bitset omit_set = Fmin.verts_covered() - Fedge;

                    BOOST_LOG_TRIVIAL(trace) << "Hitting condition failed!\n"
                                             << "F edge:\t" << Fedge << "\n"
                                             << "G edge:\t" << Gedge;

                    return omit_set;
                }
            }
        }

        // Check 1.2: same vertices covered
        bitset Fcovered = Fmin.verts_covered();
        bitset Gcovered = Gmin.verts_covered();

        bitset Fsurplus = Fcovered - Gcovered;
        bitset Gsurplus = Gcovered - Fcovered;

        if (Fsurplus.any()) {
            hindex surplus_vertex = Fsurplus.find_first();
            for (auto const& edge: Fmin) {
                if (edge.test(surplus_vertex)) {
                    BOOST_LOG_TRIVIAL(trace) << "Coverage condition 1 failed.\n"
                                             << "F edge:\t\t" << edge;

                    bitset omit_set = edge;
                    omit_set.reset(surplus_vertex);
                    return omit_set;
                }
            }
            throw std::runtime_error("Invalid state in coverage condition 1.");
        } else if (Gsurplus.any()) {
            hindex surplus_vertex = Gsurplus.find_first();
            for (auto const& edge: Gmin) {
                if (edge.test(surplus_vertex)) {
                    BOOST_LOG_TRIVIAL(trace) << "Coverage condition 2 failed.\n"
                                             << "G edge:\t\t" << edge;

                    bitset omit_set = (Fcovered | Gcovered)^ edge;
                    omit_set.set(surplus_vertex);
                    return omit_set;
                }
            }
            throw std::runtime_error("Invalid state in coverage condition 2.");
        }

        // Check 1.3: neither F nor G has edges too large
        for (auto const& Fedge: Fmin) {
            if (Fedge.count() > G.num_edges()) {
                // Found a too-large F edge. Now to construct the omit_set…
                BOOST_LOG_TRIVIAL(trace) << "Size condition 1 failed.\n"
                                         << "F edge:\t\t" << Fedge;

                hindex v_index = Fedge.find_first();
                while (v_index < bitset::npos) {
                    bitset omit_set = Fedge;
                    omit_set.reset(v_index);

                    bool omit_set_is_valid = true;

                    for (auto const& Gedge: Gmin) {
                        if (not omit_set.intersects(Gedge)) {
                            omit_set_is_valid = false;
                        }
                    }

                    if (omit_set_is_valid) {
                        BOOST_LOG_TRIVIAL(trace) << "Omitted vertex " << v_index;
                        return omit_set;
                    }

                    v_index = Fedge.find_next(v_index);
                }
                throw std::runtime_error("Invalid state in size condition 1.");
            }
        }

        for (auto const& Gedge: Gmin) {
            if (Gedge.count() > F.num_edges()) {
                // Found a too-large G edge. Now to construct the omit_set…
                // TODO: What is the omit_set?
                BOOST_LOG_TRIVIAL(trace) << "Size condition 2 failed.\n"
                                         << "G edge:\t" << Gedge;
                hindex v_index = Gedge.find_first();
                while (v_index < bitset::npos) {
                    bitset omit_set = Gedge;
                    omit_set.reset(v_index);

                    for (auto const& Fedge: Fmin) {
                        if (not omit_set.intersects(Fedge)) {
                            // TODO: This omit_set calculation may be incorrect
                            return omit_set;
                        }
                    }

                    v_index = Gedge.find_next(v_index);
                }
                throw std::runtime_error("Invalid state in size condition 2.");
            }
        }

        // Check 2.1: satisfiability count condition
        double checkvalue = 0;
        for (auto const& Fedge: Fmin) {
            checkvalue += pow(2, -Fedge.count());
        }

        for (auto const& Gedge: Gmin) {
            checkvalue += pow(2, -Gedge.count());
        }

        if (checkvalue < 1) {
            bitset omit_set (Fmin.num_verts());
            for (hindex i = 0; i < Fmin.num_verts(); ++i) {
                bitset extended_omit_set = omit_set;
                extended_omit_set.set(i);

                unsigned long oldcount = 0;
                unsigned long newcount = 0;

                for (auto const& Fedge: Fmin) {
                    if (omit_set.is_subset_of(Fedge)) {
                        ++oldcount;
                    }

                    if (extended_omit_set.is_subset_of(Fedge)) {
                        ++newcount;
                    }
                }

                for (auto const& Gedge: Gmin) {
                    if (not Gedge.is_subset_of(omit_set)) {
                        ++oldcount;
                    }

                    if (not Gedge.is_subset_of(extended_omit_set)) {
                        ++newcount;
                    }
                }

                if (newcount <= oldcount) {
                    omit_set = extended_omit_set;
                }
            }

            BOOST_LOG_TRIVIAL(trace) << "Count condition failed.";

            return omit_set;
        }

        // FK step 3: Check whether F and G are small
        // If either hypergraph is empty, they cannot be dual
        if (Fmin.num_edges() == 0 or Gmin.num_edges() == 0) {
            BOOST_LOG_TRIVIAL(trace) << "Either F or G is null.";

            // Any combination will do as a omit_set
            bitset omit_set (Fmin.num_verts());
            omit_set.set();
            return omit_set;
        }

        // If both have one edge, previous checks guarantee that they are transverse
        if (Fmin.num_edges() == 1 and Gmin.num_edges() == 1) {
            BOOST_LOG_TRIVIAL(trace) << "Either F or G is unital.";

            // Return the empty edge as a special signal
            bitset omit_set (Fmin.num_edges());
            return omit_set;
        }

        // FK step 4: Recurse

        // Find the most frequently occurring vertex
        // First, we record the frequencies of the vertices
        std::vector<int> freqs(Fmin.num_verts());
        for (hindex i = 0; i < Fmin.num_verts(); ++i) {
            for (auto const& Fedge: Fmin) {
                if (Fedge.test(i)) {
                    ++freqs[i];
                }
            }

            for (auto const& Gedge: Gmin) {
                if (Gedge.test(i)) {
                    ++freqs[i];
                }
            }
        }

        // Then we find the largest one
        auto&& max_freq_vert_iterator = std::max_element(freqs.begin(), freqs.end());
        hindex max_freq_vert = std::distance(freqs.begin(), max_freq_vert_iterator);

        BOOST_LOG_TRIVIAL(trace) << "Most frequent vertex: " << max_freq_vert;

        // Then we compute the split hypergraphs F0, F1, G0, and G1

        Hypergraph F0(Fmin.num_verts()), F1(Fmin.num_verts()), G0(Fmin.num_verts()), G1(Fmin.num_verts());
        for (auto const& Fedge: Fmin) {
            if (Fedge.test(max_freq_vert)) {
                bitset newedge = Fedge;
                newedge.reset(max_freq_vert);
                F0.add_edge(newedge);
            } else {
                F1.add_edge(Fedge);
            }
        }

        for (auto const& Gedge: Gmin) {
            if (Gedge.test(max_freq_vert)) {
                bitset newedge = Gedge;
                newedge.reset(max_freq_vert);
                G0.add_edge(newedge);
            } else {
                G1.add_edge(Gedge);
            }
        }

        // We will also need the unions F0∪F1 and G0∪G1
        // These are not guaranteed minimal, so we minimize manually
        Hypergraph Fnew = F0.edge_union(F1).minimization();
        Hypergraph Gnew = G0.edge_union(G1).minimization();

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

        // If we make it this far, we did not find a omit_set, so the pair is dual
        bitset omit_set (Fmin.num_verts());
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
                    //break;
                }
            }

            if (is_hitting_set) {
                // If so, check whether any subset is already in G
                for (auto const& edge: G) {
                    if (edge.is_subset_of(S)) {
                        BOOST_LOG_TRIVIAL(trace) << "Result has a known subset!"
                                                 << "\nedge:\t" << edge;

                        has_no_known_subsets = false;
                        //break;
                    }
                }
            }

            // If the resulting set is a hitting set and does not have any
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
