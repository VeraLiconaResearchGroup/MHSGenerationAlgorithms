# AGDMHS
This directory contains sources for the [`compsysmed/agdmhs`](//hub.docker.com/r/compsysmed/agdmhs) container.
It implements various algorithms for the minimum hitting set generation problem.
They include:
* `pMMCS`: parallelized version of the MMCS algorithm.
Serial version introduced in [Efficient algorithms for dualizing large-scale hypergraphs][shd] by Murakami and Uno.
* `pRS`: parallelized version of the RS algorithm.
Serial version introduced in [Efficient algorithms for dualizing large-scale hypergraphs][shd] by Murakami and Uno.
* `fka`: the A algorithm from [On the complexity of dualization of monotone disjunctive normal forms][fk], Fredman, M. and Khachiyan, L.
* `berge`: sequential algorithm, widely published, i.e. in *Hypergraphs: combinatorics of finite sets*, C. Berge, 1984.
* `bm`: global parallel algorithm from [A fast and simple parallel algorithm for the monotone duality problem][bm] by E. Boros and K. Makino.

## Implementation
The implementations in [src/alg](src/alg) are C++ code written by Andrew Gainer-Dewar and copyright of the Vera-Licona Research Group.

**NOTE**: these implementations store hypergraphs as lists of [boost::dynamic_bitset][bitset], which take space in memory proportional to the largest index of a vertex.
To maximize efficiency, you should be sure that the vertices of your hypergraph are "packed" down to [1, 2, …, n].

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/agdmhs:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/agdmhs:latest

## License
Most of the code in this directory is available under the terms of the GPLv3 license.
However, [src/alg/include/concurrentqueue.h](src/alg/include/concurrentqueue.h) is taken from [moodycamel::ConcurrentQueue](//github.com/cameron314/concurrentqueue) under the terms of the BSD license.

[shd]: //doi.org/10.1016/j.dam.2014.01.012
[fk]: //doi.org/10.1006/jagm.1996.0062
[bm]: //doi.org/10.1007/978-3-642-02927-1_17
[bitset]: //www.boost.org/doc/libs/1_59_0/libs/dynamic_bitset/dynamic_bitset.html
