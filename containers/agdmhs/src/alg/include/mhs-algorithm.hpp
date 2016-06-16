/**
   Abstract base class for MHS algorithms
   Copyright Vera-Licona Research Group (C) 2016
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

#ifndef __MHS_ALGORITHM_H
#define __MHS_ALGORITHM_H

#include "hypergraph.hpp"

namespace agdmhs {
    class MHSAlgorithm {
    public:
        virtual ~MHSAlgorithm () {};
        virtual Hypergraph transversal (const Hypergraph& H) const = 0;
    };
}

#endif
