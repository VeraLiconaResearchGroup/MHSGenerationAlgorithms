/**
   C++ implementation of Berge's algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _BERGE__H
#define _BERGE__H

#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>

namespace agdmhs {
    Hypergraph berge_transversal(const Hypergraph& H);
    void berge_update_transversals_with_edge(Hypergraph& G, const bitset& edge);
}

#endif
