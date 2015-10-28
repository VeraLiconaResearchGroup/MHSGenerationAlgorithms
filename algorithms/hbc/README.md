# HBC algorithm
This directory contains an Algorun container implementing the HBC algorithm "MTMiner" for minimal hitting set enumeration.
For details of the algorithm, see [_A data mining formalization to improve hypergraph minimal transversal computation_](//www.infona.pl/resource/bwmeta1.element.baztech-article-BUS5-0014-0020) ([PDF](//cremilleux.users.greyc.fr/papers/FundInfoFinal07.pdf)) by Hébert, Bretto, and Crémilleux.

## Implementation
The implementation provided in [src/alg](src/alg) is compiled from C++ code written by Céline Hébert and available from the MTMiner project [website](//forge.greyc.fr/projects/kdariane/wiki/Mtminer).
It is provide as-is and without warranty.
***WARNING***: Testing has revealed that this sofwtare does not accurately generate all MHSes.
