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
    Hypergraph berge_transversal(const Hypergraph& H,
                                 const size_t cutoff_size = 0);
}

#endif
