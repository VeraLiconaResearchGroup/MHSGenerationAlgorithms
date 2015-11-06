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

    typedef boost::dynamic_bitset<> bitset;
    typedef std::vector<bitset> bsvector;
    typedef bitset::size_type hindex;
    typedef moodycamel::ConcurrentQueue<bitset> bsqueue;

    class minimality_violated_exception: public std::exception {
        virtual const char* what() const throw() {
            return "A non-minimal edge was added.";
        }
    };

    class Hypergraph {
    public:

        Hypergraph(size_t num_verts = 0, size_t num_edges = 0);
        Hypergraph(const fs::path& input_file);
        Hypergraph(const bsvector& edges);

        size_t num_verts() const;
        size_t num_edges() const;

        void add_edge(const bitset& edge, bool test_simplicity = false);
        void reserve_edge_capacity(const size_t n_edges);
        Hypergraph edge_vee(const Hypergraph& G, const bool do_minimize = true) const;
        Hypergraph edge_wedge(const Hypergraph& G, const bool do_minimize = true) const;
        Hypergraph edge_wedge_cutoff(const Hypergraph& G, const size_t cutoff_size, const bool do_minimize = true) const;
        Hypergraph contraction(const bitset& S, const bool do_minimize = true) const;
        Hypergraph restriction(const bitset& S) const;
        bitset& operator[] (const hindex edge);
        const bitset& operator[] (const hindex edge) const;

        void write_to_file(const fs::path& output_file) const;
        Hypergraph minimization() const;
        Hypergraph transpose() const;
        bitset verts_covered() const;
        std::vector<hindex> vertex_degrees() const;
        bitset vertices_with_degree_above_threshold(const float degree_threshold) const;
        bitset edges_containing_vertex(const hindex& vertex) const;
        bool is_transversed_by(const bitset& S) const;
        bool has_edge_covered_by(const bitset& S) const;

        typedef std::vector<bitset>::const_iterator Hciterator;
        Hciterator begin() const {return _edges.cbegin();};
        Hciterator end() const {return _edges.cend();};

        friend std::ostream& operator<<(std::ostream& os, const Hypergraph& H);

    protected:
        size_t _n_verts;
        bsvector _edges;
    };
}

#endif
