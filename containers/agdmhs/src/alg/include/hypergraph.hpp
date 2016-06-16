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

#ifndef _HYPERGRAPH__H
#define _HYPERGRAPH__H

#include "concurrentqueue.h"

#include <boost/filesystem.hpp>
#include <boost/dynamic_bitset.hpp>

#include <exception>
#include <iostream>
#include <vector>

namespace agdmhs {
    namespace fs = boost::filesystem;

    class minimality_violated_exception: public std::exception {
        virtual const char* what() const throw() {
            return "A non-minimal edge was added.";
        }
    };

    class Hypergraph {
    public:
        using Edge = boost::dynamic_bitset<>;
        using EdgeVector = std::vector<Edge>;
        using EdgeQueue = moodycamel::ConcurrentQueue<Edge>;
        using EdgeIndex = Edge::size_type;

        Hypergraph(unsigned num_verts = 0, unsigned num_edges = 0);
        Hypergraph(const fs::path& input_file);
        Hypergraph(const EdgeVector& edges);

        unsigned num_verts() const;
        unsigned num_edges() const;

        void add_edge(const Edge& edge, bool test_simplicity = false);
        void reserve_edge_capacity(unsigned n_edges);
        Hypergraph edge_vee(const Hypergraph& G, bool do_minimize = true) const;
        Hypergraph edge_wedge(const Hypergraph& G, bool do_minimize = true) const;
        Hypergraph edge_wedge_cutoff(const Hypergraph& G, unsigned cutoff_size, bool do_minimize = true) const;
        Hypergraph contraction(const Edge& S, bool do_minimize = true) const;
        Hypergraph restriction(const Edge& S) const;
        Edge& operator[] (EdgeIndex edge_index);
        const Edge& operator[] (EdgeIndex edge_index) const;

        void write_to_file(const fs::path& output_file) const;
        Hypergraph minimization() const;
        Hypergraph transpose() const;
        Edge verts_covered() const;
        std::vector<unsigned> vertex_degrees() const;
        Edge vertices_with_degree_above_threshold(const float degree_threshold) const;
        Edge edges_containing_vertex(EdgeIndex vertex_index) const;
        bool is_transversed_by(const Edge& S) const;
        bool has_edge_covered_by(const Edge& S) const;

        EdgeVector::const_iterator begin() const {return _edges.cbegin();};
        EdgeVector::const_iterator end() const {return _edges.cend();};

        friend std::ostream& operator<<(std::ostream& os, const Hypergraph& H);

    protected:
        unsigned _n_verts;
        EdgeVector _edges;
    };
}

#endif
