/**
   C++ implementation of the SHD algorithms
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _SHD__H
#define _SHD__H

#include "concurrentqueue.h"
#include "hypergraph.hpp"

#include <boost/dynamic_bitset.hpp>

#include <exception>

namespace agdmhs {
    typedef moodycamel::ConcurrentQueue<bitset> bsqueue;

    class vertex_violating_exception: public std::exception {
        virtual const char* what() const throw() {
            return "The vertex was violating for this candidate hitting set.";
        }
    };

    void update_crit_and_uncov(Hypergraph& crit,
                               bitset& uncov,
                               const Hypergraph& H,
                               const Hypergraph& T,
                               const bitset& S,
                               const hindex v);

    void restore_crit_and_uncov(Hypergraph& crit,
                                bitset& uncov,
                                const Hypergraph& H,
                                const Hypergraph& T,
                                const bitset& S,
                                const hindex v);
}

#endif
