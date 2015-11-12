#!/usr/bin/env python

# Script to convert FIMDR data sets to JSON sets of positive integer indices
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

import json
import argparse

# Set up argument parsing
parser = argparse.ArgumentParser(description='FIMDR dataset converter')

parser.add_argument("input_file", help="Input file to process")
parser.add_argument("output_file", help="Output destination")

args = parser.parse_args()

# Open the input file
with open(args.input_file) as input_file:
    # Proceed line-by-line through the file
    # Each line is a space-separated list of decimal integers, which
    # we process into a (sorted) Python list of ints
    line_as_list = lambda line_as_string: sorted(int(vert) for vert in line_as_string.split())
    sets = [line_as_list(line) for line in input_file]

# Once we finish all the lines, we generate a dictionary representing this result
result = {"sets": sets}

# Finally, we write the result out as a compliant JSON file
with open(args.output_file, 'w') as output_file:
    json.dump(result, output_file)
