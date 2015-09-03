/**
   C++ implementation of a hypergraph as an array of bitsets
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _HYPERGRAPH__H
#define _HYPERGRAPH__H

#include <boost/filesystem.hpp>
#include <boost/dynamic_bitset.hpp>
#include <vector>

namespace agdmhs {
    namespace fs = boost::filesystem;

    typedef boost::dynamic_bitset<> bitset;
    typedef std::vector<bitset> bsvector;
    typedef bitset::size_type hindex;

    class Hypergraph {
    public:

        Hypergraph(size_t num_verts = 0, size_t num_edges = 0);
        Hypergraph(const fs::path& input_file);
        Hypergraph(const bsvector& edges);

        size_t num_verts() const;
        size_t num_edges() const;

        void add_edge(const bitset& edge, bool test_simplicity = false);
        void reserve_edge_capacity(size_t n_edges);
        Hypergraph edge_union(const Hypergraph& G) const;
        bitset& operator[] (const hindex edge);

        void write_to_file(const fs::path& output_file) const;
        Hypergraph minimization() const;
        bitset verts_covered() const;
        bool is_transversed_by(const bitset& S) const;
        bool has_edge_covered_by(const bitset& S) const;

        typedef std::vector<bitset>::const_iterator Hciterator;
        Hciterator begin() const {return _edges.cbegin();};
        Hciterator end() const {return _edges.cend();};

    protected:
        size_t _n_verts;
        bsvector _edges;
    };
}

#endif
