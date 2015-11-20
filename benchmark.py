#!/usr/bin/env python
# coding: utf-8

# MHS algorithm benchmark runner
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
import pyalgorun
import logging
import time
import sys
import os
from collections import defaultdict

MAX_TRIES = 1

def run_benchmarks(alg_list,
                   docker_base_url,
                   input_data_filename,
                   output_data_filename,
                   alg_results_dirname,
                   num_tests,
                   cutoff_list,
                   threads_list,
                   timeout,
                   append = False):
    # Launch containers
    logging.info("Launching containers")
    alg_collection = pyalgorun.AlgorunContainerCollection(alg_list, docker_base_url = docker_base_url)

    # Set up dicts to store the results
    runtimes = defaultdict(list)
    transcounts = defaultdict(list)

    # Load the previous results if requested
    original_runtimes = []
    original_transcounts = []
    original_algs = []
    original_timeout = timeout

    if append:
        try:
            with open(output_data_filename) as output_data_file:
                original_output = json.load(output_data_file)
            original_runtimes = original_output["runtimes"]
            original_algs = original_output["algs"]
            original_timeout = original_output["timeout_secs"]
        except IOError:
            sys.stderr.write("Requested file " + output_data_filename + " did not exist, so we could not append. Continuing.\n")
            pass
        except (ValueError, KeyError):
            sys.stderr.write("Requested file " + output_data_filename + " did not contain valid results JSON.\n")
            sys.stderr.write("Aborting so we don't destroy data.\n")
            sys.exit(1)

        if original_timeout != timeout:
            raise ValueError("Given timeout " + timeout + " ≠ original timeout " + original_timeout + ". Cannot consistently update data.")

    # Run the tests and store the timing results
    logging.info("Running algorithms.")
    for alg in alg_collection:
        # Only iterate over cases supported by the algorithm
        alg_entry = next(entry for entry in alg_list if entry["algName"] == alg._name)
        alg_thread_list = threads_list if alg_entry.get("threads") else [1]
        alg_cutoff_list = cutoff_list if alg_entry.get("cutoff") else [0]

        timeout_config_pairs = []

        for t in alg_thread_list:
            for c in alg_cutoff_list:
                # We'll try this run up to MAX_TRIES times, allowing
                # for certain sporadic errors
                num_tries = 0


                for i in range(num_tests):
                    # NOTE: We assume that increasing the cutoff or
                    # decreasing the number of threads will never decrease
                    # runtime. This only matters for runtimes that move
                    # from above to below the timeout, so it should be
                    # safe as long as the timeout is large and the
                    # algorithms aren't *very* badly behaved.

                    # Check whether a faster configuration has timed out
                    def old_test_subsumes_new_test(old_pair, new_pair):
                        old_t, old_c = old_pair
                        t, c = new_pair

                        if t <= old_t:
                            if c == 0:
                                return True
                            if (old_c <= c and old_c != 0):
                                return True
                        return False

                    alg_has_timed_out = False
                    for old_pair in timeout_config_pairs:
                        if old_test_subsumes_new_test(old_pair, (t, c)):
                            logging.info("{0} <= {1}, so killing".format(old_pair, (t, c)))
                            alg_has_timed_out = True

                    if num_tries > MAX_TRIES:
                        logging.info("{0} tries failed, so killing".format(num_tries))
                        timeout_config_pairs.append((t, c))
                        alg_has_timed_out = True

                    newname = alg._name
                    if t > 1:
                        newname += "-t{0}".format(t)
                    if c > 0:
                        newname += "-c{0}".format(c)

                    input_shortname = os.path.splitext(os.path.basename(input_data_filename))[0]
                    result_out_filename = "{0}/{1}.{2}.r{3}.json".format(alg_results_dirname, input_shortname, newname, i)
                    result = None

                    # Try to load old results if applicable
                    try:
                        with open(result_out_filename) as result_out_file:
                            result = json.load(result_out_file)
                            time_taken = float(result["timeTaken"])
                            transcount = len(result["transversals"])
                            logging.info("Using cached results for algorithm {0} with {1} threads and cutoff size {2}, run {3}/{4}".format(alg, t, c, i+1, num_tests))
                    except (IOError, ValueError):
                        pass

                    # Only execute this run if a faster configuration
                    # has not timed out
                    if not alg_has_timed_out and result is None:
                        logging.info("Running algorithm {0} with {1} threads and cutoff size {2}, run {3}/{4}".format(alg, t, c, i+1, num_tests))
                        config = {"THREADS": t, "CUTOFF_SIZE": c}
                        alg.change_config(config)

                        try:
                            result_str = alg.run_alg(input_data_filename, timeout)
                            result = json.loads(result_str)
                            time_taken = float(result["timeTaken"])
                            transcount = len(result["transversals"])
                            with open(result_out_filename, 'w') as result_outfile:
                                json.dump(result, result_outfile, separators=(',', ': '), sort_keys = True) # Pretty-print the output
                        except (pyalgorun.AlgorunTimeout):
                            logging.info("Run {0} failed to complete in {1} sec.".format(newname, timeout))
                            timeout_config_pairs.append((t, c))
                            time_taken = float('inf')
                            transcount = None
                            alg.restart()
                        except (pyalgorun.AlgorunError, ValueError) as e:
                            # Rerun to see if this was a one-off glitch
                            logging.info("Run {0} failed to complete with error {1}".format(newname, e))
                            alg.restart()
                            num_tries += 1
                            i -= 1
                            continue
                    elif alg_has_timed_out:
                        time_taken = float('inf')
                        transcount = None

                    runtimes[newname].append(time_taken)
                    if transcount is not None:
                        transcounts[newname].append(transcount)
                    else:
                        transcount = "no" # This is only used in the logging statement below

                    logging.info("Finished {0} run in {1} sec., found {2} MHSes.".format(newname, time_taken, transcount))

        # Kill the algorithm once we're done computing with it,
        # regardless of the outcome
        alg.stop()

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

    return output


