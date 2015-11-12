# HBC algorithm
This directory contains sources for the [`compsysmed/hbc`](//hub.docker.com/r/compsysmed/hbc) container.
It implements the HBC algorithm for the minimum hitting set generation problem.

For details of the algorithm, see [_A data mining formalization to improve hypergraph minimal transversal computation_](//www.infona.pl/resource/bwmeta1.element.baztech-article-BUS5-0014-0020) ([PDF](//cremilleux.users.greyc.fr/papers/FundInfoFinal07.pdf)) by Hébert, Bretto, and Crémilleux.

## Implementation
The implementation provided in [src/alg](src/alg) is compiled from C++ code written by Céline Hébert and available from the MTMiner project [website](//forge.greyc.fr/projects/kdariane/wiki/Mtminer).
It is provide as-is and without warranty.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/hbc:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/hbc:latest
