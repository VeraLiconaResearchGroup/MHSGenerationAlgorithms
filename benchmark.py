#!/usr/bin/env python

# MHS algorithm benchmark runner
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import argparse
import json
import pyalgorun
import logging
import time

# Set up argument processing
parser = argparse.ArgumentParser(description="MHS algorithm benchmark runner")

# Suppress log noise from requests library
logging.getLogger("requests").setLevel(logging.WARNING)

# Add arguments
parser.add_argument("algorithm_list_file", type=argparse.FileType('r'), help="JSON file of algorithms to benchmark")
parser.add_argument("input_data_file", type=argparse.FileType('r'), help="Input file to be passed to each algorithm")
parser.add_argument("output_data_file", type=argparse.FileType('w'), help="Output file to write results")
parser.add_argument("-n", dest="num_tests", type=int, default=1, help="Number of test iterations")
parser.add_argument("-j", dest="num_threads", type=int, default=1, help="Number of concurrent tests to run")
parser.add_argument("-d", dest="docker_base_url", default=None, help="Base URL for Docker client")
parser.add_argument('-v', '--verbose', action="count", default=0, help="Print verbose logs (may be used multiple times)")

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

# Launch containers
logging.info("Launching containers")
alg_collection = pyalgorun.AlgorunContainerCollection(alg_list, docker_base_url = args.docker_base_url, num_threads = args.num_threads)

time.sleep(1)

# Set up a dict to store the timing results
runtimes = {alg.name(): [] for alg in alg_collection}

# Run the tests and store the timing results
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
