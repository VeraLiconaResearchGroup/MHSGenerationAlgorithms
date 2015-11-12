#!/usr/bin/env python

# Script to convert JSON sets of positive integer indices to FIMDR-format data files
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
    sets = json.load(input_file)["sets"]

# Write the result
with open(args.output_file, 'w') as output_file:
    for edge in sets:
        edge_as_string = " ".join(map(str, edge))
        output_file.write(edge_as_string + "\n")
