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
    logging.basicConfig(level=logging.DEBUG)

# Read and process files
alg_list = json.load(args.algorithm_list_file)["containers"]
input_dict = json.load(args.input_data_file)
input_str = json.dumps(input_dict)

# Filter out slow algorithms if requested
if not args.slow:
    alg_list = filter(lambda alg: not alg.get("slow"), alg_list)

# Split out threading cases if requested
if args.num_threads is not None:
    thread_algs = filter(lambda alg: alg.get("threads"), alg_list)
    for alg in thread_algs:
        alg_list.remove(alg)
        for n in args.num_threads:
            newalg = copy.deepcopy(alg)
            newalg["algName"] = "{0}-t{1}".format(alg.get("algName"), n)
            if newalg.get("config") is None:
                newalg["config"] = {}
                newalg["config"]["THREADS"] = n
            alg_list.append(newalg)

# Split out cutoff cases if requested
if args.cutoff_sizes is not None:
    cutoff_algs = filter(lambda alg: alg.get("cutoff"), alg_list)
    alg_list = []
    for alg in cutoff_algs:
        for c in args.cutoff_sizes:
            newalg = copy.deepcopy(alg)
            newalg["algName"] = "{0}-c{1}".format(alg.get("algName"), c)
            if newalg.get("config") is None:
                newalg["config"] = {}
                newalg["config"]["CUTOFF_SIZE"] = c
            alg_list.append(newalg)

# Launch containers
logging.info("Launching containers")
alg_collection = pyalgorun.AlgorunContainerCollection(alg_list, docker_base_url = args.docker_base_url)

# Set up a dict to store the timing results
runtimes = {alg.name(): [] for alg in alg_collection}

# Run the tests and store the timing results
logging.info("Running algorithms")
for i in range(args.num_tests):
    logging.info("Running panel {0}/{1}".format(i+1, args.num_tests))

    results = alg_collection.run_all_with_input(input_str)
    for algname, algresult in results.iteritems():
        result = json.loads(algresult)
        time_taken = float(result["timeTaken"])
        runtimes[algname].append(time_taken)
        logging.debug("Algorithm {0} finished run {1}/{2} in {3} sec.".format(algname, i+1, args.num_tests, time_taken))

# Close up shop
alg_collection.close()

# Print the results
json.dump(runtimes, args.output_data_file)
