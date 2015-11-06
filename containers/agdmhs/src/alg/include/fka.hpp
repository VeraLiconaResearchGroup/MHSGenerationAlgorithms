/**
   C++ implementation of the FK-A algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _FKA__H
#define _FKA__H

#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "hypergraph.hpp"

namespace agdmhs {
    Hypergraph fka_transversal(const Hypergraph& H);
    bitset fka_find_omit_set(const Hypergraph& F, const Hypergraph& G);
}

#endif
