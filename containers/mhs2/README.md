# MHS²
This directory contains sources for the [`compsysmed/mhs2`](//hub.docker.com/r/compsysmed/mhs2) container.
It implements the MHS² algorithm for the minimum hitting set generation problem.

For details of the algorithm, see _An efficient distributed algorithm for computing minimal hitting sets_ ([PDF](//dx-2014.ist.tugraz.at/papers/DX14_Mon_PM_S1_paper1.pdf)) by Cardoso and Abreu.
Implementation of the [MHS2](https://github.com/npcardoso/MHS2) parallel approximating MHS algorithm.

## Implementation
The implementation provided in [src/alg](src/alg) is C++ code written by Nunu Cardoso and distributed at the project's [Github repository](//github.com/npcardoso/MHS2).
It is distributed here without modification under the terms of the GPL3; see the [README](src/alg/README.md) for details.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/mhs2:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/mhs2:latest
