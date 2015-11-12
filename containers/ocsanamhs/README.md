# OCSANA MHS
This directory contains sources for the [`compsysmed/ocsanamhs`](//hub.docker.com/r/compsysmed/ocsanamhs) container.
It implements the 'greedy' algorithm introduced in [OCSANA: optimal
combinations of interventions from network analysis,](//dx.doi.org/10.1093/bioinformatics/btt195) by Vera-Licona et al.

## Implementation
The implementation in [src/alg](src/alg) is Java code written by Misagh Kordi and copyright of the Vera-Licona Research Group.
***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/ocsanamhs:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/ocsanamhs:latest

## License
The code in this directory is available under the terms of the GPLv3 license.
