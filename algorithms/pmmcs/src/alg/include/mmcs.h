/**
   C++ implementation of the MMCS algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _MMCS_MAIN__H
#define _MMCS_MAIN__H

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "hypergraph.h"

namespace agdmhs {
    Hypergraph mmcs_transversal(const Hypergraph& H, const size_t num_threads = 0, const size_t cutoff_size = 0);

    void extend_or_confirm_set(Hypergraph H,
                               bitset S,
                               bitset CAND,
                               Hypergraph crit,
                               bitset uncov,
                               size_t cutoff_size = 0);
}

#endif
