/**
   C++ implementation of Berge's algorithm
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

#ifndef _BERGE__H
#define _BERGE__H

#include "hypergraph.hpp"
#include "mhs-algorithm.hpp"

namespace agdmhs {
    class BergeAlgorithm: public MHSAlgorithm {
        unsigned cutoff_size;

    public:
        BergeAlgorithm (unsigned cutoff_size = 0);
        Hypergraph transversal (const Hypergraph& H) const override;

    private:
        Hypergraph update_transversals_with_edge (const Hypergraph& transversals, const Hypergraph::Edge& edge) const;
    };
}

#endif
