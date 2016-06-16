/**
   C++ implementation of the FK algorithms (library)
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
#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"

#include <cassert>
#include <cmath>
#include <vector>
#include <utility>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    Hypergraph::Edge FKAlgorithm::hitting_condition_check (const Hypergraph& F,
                                                           const Hypergraph& G) {
        // Check whether all edges in F and G intersect
        // per FK (eq. 1.1)
        for (auto const& Fedge: F) {
            for (auto const& Gedge: G) {
                if (not Fedge.intersects(Gedge)) {
                    Hypergraph::Edge omit_set = F.verts_covered() - Fedge;

                    BOOST_LOG_TRIVIAL(trace) << "Hitting condition failed!"
                                             << "\nF edge:\t" << Fedge
                                             << "\nG edge:\t" << Gedge;

                    return omit_set;
                }
            }
        }

        Hypergraph::Edge omit_set(F.num_verts());
        return omit_set;
    }

    Hypergraph::Edge FKAlgorithm::coverage_condition_check (const Hypergraph& F,
                                                            const Hypergraph& G) {
        // Check whether F and G cover the same vertices
        // per FK (eq. 1.2)
        Hypergraph::Edge Fcovered = F.verts_covered();
        Hypergraph::Edge Gcovered = G.verts_covered();

        Hypergraph::Edge Fsurplus = Fcovered - Gcovered;
        Hypergraph::Edge Gsurplus = Gcovered - Fcovered;

        if (Fsurplus.any()) {
            Hypergraph::EdgeIndex surplus_vertex = Fsurplus.find_first();
            for (auto const& edge: F) {
                if (edge.test(surplus_vertex)) {
                    BOOST_LOG_TRIVIAL(trace) << "Coverage condition 1 failed: G missing vertex " << surplus_vertex << "."
                                             << "\nF edge:\t\t" << edge;

                    Hypergraph::Edge omit_set = edge;
                    omit_set.reset(surplus_vertex);
                    return omit_set;
                }
            }
            throw std::runtime_error("Invalid state in coverage condition 1.");
        } else if (Gsurplus.any()) {
            Hypergraph::EdgeIndex surplus_vertex = Gsurplus.find_first();
            for (auto const& edge: G) {
                if (edge.test(surplus_vertex)) {
                    BOOST_LOG_TRIVIAL(trace) << "Coverage condition 2 failed: F missing vertex " << surplus_vertex << "."
                                             << "\nG edge:\t\t" << edge;

                    Hypergraph::Edge omit_set = (Fcovered | Gcovered)^ edge;
                    omit_set.set(surplus_vertex);
                    return omit_set;
                }
            }
            throw std::runtime_error("Invalid state in coverage condition 2.");
        }

        Hypergraph::Edge omit_set (F.num_verts());
        return omit_set;
    }

    Hypergraph::Edge FKAlgorithm::edge_size_check (const Hypergraph& F,
                                                   const Hypergraph& G) {
        // Check whether F and G satisfy the edge size condition
        // per FK (eq. 1.3)
        for (auto const& Fedge: F) {
            if (Fedge.count() > G.num_edges()) {
                // Found a too-large F edge. Now to construct the omit_set…
                BOOST_LOG_TRIVIAL(trace) << "Size condition 1 failed."
                                         << "\nF edge:\t\t" << Fedge;

                Hypergraph::EdgeIndex v_index = Fedge.find_first();
                while (v_index < Hypergraph::Edge::npos) {
                    Hypergraph::Edge omit_set = Fedge;
                    omit_set.reset(v_index);

                    bool omit_set_is_valid = true;

                    for (auto const& Gedge: G) {
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

        for (auto const& Gedge: G) {
            if (Gedge.count() > F.num_edges()) {
                // Found a too-large G edge. Now to construct the omit_set…
                // TODO: What is the omit_set?
                BOOST_LOG_TRIVIAL(trace) << "Size condition 2 failed."
                                         << "\nG edge:\t" << Gedge;
                Hypergraph::EdgeIndex v_index = Gedge.find_first();
                while (v_index < Hypergraph::Edge::npos) {
                    Hypergraph::Edge omit_set = Gedge;
                    omit_set.reset(v_index);

                    for (auto const& Fedge: F) {
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

        Hypergraph::Edge omit_set (F.num_verts());
        return omit_set;
    }

    Hypergraph::Edge FKAlgorithm::satisfiability_count_check (const Hypergraph& F,
                                                              const Hypergraph& G) {
        // Check whether the satisfiability count condition is met
        // per FK (eq. 2.1)
        // This is a subtle algebraic condition, but it is the heart of the FK
        // algorithms. If allows the authors to establish the recursion depth
        // guarantees that lead to the state-of-the-art complexity of these
        // algorithms.
        double checkvalue = 0;
        for (auto const& Fedge: F) {
            checkvalue += pow(2, -Fedge.count());
        }

        for (auto const& Gedge: G) {
            checkvalue += pow(2, -Gedge.count());
        }

        if (checkvalue < 1) {
            Hypergraph::Edge omit_set (F.num_verts());
            for (Hypergraph::EdgeIndex i = 0; i < F.num_verts(); ++i) {
                Hypergraph::Edge extended_omit_set = omit_set;
                extended_omit_set.set(i);

                unsigned long oldcount = 0;
                unsigned long newcount = 0;

                for (auto const& Fedge: F) {
                    if (omit_set.is_subset_of(Fedge)) {
                        ++oldcount;
                    }

                    if (extended_omit_set.is_subset_of(Fedge)) {
                        ++newcount;
                    }
                }

                for (auto const& Gedge: G) {
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

        Hypergraph::Edge omit_set (F.num_verts());
        return omit_set;
    }

    Hypergraph::Edge FKAlgorithm::small_hypergraphs_check (const Hypergraph& F,
                                                           const Hypergraph& G) {
        // Check whether F or G is small enough to handle manually.
        if (F.num_edges() == 0 or G.num_edges() == 0) {
            BOOST_LOG_TRIVIAL(trace) << "Either F or G is null.";

            // Any combination will do as a omit_set
            Hypergraph::Edge omit_set (F.num_verts());
            omit_set.set();
            return omit_set;
        }

        // If both have one edge, previous checks guarantee that they are transverse
        if (F.num_edges() == 1 and G.num_edges() == 1) {
            BOOST_LOG_TRIVIAL(trace) << "Either F or G is unital.";

            // Return the empty edge as a special signal
            Hypergraph::Edge omit_set (F.num_edges());
            return omit_set;
        }

        Hypergraph::Edge omit_set (F.num_verts());
        return omit_set;
    }

    Hypergraph::EdgeIndex FKAlgorithm::most_frequent_vertex (const Hypergraph& F,
                                                const Hypergraph& G) {
        size_t n = F.num_verts();
        std::vector<int> freqs(n);

        for (auto const& edge: F) {
            Hypergraph::EdgeIndex vertex = edge.find_first();
            while (vertex != Hypergraph::Edge::npos) {
                ++freqs[vertex];
                vertex = edge.find_next(vertex);
            }
        }

        for (auto const& edge: G) {
            Hypergraph::EdgeIndex vertex = edge.find_first();
            while (vertex != Hypergraph::Edge::npos) {
                ++freqs[vertex];
                vertex = edge.find_next(vertex);
            }
        }

        // Then we find the largest one
        auto&& max_freq_vert_iterator = std::max_element(freqs.begin(), freqs.end());
        Hypergraph::EdgeIndex max_freq_vert = std::distance(freqs.begin(), max_freq_vert_iterator);

        BOOST_LOG_TRIVIAL(trace) << "Most frequent vertex: " << max_freq_vert;

        return max_freq_vert;
    }

    Hypergraph FKAlgorithm::minimized_union (const Hypergraph& F,
                                             const Hypergraph& G) {
        // Construct the edge union of F and G, under the assumption that no edge
        // in G is a subset of any edge in F (valid for split-join operation)
        // TODO: extend this to work in generality using erase-remove

        Hypergraph result = F;
        result.reserve_edge_capacity(F.num_edges() + G.num_edges());

        for (auto& Gedge: G) {
            bool should_add = true;

            for (auto& Fedge: F) {
                if (Fedge.is_subset_of(Gedge)) {
                    should_add = false;
                }
            }

            if (should_add) {
                result.add_edge(Gedge);
            }
        }

        return result;
    }

    std::pair<Hypergraph, Hypergraph> FKAlgorithm::split_hypergraph_over_vertex (const Hypergraph& H,
                                                                                 Hypergraph::EdgeIndex v) {
        // Split H into two hypergraphs based on the vertex v:
        // H0 gets the edges which contained v, but with v removed from each
        // H1 gets the edges which did not contain v
        Hypergraph H0(H.num_verts()), H1(H.num_verts());
        //H0.reserve_edge_capacity(H.num_edges());
        //H1.reserve_edge_capacity(H.num_edges());

        for (auto const& edge: H) {
            if (edge.test(v)) {
                Hypergraph::Edge newedge = edge;
                newedge.reset(v);
                H0.add_edge(newedge);
            } else {
                H1.add_edge(edge);
            }
        }

        std::pair<Hypergraph, Hypergraph> result (H0, H1);
        return result;
    }

    Hypergraph::Edge FKAlgorithm::minimize_new_hs (const Hypergraph& F,
                                                   const Hypergraph& G,
                                                   Hypergraph::Edge S) {
        /**
           Given a hypergraph F, a collection of MHSs G, and new hitting set S,
           find a new MHS which is a subset of S.
        **/

        BOOST_LOG_TRIVIAL(trace) << "Attempting minimization of"
                                 << "\nS:\t" << S;

        // Input validation
        assert(F.is_transversed_by(S));

        // We have to be careful when we construct this minimum, because
        // we don't want to find something already in G.

        // Iterate through the vertices in S
        // NOTE: This is a sneaky hack! S.count() is recomputed each time.
        for (size_t i = 0; i < S.count(); ++i) {
            // Find the ith variable of S
            Hypergraph::EdgeIndex vertex = S.find_first();
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
