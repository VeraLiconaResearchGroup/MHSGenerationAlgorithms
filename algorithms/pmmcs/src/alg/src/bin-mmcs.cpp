/**
   C++ implementation of various MHS algorithms (binary)
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include "hypergraph.hpp"
#include "mmcs.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char * argv[]) {
    // SET UP ARGUMENTS
    po::options_description desc("Options");
    desc.add_options()
        ("input", po::value<std::string>()->required(), "Input hypergraph file")
        ("output", po::value<std::string>()->default_value("out.dat"), "Output transversals file")
        ("algorithm,a", po::value<std::string>()->default_value("mmcs"), "Algorithm to use")
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

    // Run chosen algorithm
    agdmhs::Hypergraph Htrans;
    std::string algname = vm["algorithm"].as<std::string>();

    if (algname == "mmcs") {
        Htrans = agdmhs::mmcs_transversal(H, num_threads, cutoff_size);
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
