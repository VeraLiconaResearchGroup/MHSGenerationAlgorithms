# MHS algorithm implementation specification
Each implementation MUST provide an executable file `mhs` in its `src` directory.
This executable MUST be runnable in the Linux container environment; any required interpreter MUST be built into the container.
`mhs` MUST accept arguments in the following format:
    mhs input_file [FLAGS] [-f output_file]

## Input file
An input file represents a collection of sets whose minimal hitting sets are to be calculated.
(Equivalently, the file represents the edges of a hypergraph whose transversal hypergraph is to be constructed.)
`mhs` MUST accept input files which are valid JSON and contain the following key/value pairs:

* `edges`: a nonempty array of nonempty arrays of positive integers.
  Each of these arrays represents a set to be hit or an edge of the hypergraph.
  Each integer in a set is an *index*.
  The indices MAY appear in any order within a set, and the sets MAY appear in any order.

The input file MUST NOT contain pairs with the keys `guaranteedComplete`, `guaranteedMinimal`, or `transversals`.
`mhs` SHOULD assume that the vertex set is 1, 2, …, n where n is the largest index appearing in the file.
`mhs` MAY accept other keys; each such key SHOULD be documented appropriately in the `README.md` file.
`mhs` MUST ignore all unknown keys.

This input specification is formally encoded in the file `input_schema.json`.

### Example input file
Consider the hypergraph with vertices {1, 2, 3, 4, 5} and edges {1, 2, 4}, {2, 3, 5}, and {2, 4}.
The following input file represents this hypergraph in the specified format:

    {
        "someKey": "ignored value",
        "sets": [
                [1, 2, 4],
                [3, 2, 5],
                [2, 4],
                ]
    }
                

## Output file
An output file represents a collection of hitting sets of the input collection.
`mhs` MUST produce output files which are valid JSON.
The output file SHOULD contain all data from the input.
In addition, it MUST contain the following key/value pairs:

* `guaranteedMinimal`: a boolean, `true` if every hitting set in the file is guaranteed to be minimal and `false` otherwise.
* `guaranteedComplete`: a boolean, `true` if the collection of hitting sets is guaranteed to contain all minimal hitting sets and `false` otherwise.
* `transversals`: a nonempty array of nonempty arrays of positive integers.
  Each of these arrays represents a hitting set of the input collection or a transversal of the input hypergraph.
  Each integer in the set is an *index*, matching the corresponding index in the input sets.
  The indices SHOULD appear in increasing order in each hitting set, but the hitting sets MAY appear in any order in the collection.

`mhs` MAY produce additional keys, but SHOULD document these additional keys in `README.md` and SHOULD NOT overwrite keys with the same name from the input.
`mhs` MUST write this output to the filename specified with the `-f` option.
If the `-f` option is not given, `mhs` MUST write this output to `out.dat`.

### Example output file
Consider the hypergraph from the input file example above.
Some minimal transversals of this hypergraph are {2}, {3, 4}, and {4, 5}.
The following output file represents this incomplete set of minimal hitting sets in the specified format:

    {
        "someKey": "ignored value",
        "sets": [
                [1, 2, 4],
                [3, 2, 5],
                [2, 4],
                ],
        "guaranteedMinimal": true,
        "guaranteedComplete": false,
        "transversals": [
                        [2],
                        [3, 4],
                        [4, 5],
                        ]
    }

This output specification is formally encoded in the file `output_schema.json`.

## Flags
### Time flag
`mhs` MUST accept the `-t` flag.
If the `-t` flag is given, `mhs` MUST print to stdout the running time of its algorithm on the input data set in [hours]:minutes:seconds.subseconds format.
The time printed SHOULD represent the running time of the algorithm itself, omitting any time spent on input and output conversion.

### Other flags
`mhs` MAY accept any other flags that are appropriate to the algorithm implemented.
Each of these flags SHOULD be documented in the `README.md` file.
