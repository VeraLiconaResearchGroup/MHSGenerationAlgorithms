#!/usr/bin/env python
# coding: utf-8

# MHS algorithm benchmark output collator
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
import pandas
import numpy
import os
from collections import defaultdict

sorted_alglist = ["mmcs",
                  "rs",
                  "pMMCS",
                  "pRS",
                  "hbc",
                  "bmr",
                  "htcbdd",
                  "knuth",
                  "primdecomp",
                  "mhs2",
                  "dl",
                  "fka.begk",
                  "bool.iterative",
                  "hst",
                  "hsdag",
                  "berge",
                  "ocsanaGreedy",
                  "bm",
                  "staccato"]

cutoff_algs = ["pMMCS",
               "pRS",
               "mhs2",
               "bool.iterative",
               "hst",
               "hsdag",
               "berge",
               "ocsanaGreedy",
               "staccato"
]

thread_algs = ["pMMCS",
               "pRS",
               "mhs2",
               "bm"]

omit_algs = ['ks',
             'partran']

def parse_results_algname(algname_str):
    orig_name = algname_str

    # Process the name into its components
    parts = orig_name.split("-")

    # The algorithm name always comes first
    stripped_name = orig_name[0]

    # The thread and cutoff specifications, if any, come later
    threads = 1
    cutoff = 0

    for specpart in parts[1:]:
        if specpart[0] == 't':
            threads = int(specpart[1:])
        if specpart[0] == 'c':
            cutoff = int(specpart[1:])

    # Build the return dict
    result = {
        "alg_name": stripped_name,
        "threads": threads,
        "cutoff": cutoff
    }

    return result

def algorithm_result_frame(alg, runtimes):
    # Build a DataFrame with the results of a given algorithm in the given
    # results record

    # Find the instances of this algorithm in the results file
    alg_instances = filter(lambda algname: algname.split('-')[0] == alg, runtimes)

    # For each one, find the runtimes
    results = defaultdict(dict)
    for alg_instance in alg_instances:
        alg_instance_runtimes = runtimes[alg_instance]
        median_runtime = numpy.median(alg_instance_runtimes)

        alg_instance_specs = parse_results_algname(alg_instance)
        threads = alg_instance_specs["threads"]
        cutoff = alg_instance_specs["cutoff"]
        results[cutoff][threads] = median_runtime

    # TODO: Can we annotate this DataFrame to explain the axis labels?
    result_frame = pandas.DataFrame(results)
    return result_frame

def problem_results_panel(results_json):
    runtimes = results_json["runtimes"]

    # Get the list of algorithm names
    algs = set(alg["algName"] for alg in results_json["algs"] if alg["algName"] not in omit_algs)

    # For each algorithm, build a DataFrame with its results
    results = dict()
    for alg in algs:
        results[alg] = algorithm_result_frame(alg, runtimes)

    result_panel = pandas.Panel(results)
    return result_panel

def panel4d_from_filenames(alg_filenames):
    results = dict()

    for input_filename in alg_filenames:
        with open(input_filename) as input_file:
            problem_results_json = json.load(input_file)

        problem_results = problem_results_panel(problem_results_json)

        problem_shortname = os.path.splitext(os.path.basename(input_filename))[0]
        results[problem_shortname] = problem_results

    result_panel = pandas.Panel4D(results)
    return result_panel

def generate_full_csv(data4d, basename, reverse_data_sort):
    print "Processing base case"

     # Extract the data we want
    data = data4d.ix[:,:,1,0].T.dropna(axis='columns', how='all').filter(items = sorted_alglist, axis=1)

    # Sort the input sets lexicographically
    data.sort_index(ascending = not reverse_data_sort,
                    inplace = True)

    # Construct filename
    output_file_name = "{0}.full.csv".format(basename)

    # Write the data
    data.to_csv(output_file_name,
                index_label = "Input")

def generate_cutoff_csv(data4d, basename):
    print "Processing cutoff cases"

    # Find cutoff size list
    orig_cutoff_order = data4d.axes[3]

    # Put 0 last, if appropriate, for later reordering of dataframe
    if 0 in orig_cutoff_order:
        new_cutoff_order = list(orig_cutoff_order)
        new_cutoff_order.remove(0)
        new_cutoff_order.append(0)
    else:
        new_cutoff_order = list(orig_cutoff_order)

    # Iterate over datasets
    num_datasets = len(data4d.axes[0])

    for i in xrange(num_datasets):
        # Extract the data we want
        data = data4d.ix[i,:,1,:].filter(items = cutoff_algs, axis=1)

        # Reorder cutoffs to put 0 last
        data = data.reindex(new_cutoff_order)

        # Rename 0 to "None"
        data.rename(index = {0: 'None'}, inplace = True)

        # Get the name of the dataset we sliced
        datasetname = data4d.axes[0][i]

        # Construct filename
        output_file_name = "{0}.{1}.cutoff.csv".format(basename, datasetname)

        # Write the data
        data.to_csv(output_file_name,
                    index_label = "Cutoff")

def generate_thread_csv(data4d, basename):
    print "Processing parallel cases"

    # Iterate over datasets
    num_datasets = len(data4d.axes[0])

    for i in xrange(num_datasets):
        # Extract the data we want
        data = data4d.ix[i,:,:,0].filter(items = thread_algs, axis=1)

        # Get the name of the dataset we sliced
        datasetname = data4d.axes[0][i]

        # Construct filename
        output_file_name = "{0}.{1}.thread.csv".format(basename, datasetname)

        # Write the data
        data.to_csv(output_file_name,
                    index_label = "Threads")

def main():
    # Set up argument processing
    parser = argparse.ArgumentParser(description="MHS algorithm benchmark runner")

    # Add arguments
    parser.add_argument("output_file_basename", help="Base name for output files")
    parser.add_argument("input_files", help="Input file(s) to collate", nargs="+")
    parser.add_argument("-r", "--reverse_data_sort", action="store_true", help="Sort data sets in reverse lex order")

    # Process the arguments
    args = parser.parse_args()

    # Process the files
    data4d = panel4d_from_filenames(args.input_files)

    # Generate the data4d and write the plots to files
    generate_full_csv(data4d, args.output_file_basename, args.reverse_data_sort)
    generate_cutoff_csv(data4d, args.output_file_basename)
    generate_thread_csv(data4d, args.output_file_basename)

if __name__ == "__main__":
    main()

### Emacs configuration
# Local Variables:
# mode: python
# End
