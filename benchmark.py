#!/usr/bin/env python

# MHS algorithm benchmark runner
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import argparse
import json
import pyalgorun
import logging
import time
import copy
from collections import defaultdict

# Set up argument processing
parser = argparse.ArgumentParser(description="MHS algorithm benchmark runner")

# Suppress log noise from requests library
logging.getLogger("requests").setLevel(logging.WARNING)

# Add arguments
parser.add_argument("algorithm_list_file", type=argparse.FileType('r'), help="JSON file of algorithms to benchmark")
parser.add_argument("input_data_file", type=argparse.FileType('r'), help="Input file to be passed to each algorithm")
parser.add_argument("output_data_file", type=argparse.FileType('w'), help="Output file to write results")
parser.add_argument("-n", dest="num_tests", type=int, default=1, help="Number of test iterations")
parser.add_argument("-j", dest="num_threads", type=int, nargs='*', help="Numbers of threads to use for supporting algorithms")
parser.add_argument("-c", dest="cutoff_sizes", type=int, nargs='*', help="Cutoff sizes to use for supporting algorithms (if specified, full test is not run unless 0 is included)")
parser.add_argument("-d", dest="docker_base_url", default=None, help="Base URL for Docker client")
parser.add_argument('-v', '--verbose', action="count", default=0, help="Print verbose logs (may be used multiple times)")
parser.add_argument('-s', '--slow', dest="slow", action="store_true", help="Include slow algorithms (be careful!)")

# Process the arguments
args = parser.parse_args()

# Set up logging
if args.verbose == 0:
    logging.basicConfig(level=logging.WARNING)
elif args.verbose == 1:
    logging.basicConfig(level=logging.INFO)
else:
    logging.basicConfig(level=logging.INFO)

# Read and process files
alg_list = json.load(args.algorithm_list_file)["containers"]
input_dict = json.load(args.input_data_file)
input_str = json.dumps(input_dict)

# Filter out slow algorithms if requested
if not args.slow:
    alg_list = filter(lambda alg: not alg.get("slow"), alg_list)

# Filter out non-threading algorithms if appropriate
num_threads = args.num_threads
if num_threads is None:
    num_threads = [1]

if 1 not in num_threads:
    alg_list = filter(lambda alg: alg.get("threads"), alg_list)

# Filter out non-cutoff algorithms if appropriate
cutoff_sizes = args.cutoff_sizes
if cutoff_sizes is None:
    cutoff_sizes = [0]

if 0 not in cutoff_sizes:
    alg_list = filter(lambda alg: alg.get("cutoff"), alg_list)

# Launch containers
logging.info("Launching containers")
alg_collection = pyalgorun.AlgorunContainerCollection(alg_list, docker_base_url = args.docker_base_url)

# Set up a dict to store the timing results
runtimes = defaultdict(list)

# Run the tests and store the timing results
logging.info("Running algorithms.")
for alg in alg_collection:
    # Only iterate over cases supported by the algorithm
    alg_entry = next(entry for entry in alg_list if entry["algName"] == alg._name)
    alg_thread_list = num_threads if alg_entry.get("threads") else [1]
    alg_cutoff_list = cutoff_sizes if alg_entry.get("cutoff") else [0]

    for t in alg_thread_list:
        for c in alg_cutoff_list:
            for i in range(args.num_tests):
                logging.info("Running algorithm {0} with {1} threads and cutoff size {2}, run {3}/{4}".format(alg, t, c, i+1, args.num_tests))
                config = {"THREADS": t, "CUTOFF_SIZE": c}
                alg.change_config(config)
                newname = "{0}-t{1}-c{2}".format(alg._name, t, c)

                result = json.loads(alg.run_alg(input_str))
                time_taken = float(result["timeTaken"])
                runtimes[newname].append(time_taken)
                logging.info("Finished {0} run in {1} sec.".format(newname, time_taken))

# All done! Time to close up shop.
alg_collection.close()

# Build output dict
output = {
    "runtimes": runtimes,
    "algs": alg_list,
}

# Print the results
json.dump(output, args.output_data_file, indent=4, separators=(',', ': '), sort_keys = True) # Pretty-print the output
