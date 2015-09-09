/**
   C++ implementation of the SHD algorithms
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _SHD__H
#define _SHD__H

#include <boost/dynamic_bitset.hpp>

#include "concurrentqueue.h"
#include "hypergraph.hpp"

namespace agdmhs {
    typedef moodycamel::ConcurrentQueue<bitset> bsqueue;

    void update_crit_and_uncov(Hypergraph& crit,
                               bitset& uncov,
                               const Hypergraph& H,
                               const hindex v);
}

#endif
