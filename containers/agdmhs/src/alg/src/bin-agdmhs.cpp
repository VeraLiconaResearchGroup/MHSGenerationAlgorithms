/**
   C++ implementation of various MHS algorithms (binary)
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

#include "berge.hpp"
#include "bm.hpp"
#include "fka.hpp"
#include "hypergraph.hpp"
#include "mmcs.hpp"
#include "mhs-algorithm.hpp"
#include "rs.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace po = boost::program_options;

int main (int argc, char * argv[]) {
    // SET UP ARGUMENTS
    po::options_description desc("Options");
    desc.add_options()
        ("input", po::value<std::string>()->required(), "Input hypergraph file")
        ("output", po::value<std::string>()->default_value("out.dat"), "Output transversals file")
        ("verbosity,v", po::value<int>()->default_value(0)->implicit_value(1), "Write verbose debugging output (-v2 for trace output)")
        ("algorithm,a", po::value<std::string>()->default_value("pmmcs"), "Algorithm to use (pmmcs, prs, fka, berge, bm)")
        ("num-threads,t", po::value<int>()->default_value(1), "Number of threads to run in parallel. Supported by pmmcs, prs, and bm; ignored by other algorithms.")
        ("cutoff-size,c", po::value<int>()->default_value(0), "Maximum size set to return (0: no limit). Supported by pmmcs, prs, and berge; ignored by other algorithms.")
        ("count-only", po::bool_switch(), "If set, count MHSes but do not store them. Useful in cases where complete MHS set will not fit in memory. Supported by pmmcs and prs; ignored by other algorithms.");

    po::positional_options_description p;
    p.add("input", 1);
    p.add("output", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    if (vm.count("help") or argc == 1) {
        std::cout << desc << std::endl;
        return 1;
    };

    const size_t num_threads = (vm["num-threads"].as<int>());
    const size_t cutoff_size = (vm["cutoff-size"].as<int>());

    const bool count_only = vm["count-only"].as<bool>();

    po::notify(vm);

    // Process input file
    BOOST_LOG_TRIVIAL(debug) << "Loading hypergraph from file.";
    std::string input_file(vm["input"].as<std::string>());
    agdmhs::Hypergraph H (input_file);
    BOOST_LOG_TRIVIAL(debug) << "Loading complete.";

    // Process logging-related options
    int verbosity = vm["verbosity"].as<int>();
    switch (verbosity) {
    case 1:
        boost::log::core::get()->set_filter
            (boost::log::trivial::severity >= boost::log::trivial::debug);
        break;

    case 2:
        boost::log::core::get()->set_filter
            (boost::log::trivial::severity >= boost::log::trivial::trace);
        break;

    default:
        boost::log::core::get()->set_filter
            (boost::log::trivial::severity >= boost::log::trivial::warning);
        break;
    }

    // Print input information
    std::cout << "Input has " << H.num_verts() << " vertices and " << H.num_edges() << " edges." << std::endl;

    // Run chosen algorithm
    std::string algname = vm["algorithm"].as<std::string>();

    std::unique_ptr<agdmhs::MHSAlgorithm> mhs_algorithm;
    if (algname == "berge") {
        mhs_algorithm = std::unique_ptr<agdmhs::MHSAlgorithm> (new agdmhs::BergeAlgorithm(cutoff_size));
    } else if (algname == "bm") {
        mhs_algorithm = std::unique_ptr<agdmhs::MHSAlgorithm> (new agdmhs::ParBMAlgorithm (num_threads));
    } else if (algname == "fka") {
        mhs_algorithm = std::unique_ptr<agdmhs::MHSAlgorithm> (new agdmhs::FKAlgorithmA());
    } else if (algname == "mmcs" or algname == "pmmcs") {
        mhs_algorithm = std::unique_ptr<agdmhs::MHSAlgorithm> (new agdmhs::MMCSAlgorithm(num_threads, cutoff_size, count_only));
    } else if (algname == "rs" or algname == "prs") {
        mhs_algorithm = std::unique_ptr<agdmhs::MHSAlgorithm> (new agdmhs::RSAlgorithm(num_threads, cutoff_size, count_only));
    } else {
        std::stringstream error_message;
        error_message << "Did not recognize requested algorithm " << algname << ".";
        throw po::invalid_option_value(error_message.str());
    }

    BOOST_LOG_TRIVIAL(debug) << "Running algorithm " << algname;
    agdmhs::Hypergraph Htrans = mhs_algorithm->transversal(H);

    if (not count_only) {
        std::cout << "Found " << Htrans.num_edges() << " hitting sets." << std::endl;
        BOOST_LOG_TRIVIAL(debug) << "Algorithm complete.";

        // Print results
        BOOST_LOG_TRIVIAL(debug) << "Writing result file.";
        std::string output_file(vm["output"].as<std::string>());
        Htrans.write_to_file(output_file);
        BOOST_LOG_TRIVIAL(debug) << "Writing complete.";
    }

    return 0;
}
