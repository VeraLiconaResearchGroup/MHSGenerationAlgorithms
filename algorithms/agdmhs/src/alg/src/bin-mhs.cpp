/**
   C++ implementation of various MHS algorithms (binary)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "berge.hpp"
#include "fka.hpp"
#include "hypergraph.hpp"
#include "mmcs.hpp"

#include <iostream>
#include <sstream>
#include <string>

#define BOOST_LOG_DYN_LINK 1 // Fix an issue with dynamic library loading
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

void check_threads(int num_threads) {
    if (num_threads > 1) {
        std::cout << "Notice: this algorithm does not support multithreading." << std::endl;
    }
}

void check_cutoff(int cutoff_size) {
    if (cutoff_size > 0) {
        std::cout << "Notice: this algorithm does not support cutoff." << std::endl;
    }
}

int main(int argc, char * argv[]) {
    // SET UP ARGUMENTS
    po::options_description desc("Options");
    desc.add_options()
        ("input", po::value<std::string>()->required(), "Input hypergraph file")
        ("output", po::value<std::string>()->default_value("out.dat"), "Output transversals file")
        ("verbosity,v", po::value<int>()->default_value(0)->implicit_value(1), "Write verbose debugging output (-v2 for trace output)")
        ("algorithm,a", po::value<std::string>()->default_value("pmmcs"), "Algorithm to use (pmmcs, fka, berge)")
        ("num-threads,t", po::value<int>()->default_value(1), "Number of threads to run in parallel")
        ("cutoff-size,c", po::value<int>()->default_value(0), "Maximum size set to return (0: no limit)");

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

    po::notify(vm);

    // Process input file
    std::string input_file(vm["input"].as<std::string>());
    agdmhs::Hypergraph H (input_file);

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

    // Run chosen algorithm
    agdmhs::Hypergraph Htrans;
    std::string algname = vm["algorithm"].as<std::string>();

    if (algname == "pmmcs") {
        Htrans = agdmhs::mmcs_transversal(H, num_threads, cutoff_size);
    } else if (algname == "fka") {
        check_threads(num_threads);
        check_cutoff(cutoff_size);

        Htrans = agdmhs::fka_transversal(H);
    } else if (algname == "berge") {
        check_threads(num_threads);
        check_cutoff(cutoff_size);

        Htrans = agdmhs::berge_transversal(H);
    } else {
        std::stringstream error_message;
        error_message << "Did not recognize requested algorithm " << algname << ".";
        throw po::invalid_option_value(error_message.str());
    }

    // Print results
    std::cout << "Found " << Htrans.num_edges() << " hitting sets." << std::endl;
    std::string output_file(vm["output"].as<std::string>());
    Htrans.write_to_file(output_file);

    return 0;
}
