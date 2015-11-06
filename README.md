# MHS generation algorithms
This repository contains a collection of implemented algorithms for solving the Minimal Hitting Set generation problem.
We provide easy-to-use [AlgoRun][] containers for these algorithms, so you don't have to worry about compiling anything.

## The minimal hitting set generation problem
Consider a family *H* of sets *E₁*, *E₂*, … *Eₙ*.
A **hitting set** of *H* is a set *S* with the property that *S* intersects every one of the *E*s.
A **minimal hitting set** is a hitting set which cannot be made smaller without losing this property.
The **minimal hitting set generation problem** is to compute all the minimal hitting sets of the given family *H*.

For example, suppose that *H* is [[1, 2, 5], [2, 3, 4], [1, 3]].
Then [1, 2, 3] is a hitting set, because every set in *H* has at least one of these elements.
However, it is not a *minimal* hitting set, because we could remove either 2 or 3!
[1, 2] and [1, 3] are both minimal hitting sets of *H*.

Minimal hitting sets are important in a wide array of applications, ranging from very abstract mathematics (Boolean algebra and hypergraph theory) to high-tech applications (computational biology and data mining).
Hitting sets have even been used to prove that [there is no 16-clue sudoku](//dx.doi.org/10.1080/10586458.2013.870056)!
As a result, many algorithms have been developed to generate minimal hitting sets.

## The containers
However, researchers in different domains are not always aware of the large literature on the subject, and so highly-performant algorithms from one domain may not be used by researchers in another.
We have assembled a collection of software implementations of seventeen algorithms which reflect the long history and remarkable breadth of research in this area.
You can read more about the algorithms in the [containers `README`](containers/).

Each of these software packages is wrapped in a [Docker][] container based on the [AlgoRun][] framework.
You can easily download and run these containers yourself!
For more information about how to do this (either to test the algorithms on your own inputs or to integrate the algorithms into your research), consult the [`HOWTO`][HOWTO.md].

### Benchmark inputs
A variety of example inputs are provided in the [`benchmark_inputs`](benchmark_inputs/) folder.
Check the `README.md` file in each subdirectory for more information about the different data sets.

### Benchmark results
We have performed a number of performance benchmarks on all of the algorithms using the provided inputs.
The results are included in the [`results`](results/) directory.
For further information, see our [paper][].

## License
Code in this repository is distributed under a mixture of licenses.
See each file for details.
(In particular, code written by AGD for this project is typically released under the GPL 3.0, while upstream algorithm code is distributed under a variety of licenses.)

## Contact
This repository is a project of the [Vera-Licona Research Group][compsysmed] was assembled by [Andrew Gainer-Dewar][agdphd].

[docker]: //docker.io "Docker"
[algorun]: //algorun.org "Algorun"
[jsonschema]: //pypi.python.org/pypi/jsonschema "Python-jsonschema"
[compsysmed]: //compsysmed.org
[agdphd]: //github.com/agdphd
[paper]: TODO
