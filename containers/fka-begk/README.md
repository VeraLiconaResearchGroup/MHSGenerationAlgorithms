# FK-A (BEGK) algorithm
This directory contains sources for the [`compsysmed/fka-begk`](//hub.docker.com/r/compsysmed/fka-begk) container.
It implements the FK-A (BEGK) algorithm for the minimum hitting set generation problem.

For details of the algorithm, see [_On the complexity of dualization of monotone disjunctive normal forms_](//doi.org/10.1006/jagm.1996.0062) by Fredman and Khachiyan and [_An efficient implementation of a quasi-polynomial algorithm for generating hypergraph transversals and its application in joint generation_](//doi.org/10.1016/j.dam.2006.04.012) by Khachiyan et al.

## Implementation
The implementation provided in [src/alg](src/alg) is compiled from C code written by K. Elbassioni and distributed at the website of [Endre Boros](//rutcor.rutgers.edu/~boros/IDM/DualizationCode.html).
It is included here by permission of the author and is provided as-is and without warranty.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/fka-begk:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/fka-begk:latest
