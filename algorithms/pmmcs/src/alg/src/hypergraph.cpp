/**
   C++ implementation of a hypergraph as an array of bitsets
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>
#include <boost/filesystem/fstream.hpp>
#include <vector>
#include <stdexcept>
#include <sstream>

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
            for (auto& v: edges[i]) {
                _edges[i][v] = true;
            }
        }

        // All done!
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

    void Hypergraph::add_edge(const bitset& edge) {
        _edges.push_back(edge);
    };

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

        for (auto& edge: _edges) {
            hindex i = edge.find_first();
            while (i != bitset::npos) {
                output_filestream << i << " ";
                i = edge.find_next(i);
            }
            output_filestream << std::endl;
        }
    };
}
