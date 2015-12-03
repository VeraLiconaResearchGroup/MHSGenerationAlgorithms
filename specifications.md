# MHS algorithm implementation specification
Each implementation MUST provide an executable file `mhs` in its `src` directory.
This executable MUST be runnable in the Linux container environment; any required interpreter MUST be built into the container.
`mhs` MUST accept arguments in the following format:

    mhs input_file [-f output_file]

Any other configuration SHOULD be handled using environment variables.
Each such environment variable SHOULD be documented appropriately in the `README.md` file.

## Input file
An input file represents a collection of sets whose minimal hitting sets are to be calculated.
(Equivalently, the file represents the edges of a hypergraph whose transversal hypergraph is to be constructed.)
`mhs` MUST accept input files which are valid JSON and contain the following key/value pairs:

* `sets`: a nonempty array of nonempty arrays of positive integers.
  Each of these arrays represents a set to be hit or an edge of the hypergraph.
  Each integer in a set is an *index*.
  The indices MAY appear in any order within a set, and the sets MAY appear in any order.
  The hypergraph SHOULD be simple, which is to say that no edge should be a subset of any other edge.

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
                [1, 2, 5],
                [3, 2, 4],
                [1, 3]
                ]
    }


## Output file
An output file represents a collection of hitting sets of the input collection.
`mhs` MUST produce output files which are valid JSON.
The output file SHOULD contain all data from the input.
In addition, it MUST contain the following key/value pairs:

* `timeTaken'`: a (positive) number giving the execution time of the algorithm.
  This value SHOULD represent the running time of the algorithm itself, omitting any time spend on input and output conversion.
* `transversals`: a nonempty array of nonempty arrays of positive integers.
  Each of these arrays represents a hitting set of the input collection or a transversal of the input hypergraph.
  Each integer in the set is an *index*, matching the corresponding index in the input sets.
  The indices SHOULD appear in increasing order in each hitting set, but the hitting sets MAY appear in any order in the collection.

`mhs` MAY produce additional keys, but SHOULD document these additional keys in `README.md` and SHOULD NOT overwrite keys with the same name from the input.
`mhs` MUST write this output to the filename specified with the `-f` option.
If the `-f` option is not given, `mhs` MUST write this output to `out.dat`.

### Example output file
Consider the hypergraph from the input file example above.
Some minimal transversals of this hypergraph are {2} and {3, 4}.
The following output file represents this incomplete set of minimal hitting sets in the specified format:

    {
        "someKey": "ignored value",
        "sets": [
                [1, 2, 5],
                [3, 2, 4],
                [2, 4]
                ],
        "guaranteedMinimal": true,
        "guaranteedComplete": false,
        "transversals": [
                        [1, 3],
                        [3, 5]
                        ]
    }

This output specification is formally encoded in the file `output_schema.json`.
