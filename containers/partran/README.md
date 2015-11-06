# ParTran
This directory contains sources for the [`compsysmed/partran`](//hub.docker.com/r/compsysmed/partran) container.
It implements the ParTran algorithm for the minimum hitting set generation problem.

For details of the algorithm, see [_Parallel computation of the minimal elements of a poset_](//doi.org/10.1145/1837210.1837221) by Leiserson et al.

## Implementation
The implementation provided in [src/alg](src/alg) is Cilk++ code written by Leiserson et al. and distributed at the BPASlib project's [webpage](//bpaslib.org).
TODO: update with license information.
***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/partran:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/partran:latest