def main():
    # Set up argument processing
    parser = argparse.ArgumentParser(description="MHS algorithm benchmark runner")

    # Suppress log noise from requests library
    logging.getLogger("requests").setLevel(logging.WARNING)

    # Add arguments
    parser.add_argument("algorithm_list_file", help="JSON file of algorithms to benchmark")
    parser.add_argument("input_data_file", help="Input file to be passed to each algorithm")
    parser.add_argument("output_dir", help="Output directory to write results")
    parser.add_argument("-n", "--num_tests", type=int, default=1, help="Number of test iterations")
    parser.add_argument("-j", "--num_threads", type=int, nargs='*', help="Numbers of threads to use for supporting algorithms")
    parser.add_argument("-c", "--cutoff_sizes", type=int, nargs='*', help="Cutoff sizes to use for supporting algorithms (if specified, full test is not run unless 0 is included)")
    parser.add_argument("-t", "--timeout", type=int, default=0, help="Kill algorithms after this many seconds (0 to run forever)")
    parser.add_argument("-d", "--docker_base_url", default=None, help="Base URL for Docker client")
    parser.add_argument('-v', '--verbose', action="count", default=0, help="Print verbose logs (may be used multiple times)")
    parser.add_argument('-s', '--slow', action="store_true", help="Include slow algorithms (be careful!)")
    parser.add_argument('-a', '--append', action="store_true", help="Append results to existing file, replacing repeated algorithms (otherwise, overwrite!)")
    parser.add_argument('-b', '--bigfiles_dir', default=None, help="Directory to store big files")

    # Process the arguments
    args = parser.parse_args()

    # Set up logging
    infile_basename = os.path.splitext(os.path.basename(args.input_data_file))[0]
    log_format = '{0}: [%(asctime)-15s] %(message)s'.format(infile_basename)

    if args.verbose == 0:
        log_level = logging.WARNING
    elif args.verbose == 1:
        log_level = logging.INFO
    else:
        log_level = logging.DEBUG

    logging.basicConfig(format = log_format, level = log_level)

    logfile_path = "{0}/{1}.log".format(args.output_dir, infile_basename)

    try:
        os.makedirs(args.output_dir)
    except OSError:
        if not os.path.isdir(args.output_dir):
            raise

    if (args.bigfiles_dir is not None):
        bigfiles_dir = args.bigfiles_dir
        try:
            os.makedirs(bigfiles_dir)
        except OSError:
            if not os.path.isdir(args.output_dir):
                raise
    else:
        bigfiles_dir = args.output_dir

    logfile_formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    logfile_handler = logging.FileHandler(logfile_path)
    logfile_handler.setFormatter(logfile_formatter)
    logging.getLogger().addHandler(logfile_handler)

    # Read and process files
    with open(args.algorithm_list_file) as algorithm_list_file:
        alg_list = json.load(algorithm_list_file)["containers"]

    # Filter out slow algorithms if requested
    if not args.slow:
        alg_list = filter(lambda alg: not alg.get("slow"), alg_list)

    # Filter out non-threading algorithms if appropriate
    threads_list = args.num_threads
    if threads_list is None:
        threads_list = [1]

    if 1 not in threads_list:
        alg_list = filter(lambda alg: alg.get("threads"), alg_list)

    threads_list.sort(reverse=True)

    # Filter out non-cutoff algorithms if appropriate
    cutoff_list = args.cutoff_sizes
    if cutoff_list is None:
        cutoff_list = [0]

    cutoff_list.sort()

    if 0 in cutoff_list:
        # Put 0 at the end of the list so the timeout filtering logic works
        cutoff_list.remove(0)
        cutoff_list.append(0)
    else:
        # Remove algorithms that don't support cutoff
        alg_list = filter(lambda alg: alg.get("cutoff"), alg_list)

    # Process timeout
    timeout = args.timeout
    if timeout == 0:
        timeout = None

    # Process filenames
    output_data_filename = "{0}/{1}.json".format(args.output_dir, infile_basename)


    # Run the benchmarks
    results = run_benchmarks(alg_list,
                             args.docker_base_url,
                             args.input_data_file,
                             output_data_filename,
                             bigfiles_dir,
                             args.num_tests,
                             cutoff_list,
                             threads_list,
                             timeout,
                             args.append)

    logging.info("Writing results to file")

    # Print the results
    with open(output_data_filename, 'w') as output_data_file:
        json.dump(results, output_data_file, indent=4, separators=(',', ': '), sort_keys = True) # Pretty-print the output

    logging.info("Benchmark complete!")

if __name__ == "__main__":
    main()

### Emacs configuration
# Local Variables:
# mode: python
# End
