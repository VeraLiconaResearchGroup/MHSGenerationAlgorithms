# MHS2

MHS2 [[1]](#ref1) is a heuristic-based approximation algorithm for solving the
minimal hitting set/set cover problem.

The minimal hitting set problem can be formulated as follows:

Given a set of elements `U = {1, 2, ..., M}` (called the universe)
and a collection `S` of `N` non-empty sets whose union is equal to the
universe, find all sets `d` such that:

- The intersection of `d` with any element of `S` is not empty.
- The intersection of any proper subset of `d` with some element of
  `S` is empty.


## Introduction

This algorithm was primarily designed for solving the candidate
generation problem in scope of spectrum-based reasoning to fault
localization (SFL) [[2]](#ref2).

### Terminology
* SFL approaches work by abstracting the run-time behavior of the
  system under analysis in terms of two general concepts:
  **components** and **transactions**.
  * A component is an element of the system that, for diagnostic
    purposes, is considered to be atomic (e.g., a function, a class, a
    service, etc.), whereas a transaction is a set of component
    activations that:
    1. Share a common goal.
    2. The correctness of the output can be verified (e.g., a unit-test).
  * A failed transaction (or more precisely the components involved in
    a failed transaction) represents a **conflict**.
  * A conflict is a set of components that cannot be simultaneously
    healthy to explain the observed erroneous behavior.
* The input for our algorithm is called **spectra**. This data
  structure is composed of two elements: **activity matrix** and
  **error vector**.
  * The activity matrix encodes the involvement of system components
    in each transaction.
  * The error vector encodes the outcome of each transaction
    in terms of pass/fail.
    related.
* A **diagnostic candidate** (referred to simply as candidate) is a
  set of components such that its intersection with every conflict set
  is not empty.
* A **minimal candidate** is a candidate not containing any
  candidate of smaller cardinality (i.e., a proper subset).

Consider a system with 3 components: `c1`, `c2`, and `c3`. Also, consider that
3 transactions were observed for this system:

1. Involving `c1` and `c2`. Outcome: `fail`
2. Involving `c1` and `c3`. Outcome: `fail`
3. Involving `c1`. Outcome: pass

For this particular example, two conflicts exist: `{c1,c2}` and `{c1,c3}`.
For such conflicts 5 candidates exist: `{c1}`, `{c1,c3}`, `{c2,c3}`, `{c1,c2}`, `{c1,c2,c3}`.
From the 5 candidates only two are minimal: `{c1}`, `{c2,c3}`.

### Application to other domains

Even though designed for SFL applications, this algorithm is also well suited
to other domains which require the calculation of minimal hitting sets.

In practice, the algorithm calculates the minimal candidates for the
conflict collection encoded in the spectra.
Mapping the terminology presented above to the problem statement presented on top:

- The set of all components is equivalent to the universe `U`.
- The set of all conflicts (i.e., the failing transactions) is
  equivalent to the collection `S`.
- A minimal candidate is in fact a minimal hitting set for `(U,S)`.

In order to apply implementation to arbitrary minimal hitting set
problems, the elements of `S` must be encoded as failing transactions.

### More information

The minimal hitting set algorithm MHS2 was first described in [[1]](#ref1), and it is an improvement of the algorithm described in [[3]](#ref3).

We are currently advancing its capabilities further, and we expect to report the new changes in a journal paper soon.

### References
<a id="ref1"></a>[[1]](http://dx-2014.ist.tugraz.at/papers/DX14_Mon_PM_S1_paper1.pdf)
```
Nuno Cardoso and Rui Abreu. "An Efficient Distributed Algorithm for Computing Minimal Hitting Sets."
In Proceedings of the 25th International Workshop on Principles of Diagnosis (DX'14).
Graz, Austria, 2014. (Best Paper Award)
```

<a id="ref1"></a>[[2]](http://link.springer.com/chapter/10.1007/978-3-642-39955-8_3)
```
Nuno Cardoso and Rui Abreu. "MHS2: A Map-Reduce Heuristic-Driven Minimal Hitting Set Search Algorithm."
In Multicore Software Engineering, Performance, and Tools (MUSEPAT'13).
Springer Berlin Heidelberg, 2013. 25-36.
```

<a id="ref2"></a>[[3]](http://ieeexplore.ieee.org/?arnumber=5431781)
```
Rui Abreu, Peter Zoeteweij, and Arjan JC Van Gemund. "Spectrum-based multiple fault localization."
In Proceedings of the 24th IEEE/ACM International Conference on Automated Software Engineering (ASE'09).
IEEE Computer Society, 2009.
```

<a id="ref3"></a>[[4]](http://aaai.org/ocs/index.php/SARA/SARA09/paper/view/834)
```
Rui Abreu and Arjan JC van Gemund. "A Low-Cost Approximate Minimal Hitting Set Algorithm and its Application to Model-Based Diagnosis."
In Proceedings of the 8th Symposium on Abstraction, Reformulation and Approximation (SARA'09).
AAAI Press, 2009.
```

## Building

The dependencies for this project are:

* [g++](http://gcc.gnu.org/)/[clang++](http://clang.llvm.org/) with c++11 enabled
* [boost](http://www.boost.org/)
* [scons](http://www.scons.org/)

To build just type `scons` and all should be fine.

In order to build with boost in a non-default place, create a
`.scons.conf` file in the project root defining the following
variables:

* `LIBPATH`: path to `lib` directory (e.g., `LIBPATH="/usr/local/lib"`)
* `CPPPATH`: path to `include` directory (e.g., `CPPPATH="/usr/local/include"`)

In order to build with unit tests and assertions, create a
`.scons.conf` file in the project root defining `debug=True`.
Use `run_tests.sh` to run the unit tests.

## Running

```
Usage: ./build/mhs2 [options]
  -i, --input			Defines input file
  -o, --output			Defines output file
  -h, --help			Shows help text
  -v, --verbose			Enables verbose output
  -p, --print-spectra		Prints the spectra read from input
  -P, --candidate-printer	Selects a candidate printer (normal, pretty, latex)
  -a, --ambiguity		Turns on ambiguity group removal
  -c, --conflict		Turns on conflict ambiguity removal
  -t, --time			Sets the time-based cutoff value (seconds)
  -D, --candidates		Sets the candidate collection size cutoff value
  -d, --cardinality		Sets the candidate cardinality cutoff value
  -l, --lambda			Sets the lambda cutoff value
  -s, --similarity		Sets heuristic (ochiai, jaccard, tarantula, random)
  -L, --fork-level		Sets the forking level
  -T, --threads			Sets the number of threads
```

### Input Format

A spectra is represented in the following format:

```
<M> <N>
<     > < >
<  A  > <e>
<     > < >
```

Where:

* `M`: number of components
* `N`: number of transactions
* `A`: binary activity matrix (`1` -> `active`, `0` -> `inactive`)
* `e`: error vector (`x`/`1` -> `fail`, `.`/`0` -> `pass`)

Consider a system with 3 components. Also, consider that 2
transactions were observed for this system:

1. Involving `c1` and `c2`. Outcome: `fail`
2. Involving `c1` and `c3`. Outcome: `pass`

The corresponding spectra is as follows:

```
3 2
1 1 0 x
1 0 1 .
```

### Output Format

A candidate is represented by a set of positive numbers (the 1-based
indexes of its components) followed by one `0`.
The list of candidates is followed by two `0`.

The set of candidates `{{c1}, {c2,c3}, {c4,c5,c6}}` would be
represented as follows:

```
1 0
2 3 0
4 5 6 0
0 0
```

### Ambiguity Group Removal

An optimization featured in this implementation is the removal of
ambiguity groups (`-a`).  Ambiguity groups occur when two or more components
have equal activation pattern in failing transactions.

Take for instance the following spectra:

```
4 2
1 1 0 0 x
0 0 1 1 x
```

For this spectra `c1` and `c2` form an ambiguity group.
Also, `c3` and `c4` form another ambiguity group.

If the ambiguity groups are collapsed into a single column and the
elements of each group are available, it is possible to solve the same
problem much faster (provided that a lot of redundancy exists).

Returning to the example, running the algorithm with no ambiguity
removal yields 4 candidates.

```
$ echo "4 2 1 1 0 0 x 0 0 1 1 x" | ./build/mhs2
1 3 0
1 4 0
2 3 0
2 4 0
0 0
```

Running the algorithm with ambiguity removal only 1 candidate is
returned, however the remaining 3 candidates can be generated by
applying all possible substitutions encoded in the ambiguity group
information.

```
$ echo "4 2 1 1 0 0 x 0 0 1 1 x" | ./build/mhs2 -a
g(1) = s(2)
g(3) = s(4)

1 3 0
0 0
```

An ambiguity group is encoded as `g(grp_id) = s(a, b, ..., k)`, where
`grp_id` is the id of the first component in the group and `a, b, ...,
k` are the ids of the remaining components.

### Conflict Redundancy Removal

### Cutoffs

Being a NP-hard problem, trade-offs must be made in order to handle
large problems.

- `-t, --time` limits the execution time of the algorithm.
- `-D, --candidates` limits the number of computed candidates.
- `-d, --cardinality` limits the maximum size of the computed candidates.
- `-l, --lambda` sets the amount of search tree exploration (`0`: minimum, `1`: full exploration).

The default parameters should guarantee both soundness (i.e., all
candidates are minimal) and completeness (i.e., all minimal candidates
are found).
By using any of the cutoffs, the completeness guarantee
is lost.  All cutoffs except for `-d` will also break the soundness
guarantee.

## Benchmarking

In order to test the performance of this implementation, a
spectra generation tool is included.
This tool randomly generates spectra based on 4 parameters:

* `M`: number of components
* `N`: number of transactions
* `pa`: component activation probability
* `pe`: transaction failure probability

```
$ ./tools/generate.py 4 3 0.5 1
4 3
0 0 1 0 x
1 1 0 1 x
1 1 0 0 x
```

# Indentation

Use uncrustify to indent code:
- cd .git/hooks
- ln -s ../../tools/uncrustify/uncrustify-hook pre-commit

# License

Distributed under the GNU LESSER GENERAL PUBLIC LICENSE - see LICENSE.txt for further details.
