/**
   C++ implementation of a hypergraph as an array of bitsets
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

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem/fstream.hpp>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace agdmhs {
    Hypergraph::Hypergraph (unsigned num_verts,
                            unsigned num_edges):
        _n_verts(num_verts)
    {
        _edges = Hypergraph::EdgeVector(num_edges, Edge(num_verts));
    };

    Hypergraph::Hypergraph (const fs::path& input_file)
    {
        // Set up file reader
        fs::ifstream hypergraph_filestream(input_file);
        if (!hypergraph_filestream.good()) {
            std::stringstream errorMessage;
            errorMessage << "Could not open hypergraph file " << input_file << " for reading.";
            throw std::runtime_error(errorMessage.str());
        }

        // Set up intermediate variables
        std::vector<std::vector<Hypergraph::EdgeIndex>> edges_by_indices;
        Hypergraph::EdgeIndex max_vertex = 0;
        unsigned n_edges = 0;

        // Read the file line by line
        for (std::string line; std::getline(hypergraph_filestream, line); ) {
            // Each line is an edge
            std::istringstream linestream(line);
            ++n_edges;
            std::vector<Hypergraph::EdgeIndex> edge_indices;
            for (Hypergraph::EdgeIndex v; linestream >> v; ) {
                // Each word of the line is a vertex
                edge_indices.push_back(v);
                max_vertex = std::max(max_vertex, v);
            }
            edges_by_indices.push_back(edge_indices);
        }

        // Set up the hypergraph as a vector of edges
        _n_verts = max_vertex + 1;
        _edges = Hypergraph::EdgeVector(edges_by_indices.size(), Hypergraph::Edge(_n_verts));

        for (Hypergraph::EdgeIndex i = 0; i < n_edges; ++i) {
            for (auto const& v: edges_by_indices[i]) {
                _edges[i][v] = true;
            }
        }
    };

    Hypergraph::Hypergraph (const Hypergraph::EdgeVector& edges):
        _edges(edges)
    {
        if (edges.size() > 0) {
            _n_verts = edges.at(0).size();
        } else {
            _n_verts = 0;
        }
    };

    unsigned Hypergraph::num_verts () const {
        /**
           Return the number of vertices.

           NOTE: The underlying vector is 0-indexed, so this is one more
           than the maximum index!
        **/
        return _n_verts;
    };

    unsigned Hypergraph::num_edges () const {
        /**
           Return the number of edges.

           NOTE: The underlying vector is 0-indexed, so this is one more
           than the maximum index!
        **/
        return _edges.size();
    };

    void Hypergraph::add_edge (const Hypergraph::Edge& edge,
                               bool test_simplicity) {
        if (test_simplicity) {
            for (auto const& existing_edge: _edges) {
                if (existing_edge.is_subset_of(edge)) {
                    throw minimality_violated_exception();
                }

                if (edge.is_subset_of(existing_edge)) {
                    throw minimality_violated_exception();
                }
            }
        }

        _edges.push_back(edge);

        if (_n_verts == 0) {
            _n_verts = edge.size();
        } else if (_n_verts != edge.size()) {
            throw std::runtime_error("Attempted to add edge of invalid size!");
        }
    };

    void Hypergraph::reserve_edge_capacity (unsigned n_edges) {
        _edges.reserve(n_edges);
    };

    Hypergraph Hypergraph::edge_vee (const Hypergraph& G,
                                     bool do_minimize) const {
        // Return new hypergraph with the edges of this and G
        // Note: we assume that this and G share the same vertex set
        assert(num_verts() == G.num_verts());

        // Container to store all the new edges
        Hypergraph::EdgeVector newedges;

        // Fill newedges with the edges from this and G
        // We use a bit of vector magic to save some time
        newedges.reserve(num_edges() + G.num_edges());
        newedges.insert(newedges.end(), _edges.begin(), _edges.end());
        newedges.insert(newedges.end(), G._edges.begin(), G._edges.end());

        // Build the result hypergraph with these edges
        Hypergraph result(newedges);

        // Minimize if requested
        if (do_minimize) {
            result = result.minimization();
        }

        return result;
    }

    Hypergraph Hypergraph::transpose () const {
        // Return new hypergraph T such that T[i][j] = this[j][i]
        Hypergraph T (num_edges());
        T.reserve_edge_capacity(_n_verts);

        for (Hypergraph::EdgeIndex v = 0; v < _n_verts; ++v) {
            T.add_edge(edges_containing_vertex(v));
        }

        return T;
    }

    Hypergraph Hypergraph::edge_wedge (const Hypergraph& G,
                                       bool do_minimize) const {
        // Return new hypergraph with edges all possible unions of
        // edges from this and G
        // Note: we assume that this and G share the same vertex set
        assert(num_verts() == G.num_verts());

        // Container to store all the new edges
        Hypergraph::EdgeVector newedges (num_edges() * G.num_edges(), Edge(_n_verts));

        // For every pair of edges in this and G, add their union
        for (auto& edge1: _edges) {
            for (auto& edge2: G._edges) {
                newedges.push_back(edge1 | edge2);
            }
        }

        // Build the result hypergraph with these edges
        Hypergraph result(newedges);

        // Minimize if requested
        // TODO: Can this be rolled into the union operation to save time?
        if (do_minimize) {
            result = result.minimization();
        }

        return result;
    }

    Hypergraph Hypergraph::edge_wedge_cutoff (const Hypergraph& G,
                                              unsigned cutoff_size,
                                              bool do_minimize) const {
        // Return new hypergraph with edges all possible unions of edges from
        // this and G whose size is no greater than cutoff_size
        // Note: we assume that this and G share the same vertex set
        assert(num_verts() == G.num_verts());

        // Container to store all the new edges
        Hypergraph result (G.num_verts());

        // For every pair of edges in this and G, add their union
        for (auto& edge1: _edges) {
            for (auto& edge2: G._edges) {
                Hypergraph::Edge newedge = edge1 | edge2;
                if (newedge.count() <= cutoff_size) {
                    result.add_edge(newedge);
                }
            }
        }

        // Minimize if requested
        // TODO: Can this be rolled into the union operation to save time?
        if (do_minimize) {
            result = result.minimization();
        }

        return result;
    }

    Hypergraph Hypergraph::contraction (const Hypergraph::Edge& S,
                                        bool do_minimize) const {
        /**
           Return the contraction of H onto S.
           This is the hypergraph whose edges are e∩S.
           This is not automatically minimal, but optionally we can minimize
           the result before returning it.
        **/
        Hypergraph result (num_verts());
        for (auto& edge: _edges) {
            Hypergraph::Edge new_edge = edge & S;
            result.add_edge(new_edge);
        }

        if (do_minimize) {
            result = result.minimization();
        }

        return result;
    }

    Hypergraph Hypergraph::restriction (const Hypergraph::Edge& S) const {
        /**
           Return the restriction of H to S.
           This is the hypergraph whose edges are the edges e of H which are
           subsets of H.
           If H is minimal, so is its restriction.
        **/
        Hypergraph result (num_verts());
        for (auto& edge: _edges) {
            if (edge.is_subset_of(S)) {
                result.add_edge(edge);
            }
        }

        return result;
    }

    Hypergraph::Edge& Hypergraph::operator[] (Hypergraph::EdgeIndex edge_index) {
        return _edges.at(edge_index);
    };

    const Hypergraph::Edge& Hypergraph::operator[] (Hypergraph::EdgeIndex edge_index) const {
        return _edges.at(edge_index);
    }

    void Hypergraph::write_to_file (const fs::path& output_file) const {
        // Set up file writer
        fs::ofstream output_filestream(output_file);
        if (!output_filestream.good()) {
            std::stringstream errorMessage;
            errorMessage << "Could not open output file " << output_file << " for writing.";
            throw std::runtime_error(errorMessage.str());
        }

        for (auto const& edge: _edges) {
            Hypergraph::EdgeIndex v = edge.find_first();
            while (v != Edge::npos) {
                output_filestream << v << ' ';
                v = edge.find_next(v);
            }
            output_filestream << "\n";
        }
    };

    Hypergraph Hypergraph::minimization () const {
        /**
           Return a new hypergraph containing only the inclusion-minimal
           elements of this one.
        **/

        if (_edges.empty()) {
            return *this;
        }

        // TODO: This algorithm is O(n²)!
        // Can it be made better with sort-and-scan?
        Hypergraph::EdgeVector sorted_edges = _edges;
        std::sort(sorted_edges.begin(), sorted_edges.end());

        Hypergraph::EdgeVector new_edges;

        for (auto const& new_edge: sorted_edges) {
            if (new_edge.none()) {
                continue;
            }

            bool edge_is_minimal = true;
            for (auto const& confirmed_edge: new_edges) {
                if (confirmed_edge.is_subset_of(new_edge)) {
                    edge_is_minimal = false;
                    break;
                }
            }

            if (edge_is_minimal) {
                new_edges.push_back(new_edge);
            }
        }

        if (new_edges.empty()) {
            Hypergraph Hmin(_n_verts);
            return Hmin;
        } else {
            Hypergraph Hmin(new_edges);
            return Hmin;
        }
    };

    Hypergraph::Edge Hypergraph::verts_covered () const {
        // Find the vertices covered by edges of this hypergraph
        Hypergraph::Edge result (_n_verts);
        for (auto const& edge: _edges) {
            result |= edge;
        }

        return result;
    };

    std::vector<unsigned> Hypergraph::vertex_degrees () const {
        std::vector<unsigned> result (num_verts());
        for (auto& edge: _edges) {
            // TODO: If this is a bottleneck, we could speed things
            // up by working on blocks instead of individual values
            for (Hypergraph::EdgeIndex i = 0; i < edge.size(); ++i) {
                result[i] += edge[i];
            }
        }

        return result;
    }

    Hypergraph::Edge Hypergraph::vertices_with_degree_above_threshold (const float degree_threshold) const {
        /**
           Return the set of vertices whose degree is greater than
           degree_threshold*|H|.
        **/
        assert(0 <= degree_threshold);
        assert(degree_threshold <= 1);
        auto&& degrees = vertex_degrees();

        unsigned n = num_verts();
        unsigned m = num_edges();
        unsigned count_threshold = floor(m * degree_threshold);

        Edge result (n);

        for (Hypergraph::EdgeIndex i = 0; i < n; ++i) {
            if (degrees[i] > count_threshold) {
                result.set(i);
            }
        }

        return result;
    }

    Hypergraph::Edge Hypergraph::edges_containing_vertex (Hypergraph::EdgeIndex vertex_index) const {
        unsigned n = num_edges();
        Hypergraph::Edge result (n);
        for (Hypergraph::EdgeIndex edge_index = 0; edge_index < n; ++edge_index) {
            if (_edges[edge_index].test(vertex_index)) {
                result.set(edge_index);
            }
        }
        return result;
    };

    bool Hypergraph::is_transversed_by (const Hypergraph::Edge& S) const {
        // Return true if S is a hitting set for this hypergraph
        // NOTE: equivalent to evaluating as a CNF with S as an assignment
        assert(S.size() == _n_verts);

        for (auto const& edge: _edges) {
            if (not S.intersects(edge)) {
                return false;
            }
        }

        return true;
    };

    bool Hypergraph::has_edge_covered_by (const Hypergraph::Edge& S) const {
        // Return true if some edge of this hypergraph is covered by S
        // NOTE: equivalent to evaluating as a DNF with S as an assignment
        assert(S.size() == _n_verts);

        for (auto const& edge: _edges) {
            if (edge.is_subset_of(S)) {
                return true;
            }
        }

        return false;
    }

    std::ostream& operator<< (std::ostream& os, const Hypergraph& H) {
        os << "Hypergraph with " << H.num_verts() << " vertices and "
           << H.num_edges() << " edges: \n";
        for (auto& e: H) {
            os << e << "\n";
        }

        return os;
    }
}
