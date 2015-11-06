# DL algorithm
This directory contains sources for the [`compsysmed/dl`](//hub.docker.com/r/compsysmed/dl) container.
It implements the DL algorithm for the minimum hitting set generation problem.

For details of the algorithm, see [_Mining border descriptions of emerging patterns from dataset pairs_](//doi.org/10.1007/s10115-004-0178-1) by Dong and Li.

## Implementation
The implementation provided in [src/alg](src/alg) is C code written by Keisuke Murakami and distributed at the [Hypergraph Dualization Repository](//research.nii.ac.jp/~uno/dualization.html).
It is redistributed here by permission from the authors; see [src/alg/LICENSE.md](src/alg/LICENSE.md) for details.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/dl:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/dl:latest
