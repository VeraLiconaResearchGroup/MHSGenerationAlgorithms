#!/usr/bin/env python

# MHS algorithm correctness tester
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import argparse
import json
import pyalgorun
import logging
import copy

# Set up argument processing
parser = argparse.ArgumentParser(description="MHS algorithm correctness tester")

# Suppress log noise from requests library
logging.getLogger("requests").setLevel(logging.WARNING)

# Add arguments
parser.add_argument("algorithm_list_file", type=argparse.FileType('r'), help="JSON file of algorithms to benchmark")
parser.add_argument("test_data_file", type=argparse.FileType('r'), help="Test input file containing both sets and transversals")
parser.add_argument("errors_file", type=argparse.FileType('w'), help="File to write errors in JSON format")
parser.add_argument("-j", dest="num_threads", type=int, nargs='*', help="Number of threads to use for supporting algorithms")
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

test_data_dict = json.load(args.test_data_file)
input_str = json.dumps(test_data_dict)

correct_transversals = frozenset(frozenset(transversal) for transversal in test_data_dict["transversals"])

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

# Launch containers
logging.info("Launching containers")
alg_collection = pyalgorun.AlgorunContainerCollection(alg_list, docker_base_url = args.docker_base_url)

# Run the tests and store the timing results
tests_failed = []

logging.info("Running algorithms")
results = alg_collection.run_all_with_input(input_str)
for algname, algresult in results.iteritems():
    # Run the algorithm
    try:
        result = json.loads(algresult)
    except ValueError:
        errormessage = "Algorithm {0} returned invalid JSON: {1}".format(algname, algresult)
        raise RuntimeError(errormessage)

    logging.debug("Algorithm {0} finished.".format(algname))

    # Store the transversals as sets in a set for easy equality testing
    transversals = frozenset(frozenset(transversal) for transversal in result["transversals"])

    # Test the result for correctness and write diagnostics in case of failure
    if transversals != correct_transversals:
        false_includes = list(list(transversal) for transversal in transversals.difference(correct_transversals))
        false_excludes = list(list(transversal) for transversal in correct_transversals.difference(transversals))

        errors = {
            "false_excludes": false_excludes,
            "false_includes": false_includes
        }

        result = {"algName": algname, "errors": errors}

        tests_failed.append(result)

if len(tests_failed) == 0:
    logging.warning("All tests succeded.")
else:
    result = {"correctTransversals": test_data_dict["transversals"], "algErrors": tests_failed}
    logging.warning("{0} tests failed. Dumping results to {1}.".format(len(tests_failed), args.errors_file.name))
    json.dump(result, args.errors_file, indent=4, separators=(',', ': ')) # Pretty-print the output


# Close up shop
alg_collection.close()
