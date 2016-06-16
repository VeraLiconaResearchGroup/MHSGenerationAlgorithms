/**
   C++ implementation of the SHD algorithms
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

#ifndef _SHD__H
#define _SHD__H

#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"

#include <map>

namespace agdmhs {
    class vertex_violating_exception: public std::exception {
        virtual const char* what() const throw() {
            return "The vertex was violating for this candidate hitting set.";
        }
    };

    class SHDAlgorithm: public MHSAlgorithm {
    protected:
        using hsetmap = std::map<Hypergraph::EdgeIndex, Hypergraph::Edge>;

        bool vertex_would_violate (const Hypergraph& crit, const Hypergraph::Edge& uncov, const Hypergraph& H, const Hypergraph& T, const Hypergraph::Edge& S, Hypergraph::EdgeIndex v) const;
        hsetmap update_crit_and_uncov(Hypergraph& crit, Hypergraph::Edge& uncov, const Hypergraph& H, const Hypergraph& T, const Hypergraph::Edge& S, Hypergraph::EdgeIndex v) const ;
        void restore_crit_and_uncov(Hypergraph& crit, Hypergraph::Edge& uncov, const Hypergraph::Edge& S, const hsetmap& critmark, Hypergraph::EdgeIndex v) const;
    };
}

#endif
