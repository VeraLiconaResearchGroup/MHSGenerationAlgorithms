# KS algorithm
This directory contains sources for the [`compsysmed/ks`](//hub.docker.com/r/compsysmed/ks) container.
It implements the KS algorithm for the minimum hitting set generation problem.

For details of the algorithm, see [_An efficient algorithm for the transversal hypergraph generation_](//doi.org/10.7155/jgaa.00107) by Kavvadias and Stavropoulos.

## Implementation
The implementation provided in [src/alg](src/alg) is compiled from Pascal code written by Dimitris J. Kavvadias and Elias C. Stavropoulos.
The source is distributed at the [Hypergraph Dualization Repository](//research.nii.ac.jp/~uno/dualization.html) and the [author's page](//lca.ceid.upatras.gr/~estavrop/transversal/).
It is provided as-is and without any warranty.
***WARNING***: Testing has revealed that this sofwtare does not accurately generate all MHSes.
