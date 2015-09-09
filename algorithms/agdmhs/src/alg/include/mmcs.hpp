/**
   C++ implementation of the MMCS algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _MMCS__H
#define _MMCS__H

#include <boost/dynamic_bitset.hpp>

#include "hypergraph.hpp"
#include "shd-base.hpp"

namespace agdmhs {
    Hypergraph mmcs_transversal(const Hypergraph& H, const size_t num_threads = 0, const size_t cutoff_size = 0);

    void mmcs_extend_or_confirm_set(bsqueue& HittingSets,
                                    const Hypergraph& H,
                                    const bitset S,
                                    const bitset CAND,
                                    const Hypergraph crit,
                                    const bitset uncov,
                                    const size_t cutoff_size = 0);
}

#endif
