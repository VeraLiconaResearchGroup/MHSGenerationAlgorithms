/**
   C++ implementation of the MMCS algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#include <string>
#include <vector>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <omp.h>

#include <boost/program_options.hpp>

#include "concurrentqueue.h"
#include "mmcs.h"

typedef boost::dynamic_bitset<> bitset;
typedef std::vector<bitset> bsvector;
typedef bitset::size_type bindex;
typedef bsvector::size_type bsvindex;

namespace po = boost::program_options;

int fork_at_depth = 0; // Tuneable parameter

moodycamel::ConcurrentQueue<bitset> HittingSets;

int main(int argc, char * argv[]) {
    // SET UP ARGUMENTS
    po::options_description desc("Options");
    desc.add_options()
        ("input", po::value<std::string>()->required(), "Input hypergraph file")
        ("output", po::value<std::string>()->default_value("out.dat"), "Output transversals file")
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
    omp_set_num_threads(num_threads);

    const size_t cutoff_size = (vm["cutoff-size"].as<int>());

    po::notify(vm);

    // Process input file
    std::string input_file(vm["input"].as<std::string>());
    bsvector H = hypergraph_from_file(input_file);
    bindex n_edges = H.size();
    bsvindex n_verts = H[0].size();

    // SET UP INTERNAL VARIABLES
    // Candidate hitting set
    bitset S (n_verts);
    S.reset(); // Initially empty

    // Eligible vertices
    bitset CAND (n_verts);
    CAND.set(); // Initially full


    // Which edges each vertex is critical for
    bsvector crit (n_verts, bitset(n_edges));
    for (auto & critset: crit) {
        critset.reset(); // Each one initially empty
    }

    // Which edges are uncovered
    bitset uncov (n_edges);
    uncov.set(); // Initially full

    // RUN ALGORITHM
    {
#pragma omp parallel shared(H)
#pragma omp single
        extend_or_confirm_set(H, S, CAND, crit, uncov, cutoff_size);
#pragma omp taskwait
    }

    // Print results
    std::cout << "Found " << HittingSets.size_approx() << " hitting sets." << std::endl;
    std::string output_file(vm["output"].as<std::string>());
    write_results_to_file(output_file);
}

bsvector hypergraph_from_file(const std::string & hypergraph_file) {
    // Set up file reader
    std::ifstream hypergraph_filestream(hypergraph_file);
    if (!hypergraph_filestream.good()) {
        std::stringstream errorMessage;
        errorMessage << "Could not open hypergraph file " << hypergraph_file << " for reading.";
        throw std::runtime_error(errorMessage.str());
    }

    // Set up intermediate variables
    std::vector<std::vector<bindex>> edges;
    bindex max_vertex = 0;
    bsvindex n_edges = 0;

    // Read the file line by line
    for (std::string line; std::getline(hypergraph_filestream, line); ) {
        // Each line is an edge
        std::istringstream linestream(line);
        ++n_edges;
        std::vector<bindex> edge;
        for (bindex v; linestream >> v; ) {
            // Each word of the line is a vertex
            edge.push_back(v);
            max_vertex = std::max(max_vertex, v);
        }
        edges.push_back(edge);
    }

    // Set up the hypergraph as a vector of bitsets
    bsvector H (n_edges, bitset(max_vertex+1));
    for (bsvindex i = 0; i < edges.size(); ++i) {
        for (auto& v: edges[i]) {
            H[i][v] = true;
        }
    }
    return H;
}

void write_results_to_file(const std::string & output_file) {
    // Set up file writer
    std::ofstream output_filestream(output_file);
    if (!output_filestream.good()) {
        std::stringstream errorMessage;
        errorMessage << "Could not open output file " << output_file << " for writing.";
        throw std::runtime_error(errorMessage.str());
    }

    bitset result;
    while (HittingSets.try_dequeue(result)) {
        bindex i = result.find_first();
        while (i != bitset::npos) {
            output_filestream << i << " ";
            i = result.find_next(i);
        }
        output_filestream << std::endl;
    }
}

void extend_or_confirm_set(bsvector H,
                           bitset S,
                           bitset CAND,
                           bsvector crit,
                           bitset uncov,
                           size_t cutoff_size){
    // if uncov is empty, S is a hitting set
    if (uncov.none()) {
        HittingSets.enqueue(S);
        return;
    }

    // If CAND is empty or S is too big, S cannot be extended, so we're done
    if (CAND.none() or (cutoff_size > 0 and S.count() >= cutoff_size)) {
        return;
    }

    // Otherwise, get an uncovered edge and remove its elements from CAND
    // TODO: Implement the optimization of Murakami and Uno
    bitset e = H[uncov.find_first()]; // Just use the first set in uncov
    bitset C = CAND & e; // intersection
    CAND = CAND & (~e); // difference

    // Test all the vertices in C
    auto vert_index = C.find_first();
    while (vert_index != bitset::npos) {
        // Update uncov and crit by iterating over edges containing the vertex
        bitset new_uncov = uncov;
        bsvector new_crit = crit;
        for (bsvindex edge_index = 0; edge_index < H.size(); ++edge_index) {
            // If the vertex is in this edge, proceed
            if (H[edge_index][vert_index]) {
                // Remove e from all crit[v]'s
                for (bindex v = 0; v < new_crit.size(); ++v) {
                    new_crit[v][edge_index] = false;
                }

                // If this edge was new_uncovered, it is no longer, but v is now new_critical for it
                if (new_uncov[edge_index] == true) {
                    new_uncov[edge_index] = false;
                    new_crit[vert_index][edge_index] = true;
                }
            }
        }

        // Construct the new candidate hitting set
        bitset newS = S;
        newS[vert_index] = true;

        // Test the minimality condition on newS
        bool is_minimal = true;
        auto v = newS.find_first();
        while (v != bitset::npos and is_minimal) {
            if (new_crit[v].none()) {
                is_minimal = false;
            }
            v = newS.find_next(v);
        }

        // If we made it this far, minimality holds, so we process newS
        if (is_minimal and new_uncov.none() and (cutoff_size == 0 or newS.count() <= cutoff_size)) {
            // In this case, newS is a valid hitting set, so we store it
            HittingSets.enqueue(newS);
        } else if (is_minimal and CAND.count() > 0 and (cutoff_size == 0 or newS.count() < cutoff_size)) {
            // In this case, newS is not yet a hitting set but is not too large either
#pragma omp task untied
            extend_or_confirm_set(H, newS, CAND, new_crit, new_uncov, cutoff_size);
        }

        // Update CAND and proceed to new vertex
        CAND[vert_index] = true;
        vert_index = C.find_next(vert_index);
    }
}
