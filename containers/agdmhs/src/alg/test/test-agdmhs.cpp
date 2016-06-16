/**
 Test cases for agdmhs package

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

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

int main (int argc, char* const argv[]) {
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
    int result = Catch::Session().run(argc, argv);
    return result;
}

// No tests defined in this file
