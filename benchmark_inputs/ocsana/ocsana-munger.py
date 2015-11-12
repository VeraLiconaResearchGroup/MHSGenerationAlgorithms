#!/usr/bin/env python

# Script to convert cleaned up OCSANA paths to JSON sets of positive integer indices
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import json
import argparse

# Set up argument parsing
parser = argparse.ArgumentParser(description='OCSANA path converter')

parser.add_argument("input_file", help="Input file to process")
parser.add_argument("output_file", help="Output destination")

args = parser.parse_args()

# Open the input file
with open(args.input_file) as input_file:
    # We'll store the OCSANA node names in a dictionary {name: index} and the results in a list
    name_map = {}
    available_index = 1
    paths = []

    # The file will begin with some lines we don't care about.
    # The information we want begins with
    # "Found [n] elementary paths and [k] elementary nodes".
    # Here, we scan for that line.
    fileline = input_file.next()
    while not fileline.startswith("Found "):
        fileline = input_file.next()

    # Now we need to skip one more line to get to the good stuff
    fileline = input_file.next()

    # Now, we proceed line-by-line through the file
    for line in input_file:
        # An empty line signals the end of the paths
        if len(line) == 1:
            break

        path = []
        # Paths are delimited with two symbols, '->' and '-|'
        # We just turn them both into spaces to ease the split
        line = line.replace('->', ' ').replace('-|', ' ')
        names = line.split()

        # For each name that appears in the line, we find its code (generating
        # it if necessary) and add it to the result array
        for name in names:
            if name not in name_map:
                name_map[name] = available_index
                available_index += 1
            path.append(name_map[name])

        # Finally, we add this result to the larger collection
        paths.append(path)

# Once we finish all the lines, we generate a dictionary representing this result
result = {"sets": paths}

# We also do some quick analysis
num_verts = max(max(path) for path in paths) - min(min(path) for path in paths)
num_edges = len(paths)
num_vert_occurrences = sum(len(path) for path in paths)
avg_edge_size = num_vert_occurrences / float(num_edges)

print "Network from file {0} has {1} vertices, {2} elementary paths, and average path length {3}.".format(args.input_file, num_verts, num_edges, avg_edge_size)

# Finally, we write the result out as a compliant JSON file
with open(args.output_file, 'w') as output_file:
    json.dump(result, output_file, indent=4, separators=(',', ': '), sort_keys = True) # Pretty-print the output
