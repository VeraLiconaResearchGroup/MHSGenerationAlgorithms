# BMR algorithm
This directory contains sources for the [`compsysmed/bmr`](//hub.docker.com/r/compsysmed/bmr) container.
It implements the BMR algorithm for the minimum hitting set generation problem.

For details of the algorithm, see [_A fast algorithm for computing hypergraph transversals and its application in mining emerging patterns_](//doi.org/10.1109/ICDM.2003.1250958).

## Implementation
The implementation provided in [src/alg](src/alg) is C code written by and copyright of Keisuke Murakami and distributed at the [Hypergraph Dualization Repository](//research.nii.ac.jp/~uno/dualization.html).
It is redistributed here by permission from the authors; see [src/alg/LICENSE.md](src/alg/LICENSE.md) for details.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/bmr:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/bmr:latest
