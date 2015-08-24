#!/usr/bin/env python

# MHS algorithm correctness tester
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import argparse
import json
import pyalgorun
import logging

# Set up argument processing
parser = argparse.ArgumentParser(description="MHS algorithm correctness tester")

# Suppress log noise from requests library
logging.getLogger("requests").setLevel(logging.WARNING)

# Add arguments
parser.add_argument("algorithm_list_file", type=argparse.FileType('r'), help="JSON file of algorithms to benchmark")
parser.add_argument("test_data_file", type=argparse.FileType('r'), help="Test input file containing both sets and transversals")
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

test_data_dict = json.load(args.test_data_file)
input_str = json.dumps(test_data_dict)

correct_transversals = frozenset(frozenset(transversal) for transversal in test_data_dict["transversals"])

# Launch containers
logging.info("Launching containers")
alg_collection = pyalgorun.AlgorunContainerCollection(alg_list, docker_base_url = args.docker_base_url, num_threads = args.num_threads)

# Run the tests and store the timing results
tests_failed = []

logging.info("Running algorithms")
results = alg_collection.run_all_with_input(input_str)
for algname, algresult in results.iteritems():
    # Run the algorithm
    result = json.loads(algresult)
    logging.debug("Algorithm {0} finished.".format(algname))

    # Store the transversals as sets in a set for easy equality testing
    transversals = frozenset(frozenset(transversal) for transversal in result["transversals"])

    # Test the result for correctness and write diagnostics in case of failure
    if transversals != correct_transversals:
        dumppath = "{0}.errors.json".format(algname)
        false_includes = list(list(transversal) for transversal in transversals.difference(correct_transversals))
        false_excludes = list(list(transversal) for transversal in correct_transversals.difference(transversals))

        logging.error("Algorithm {0} failed! Dumping diagnostics to {1}.".format(algname, dumppath))
        errors = {
            "false_excludes": false_excludes,
            "false_includes": false_includes
        }

        with open(dumppath, 'w') as dumpfile:
            json.dump(errors, dumpfile, indent=4, separators=(',', ': '))

if len(tests_failed) == 0:
    logging.warning("All tests succeded.")

# Close up shop
alg_collection.close()
