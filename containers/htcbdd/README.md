# HTC-BDD
This directory contains sources for the [`compsysmed/htcbdd`](//hub.docker.com/r/compsysmed/htcbdd) container.
It implements two algorithms for the minimal hitting set generation problem based on (Zero-reduced) Binary Decision Diagrams (ZDDs and BDDs).
They are:
* `KNUTH`: ZDD-based algorithm given in solution to exercise 237 of §7.1.4 of Volume 4a of *The Art of Computer Programming* (1st ed.) by Donald E. Knuth.
* `HTCBDD`: improved version of `KNUTH` using both BDDs and ZDDs.
Introduced in [Hypergraph transversal computation with binary decision diagrams](//doi.org/10.1007/978-3-642-38527-8_10) by Toda.

## Implementation
The implementations in [src/alg](src/alg) are C code written by and copyright of Takahisa Toda.
They are available to download from Toda's [website](//www.sd.is.uec.ac.jp/toda/htcbdd.html) and are included here under the terms of the MIT license.

The code in [src/alg/cudd-2.5.0](src/alg/cudd-2.5.0) is version 2.5.0 of the [CUDD library](http://vlsi.colorado.edu/~fabio/CUDD/) by Fabio Somenzi.
It is included here under the terms of the author's license, included in full at [src/alg/cudd-2.5.0/LICENSE](src/alg/cudd-2.5.0/LICENSE).

**NOTE**: these implementations store hypergraphs as lists of [boost::dynamic_bitset][bitset], which take space in memory proportional to the largest index of a vertex.
To maximize efficiency, you should be sure that the vertices of your hypergraph are "packed" down to [1, 2, …, n].

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/htcbdd:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/htcbdd:latest

## License
Most of the code in this directory is available under the terms of the GPLv3 license.
However, [src/alg/include/concurrentqueue.h](src/alg/include/concurrentqueue.h) is taken from [moodycamel::ConcurrentQueue](//github.com/cameron314/concurrentqueue) under the terms of the BSD license.

[shd]: //doi.org/10.1016/j.dam.2014.01.012
[fk]: //doi.org/10.1006/jagm.1996.0062
[bm]: //doi.org/10.1007/978-3-642-02927-1_17
[bitset]: //www.boost.org/doc/libs/1_59_0/libs/dynamic_bitset/dynamic_bitset.html
