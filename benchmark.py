#!/usr/bin/env python
# coding: utf-8

# MHS algorithm benchmark runner
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import argparse
import json
import pyalgorun
import logging
import time
import sys
from collections import defaultdict

MAX_TRIES = 1

def main():
    # Set up argument processing
    parser = argparse.ArgumentParser(description="MHS algorithm benchmark runner")

    # Suppress log noise from requests library
    logging.getLogger("requests").setLevel(logging.WARNING)

    # Add arguments
    parser.add_argument("algorithm_list_file", help="JSON file of algorithms to benchmark")
    parser.add_argument("input_data_file", help="Input file to be passed to each algorithm")
    parser.add_argument("output_data_file", help="Output file to write results")
    parser.add_argument("-n", "--num_tests", type=int, default=1, help="Number of test iterations")
    parser.add_argument("-j", "--num_threads", type=int, nargs='*', help="Numbers of threads to use for supporting algorithms")
    parser.add_argument("-c", "--cutoff_sizes", type=int, nargs='*', help="Cutoff sizes to use for supporting algorithms (if specified, full test is not run unless 0 is included)")
    parser.add_argument("-t", "--timeout", type=int, default=0, help="Kill algorithms after this many seconds (0 to run forever)")
    parser.add_argument("-d", "--docker_base_url", default=None, help="Base URL for Docker client")
    parser.add_argument('-v', '--verbose', action="count", default=0, help="Print verbose logs (may be used multiple times)")
    parser.add_argument('-s', '--slow', action="store_true", help="Include slow algorithms (be careful!)")
    parser.add_argument('-a', '--append', action="store_true", help="Append results to existing file, replacing repeated algorithms (otherwise, overwrite!)")

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
    with open(args.algorithm_list_file) as algorithm_list_file:
        alg_list = json.load(algorithm_list_file)["containers"]

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

    # Set up dicts to store the results
    runtimes = defaultdict(list)
    transcounts = defaultdict(list)

    # Load the previous results if requested
    original_runtimes = []
    original_transcounts = []
    original_algs = []
    original_timeout = timeout

    if args.append:
        try:
            with open(args.output_data_file) as output_data_file:
                original_output = json.load(output_data_file)
            original_runtimes = original_output["runtimes"]
            original_transcounts = original_output["transversal_counts"]
            original_algs = original_output["algs"]
            original_timeout = original_output["timeout_secs"]
        except IOError:
            sys.stderr.write("Requested file " + args.output_data_file + " did not exist, so we could not append. Continuing.\n")
            pass
        except (ValueError, KeyError):
            sys.stderr.write("Requested file " + args.output_data_file + " did not contain valid results JSON.\n")
            sys.stderr.write("Aborting so we don't destroy data.\n")
            sys.exit(1)

        if original_timeout != timeout:
            raise ValueError("Given timeout " + timeout + " ≠ original timeout " + original_timeout + ". Cannot consistently update data.")

    # Run the tests and store the timing results
    logging.info("Running algorithms.")
    for alg in alg_collection:
        # Only iterate over cases supported by the algorithm
        alg_entry = next(entry for entry in alg_list if entry["algName"] == alg._name)
        alg_thread_list = num_threads if alg_entry.get("threads") else [1]
        alg_cutoff_list = cutoff_sizes if alg_entry.get("cutoff") else [0]

        timeout_config_pairs = []

        for t in alg_thread_list:
            for c in alg_cutoff_list:
                # We'll try this run up to MAX_TRIES times, allowing
                # for certain sporadic errors
                num_tries = 0

                for i in range(args.num_tests):
                    # NOTE: We assume that increasing the cutoff or
                    # decreasing the number of threads will never decrease
                    # runtime. This only matters for runtimes that move
                    # from above to below the timeout, so it should be
                    # safe as long as the timeout is large and the
                    # algorithms aren't *very* badly behaved.

                    # Check whether a faster configuration has timed out
                    alg_has_timed_out = False
                    for old_t, old_c in timeout_config_pairs:
                        if old_t >= t and old_c <= c:
                            logging.info("{0} <= {1}, so killing".format((old_t, old_c), (t, c)))
                            alg_has_timed_out = True

                    if num_tries > MAX_TRIES:
                        logging.info("{0} tries failed, so killing".format(num_tries))
                        alg_has_timed_out = True

                    # Only execute this run if a faster configuration
                    # has not timed out
                    if not alg_has_timed_out:
                        logging.info("Running algorithm {0} with {1} threads and cutoff size {2}, run {3}/{4}".format(alg, t, c, i+1, args.num_tests))
                        config = {"THREADS": t, "CUTOFF_SIZE": c}
                        alg.change_config(config)
                        newname = alg._name
                        if t > 1:
                            newname += "-t{0}".format(t)
                        if c > 0:
                            newname += "-c{0}".format(c)

                        try:
                            result_str = alg.run_alg(args.input_data_file, timeout)
                            result = json.loads(result_str)
                            time_taken = float(result["timeTaken"])
                            transcounts[newname].append(len(result["sets"]))
                        except (pyalgorun.AlgorunTimeout):
                            logging.info("Run {0} failed to complete in {1} sec.".format(newname, timeout))
                            timeout_config_pairs.append((t, c))
                            time_taken = float('inf')
                            alg.restart()
                        except (pyalgorun.AlgorunError, ValueError) as e:
                            # Rerun to see if this was a one-off glitch
                            logging.info("Run {0} failed to complete with error {1}".format(newname, e))
                            alg.restart()
                            num_tries += 1
                            i -= 1
                            continue

                    else:
                        time_taken = float('inf')

                    runtimes[newname].append(time_taken)
                    logging.info("Finished {0} run in {1} sec.".format(newname, time_taken))

        # Kill the algorithm once we're done computing with it,
        # regardless of the outcome
        alg.stop()

    # All done! Time to close up shop.
    alg_collection.close()

    # Combine old data with new
    # First, combine algorithm lists, giving preference to new ones
    for alg in alg_list:
        for orig_alg in original_algs:
            if orig_alg["algName"] == alg["algName"]:
                original_algs.remove(orig_alg)

    alg_list += original_algs

    # Then combine result lists, giving preference to new ones
    for orig_alg in original_runtimes:
        if orig_alg not in runtimes:
            runtimes[orig_alg] = original_runtimes[orig_alg]

    for orig_alg in original_transcounts:
        if orig_alg not in transcounts:
            transcounts[orig_alg] = original_transcounts[orig_alg]

    # Build output dict
    output = {
        "timeout_secs": timeout,
        "runtimes": runtimes,
        "transversal_counts": transcounts,
        "algs": alg_list,
    }

    # Print the results
    with open(args.output_data_file, 'w') as output_data_file:
        json.dump(output, output_data_file, indent=4, separators=(',', ': '), sort_keys = True) # Pretty-print the output

if __name__ == "__main__":
    main()

### Emacs configuration
# Local Variables:
# mode: python
# End
