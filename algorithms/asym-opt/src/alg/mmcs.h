/**
   C++ implementation of the MMCS algorithm
   Copyright Vera-Licona Research Group (C) 2015
   Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>
**/

#ifndef _MMCS_MAIN__H
#define _MMCS_MAIN__H

#include <vector>
#include <boost/dynamic_bitset.hpp>

void extend_or_confirm_set(std::vector<boost::dynamic_bitset<>> H,
                           boost::dynamic_bitset<> S,
                           boost::dynamic_bitset<> CAND,
                           std::vector<boost::dynamic_bitset<>> crit,
                           boost::dynamic_bitset<> uncov);

std::vector<boost::dynamic_bitset<>> hypergraph_from_file(const std::string & hypergraph_file);
void write_results_to_file(const std::string & output_file);

#endif
