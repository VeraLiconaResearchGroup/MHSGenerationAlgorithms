#!/usr/bin/env python
# coding: utf-8

# MHS algorithm benchmark result sanity checker
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

# This file is part of MHSGenerationAlgorithms.

# MHSGenerationAlgorithms is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# MHSGenerationAlgorithms is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

import argparse
import json
import logging
import os
from collections import defaultdict, Counter

def main():
    # Set up argument processing
    parser = argparse.ArgumentParser(description="MHS algorithm benchmark result sanity checker")

    # Add arguments
    parser.add_argument("algorithm_list_file", help="JSON file of algorithms to benchmark")
    parser.add_argument("result_files", nargs="+", help="Results files to be checked")
    parser.add_argument('-v', '--verbose', action="count", default=0, help="Print verbose logs (may be used multiple times)")

    # Process the arguments
    args = parser.parse_args()

    # Set up logging
    log_format = '%(levelname)s [%(asctime)s] %(message)s'

    if args.verbose == 0:
        log_level = logging.INFO
    elif args.verbose == 1:
        log_level = logging.DEBUG

    logging.basicConfig(format = log_format, level = log_level)

    # Read algorithm list
    with open(args.algorithm_list_file) as algorithm_list_file:
        alg_list = json.load(algorithm_list_file)["containers"]
    alg_set = set(alg["algName"] for alg in alg_list)

    # Validate the results files
    for result_filename in args.result_files:
        with open(result_filename) as result_file:
            result_json = json.load(result_file)
        result_name = os.path.splitext(os.path.split(result_filename)[1])[0]
        sanity_check(result_name, result_json, alg_set)

def sanity_check(result_name, result_json, alg_set):
    logging.info("Sanity checking result {0}".format(result_name))

    check_alg_sets(result_name, result_json, alg_set)
    check_set_sizes(result_name, result_json, alg_set)

def check_alg_sets(result_name, result_json, alg_set):
    # Verify that all parts of the result have the same algorithm set
    # and that it agrees with the given alg_set
    result_alg_set = set(alg["algName"] for alg in result_json["algs"])
    result_runtime_alg_set = set(alg.split('-')[0] for alg in result_json["runtimes"])

    algset_errors = result_alg_set.symmetric_difference(alg_set)
    if len(algset_errors) != 0:
        logging.info("Algset surplus: {0}, missing: {1}".format(result_alg_set.difference(alg_set), alg_set.difference(result_alg_set)))

    runtime_errors = result_runtime_alg_set.symmetric_difference(alg_set)
    if len(runtime_errors) != 0:
        logging.info("Runtime surplus: {0}, missing: {1}".format(result_runtime_alg_set.difference(alg_set), alg_set.difference(result_runtime_alg_set)))

def check_set_sizes(result_name, result_json, alg_set):
    # Verify that all the result sizes agree for each cutoff size
    if "transversal_counts" not in result_json:
        logging.info("No transversal count information for {0}".format(result_name))
        return

    result_counts = result_json["transversal_counts"]

    found_transversal_error = False

    # Record the counts separately for each cutoff
    counts_for_cutoff = defaultdict(lambda: defaultdict(list))

    for run_name_combo in result_counts:
        # Get the algorithm's name and parameters from the compressed name
        split_name = run_name_combo.split("-")
        alg_name = split_name[0]

        alg_params = {param[0]: int(param[1:]) for param in split_name[1:]}
        if "c" in alg_params:
            alg_cutoff = alg_params["c"]
        else:
            alg_cutoff = 0
        if "t" in alg_params:
            alg_threads = alg_params["t"]
        else:
            alg_threads = 1

        alg_counts = set(result_counts[run_name_combo])
        num_counts = len(alg_counts)
        if num_counts == 1:
            counts_for_cutoff[alg_cutoff][result_counts[run_name_combo][0]].append(alg_name)
        else:
            logging.info("Run {0} has inconsistent transversal counts {1}".format(run_name_combo, alg_counts))
            found_transversal_error = True

    # Check each cutoff for consistency
    for cutoff in counts_for_cutoff:
        # Generate pairs (count, algs) giving a count and the algorithms that found that many transversals
        count_and_alg_list_pairs = [(val, list(set(algs))) for val, algs in counts_for_cutoff[cutoff].items()]

        if len(count_and_alg_list_pairs) != 1:
            found_transversal_error = True
            # Print the results, compressing the most common count since it's probably correct
            sorted_clusters = sorted(count_and_alg_list_pairs, key = lambda pair: len(pair[1]), reverse = True)
            logging.info("Cutoff {0} has inconsistent transversal counts! {1} algs found {2} transversals, other results: {3}".format(cutoff, len(sorted_clusters[0][1]), sorted_clusters[0][0], sorted_clusters[1:]))

    if not found_transversal_error:
        logging.info("All available transversal counts were consistent.")


if __name__ == "__main__":
    main()

### Emacs configuration
# Local Variables:
# mode: python
# End
