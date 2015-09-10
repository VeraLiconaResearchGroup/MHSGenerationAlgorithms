/**
   C++ implementation of the RS algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _RS__H
#define _RS__H

#include <boost/dynamic_bitset.hpp>

#include "hypergraph.hpp"
#include "shd-base.hpp"

namespace agdmhs {
    Hypergraph rs_transversal(const Hypergraph& H, const size_t num_threads = 0, const size_t cutoff_size = 0);
}

#endif
