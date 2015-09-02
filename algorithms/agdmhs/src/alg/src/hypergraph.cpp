/**
   C++ implementation of a hypergraph as an array of bitsets
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem/fstream.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace agdmhs {
    Hypergraph::Hypergraph (size_t num_verts, size_t num_edges):
        _n_verts(num_verts)
    {
        _edges = bsvector(num_edges, bitset(num_verts));
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
        std::vector<std::vector<hindex>> edges;
        hindex max_vertex = 0;
        hindex n_edges = 0;

        // Read the file line by line
        for (std::string line; std::getline(hypergraph_filestream, line); ) {
            // Each line is an edge
            std::istringstream linestream(line);
            ++n_edges;
            std::vector<hindex> edge;
            for (hindex v; linestream >> v; ) {
                // Each word of the line is a vertex
                edge.push_back(v);
                max_vertex = std::max(max_vertex, v);
            }
            edges.push_back(edge);
        }

        // Set up the hypergraph as a vector of bitsets
        _n_verts = max_vertex + 1;
        _edges = bsvector(edges.size(), bitset(_n_verts));

        for (hindex i = 0; i < n_edges; ++i) {
            for (auto const& v: edges[i]) {
                _edges[i][v] = true;
            }
        }
    };

    Hypergraph::Hypergraph (const bsvector& edges):
        _edges(edges)
    {
        if (edges.size() > 0) {
            _n_verts = edges.at(0).size();
        } else {
            _n_verts = 0;
        }
    };

    size_t Hypergraph::num_verts() const {
        /**
           Return the number of vertices.

           NOTE: The underlying vector is 0-indexed, so this is one more
           than the maximum index!
         **/
        return _n_verts;
    };

    size_t Hypergraph::num_edges() const {
        /**
           Return the number of edges.

           NOTE: The underlying vector is 0-indexed, so this is one more
           than the maximum index!
        **/
        return _edges.size();
    };

    void Hypergraph::add_edge(const bitset& edge, bool test_simplicity) {
        if (test_simplicity) {
            for (auto const& existing_edge: _edges) {
                if (existing_edge.is_subset_of(edge)) {
                    std::cout << "existing:\t" << existing_edge << "\n⊂ added:\t" << edge << std::endl;
                    throw std::runtime_error("Invalid edge addition.");
                }

                if (edge.is_subset_of(existing_edge)) {
                    std::cout << "added:\t\t" << edge << "\n⊂ existing:\t" << existing_edge << std::endl;
                    throw std::runtime_error("Invalid edge addition.");
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

    Hypergraph Hypergraph::edge_union(const Hypergraph& G) const {
        // Return new hypergraph with the edges of this and G
        assert(_n_verts == G.num_verts());

        bsvector newedges;

        // A bit of vector magic to save some time
        newedges.reserve(_edges.size() + G.num_edges());
        newedges.insert(newedges.end(), _edges.begin(), _edges.end());
        newedges.insert(newedges.end(), G._edges.begin(), G._edges.end());

        Hypergraph result(newedges);
        return result;
    }

    bitset& Hypergraph::operator[] (const hindex edge) {
        return _edges.at(edge);
    };

    void Hypergraph::write_to_file(const fs::path& output_file) const {
        // Set up file writer
        fs::ofstream output_filestream(output_file);
        if (!output_filestream.good()) {
            std::stringstream errorMessage;
            errorMessage << "Could not open output file " << output_file << " for writing.";
            throw std::runtime_error(errorMessage.str());
        }

        for (auto const& edge: _edges) {
            hindex i = edge.find_first();
            while (i != bitset::npos) {
                output_filestream << i << " ";
                i = edge.find_next(i);
            }
            output_filestream << std::endl;
        }
    };

    Hypergraph Hypergraph::minimization() const {
        /**
           Return a new hypergraph containing only the inclusion-minimal
           elements of this one.
         **/

        if (_edges.size() == 0) {
            return *this;
        }

        // TODO: This algorithm is O(n²)!
        // Can it be made better with sort-and-scan?
        bsvector sorted_edges = _edges;
        std::sort(sorted_edges.begin(), sorted_edges.end());

        bsvector new_edges;

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

    bitset Hypergraph::verts_covered() const {
        // Find the vertices covered by edges of this hypergraph
        bitset result (_n_verts);
        for (auto const& edge: _edges) {
            result |= edge;
        }

        return result;
    };

    bool Hypergraph::is_transversed_by(const bitset& S) const {
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

    bool Hypergraph::has_edge_covered_by(const bitset& S) const {
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
}
