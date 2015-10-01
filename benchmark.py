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

def main():
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
    parser.add_argument("-t", dest="timeout", type=int, default=0, help="Kill algorithms after this many seconds (0 to run forever)")
    parser.add_argument("-d", dest="docker_base_url", default=None, help="Base URL for Docker client")
    parser.add_argument('-v', '--verbose', action="count", default=0, help="Print verbose logs (may be used multiple times)")
    parser.add_argument('-s', '--slow', dest="slow", action="store_true", help="Include slow algorithms (be careful!)")

    # Process the arguments
    args = parser.parse_args()

    # Set up logging
    log_format = '%(levelname)s [%(asctime)s] %(message)s'

    if args.verbose == 0:
        log_level = logging.WARNING
    elif args.verbose == 1:
        log_level = logging.INFO
    else:
        log_level = logging.DEBUG

    logging.basicConfig(format = log_format, level = log_level)

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

    num_threads.sort(reverse=True)

    # Filter out non-cutoff algorithms if appropriate
    cutoff_sizes = args.cutoff_sizes
    if cutoff_sizes is None:
        cutoff_sizes = [0]

    cutoff_sizes.sort()

    if 0 in cutoff_sizes:
        # Put 0 at the end of the list so the timeout filtering logic works
        cutoff_sizes.remove(0)
        cutoff_sizes.append(0)
    else:
        # Remove algorithms that don't support cutoff
        alg_list = filter(lambda alg: alg.get("cutoff"), alg_list)

    # Process timeout
    timeout = args.timeout
    if timeout == 0:
        timeout = None

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
            alg_has_timed_out = False

            for c in alg_cutoff_list:
                for i in range(args.num_tests):
                    logging.info("Running algorithm {0} with {1} threads and cutoff size {2}, run {3}/{4}".format(alg, t, c, i+1, args.num_tests))
                    config = {"THREADS": t, "CUTOFF_SIZE": c}
                    alg.change_config(config)
                    newname = alg._name
                    if t > 1:
                        newname += "-t{0}".format(t)
                    if c > 0:
                        newname += "-c{0}".format(c)

                    if not alg_has_timed_out:
                        try:
                            result_str = alg.run_alg(input_str, timeout)
                            result = json.loads(result_str)
                            time_taken = float(result["timeTaken"])
                        except (pyalgorun.AlgorunTimeout, ValueError):
                            logging.info("Run {0} failed to complete in {1} sec.".format(newname, timeout))
                            alg_has_timed_out = True
                            time_taken = -1
                    else:
                        time_taken = -1

                    runtimes[newname].append(time_taken)
                    logging.info("Finished {0} run in {1} sec.".format(newname, time_taken))

        # Kill the algorithm once we're done computing with it,
        # regardless of the outcome
        alg.stop()

    # All done! Time to close up shop.
    alg_collection.close()

    # Build output dict
    output = {
        "timeout_secs": timeout,
        "runtimes": runtimes,
        "algs": alg_list,
    }

    # Print the results
    json.dump(output, args.output_data_file, indent=4, separators=(',', ': '), sort_keys = True) # Pretty-print the output

if __name__ == "__main__":
    main()

### Emacs configuration
# Local Variables:
# mode: python
# End
:
