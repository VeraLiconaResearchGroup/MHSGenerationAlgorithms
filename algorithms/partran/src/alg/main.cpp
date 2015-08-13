// -*- C++ -*-
#include "sort.h"
#include "trvgen.h"
#include "gen.h"
#include <cstdlib>
#include <ctime>

#include "boost/program_options.hpp"
#include <cilk/cilk_api.h>
#include <fstream>

using namespace std;
namespace po = boost::program_options;
// #define cilk_sync
// #define cilk_spawn
// #define cilk_for for

/**
 * cilk_main:
 * method = argc[1] :
 *
 * method = 0 =====> random input
 * ne :: argv[2],
 * nv :: argv[3] nodes. the number of nodes should be a multiple of 32,
 * otherwise need to modify a bit "generate_rand"
 * Example: main 0 5 32

 * method = 1 =====> enumeration problem
 * nv :: argv2, r :: argv3, ne :: argv4
 * Example: main 1 40 10 847660528

 * method = 2 =====> lovasz hypergraph of rank r
 * r :: argv2
 * Example: main 2 6

 * method = 3 =====> read files from Kavvadias and Elias
 **/

int main(int argc, char **argv)
{

    // Argument processing
    po::options_description desc("Options");
    desc.add_options()
        ("input-file", po::value<std::string>()->required(), "Input data file")
        ("output-file", po::value<std::string>()->required(), "Output data file")
        ("num-threads,t", po::value<std::string>()->default_value("1"), "Number of threads")
        ("help,h", "Display this help message")
        ;

    po::positional_options_description p;
    p.add("input-file", 1);
    p.add("output-file", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

    if (vm.count("help") or argc == 1) {
        std::cout << desc << std::endl;
        return 1;
    };

    po::notify(vm);

    // Process thread-related options
    const char* nthreads = vm["num-threads"].as<string>().c_str();
    __cilkrts_set_param("nworkers", nthreads);

    // Set up local variables
    int nv, elem_size;
    long long ne;
    sfixn *edges;
    int r;

    // Read input
    FILE *fp;
    const char* infile = vm["input-file"].as<string>().c_str();
    if((fp = fopen(infile, "r")) == NULL)
        return -1;

    edges = read_files(ne, nv, elem_size, fp);
    fclose(fp);

    // Run algorithm
    unsigned *ts;
    long long nbout;

    ts = trvgen(nbout, edges, ne, elem_size, nv);

    // Write output
    std::string outfile = vm["output-file"].as<string>();
    std::ofstream ofs (outfile.c_str(), std::ofstream::out);

    for(long long i=0; i<nbout; i++){
        sfixn *edge = ts + i*elem_size;
        for(int i = 0; i < nv; i++){
            if(is_set(edge, i))
                ofs << "1";
            else
                ofs << "0";
        }
        ofs << std::endl;
    }

    ofs.close();

    // Clean up and exit
    free(edges);
    free(ts);
    return 0;
}
