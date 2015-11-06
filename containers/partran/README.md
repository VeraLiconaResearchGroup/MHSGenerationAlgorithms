# ParTran
This directory contains an Algorun container implementing the ParTran algorithm for minimal hitting set enumeration.
For details of the algorithm, see [_Parallel computation of the minimal elements of a poset_](//doi.org/10.1145/1837210.1837221) by Leiserson et al.

## Implementation
The implementation provided in [src/alg](src/alg) is Cilk++ code written by Leiserson et al. and distributed at the BPASlib project's [webpage](//bpaslib.org).
TODO: update with license information.
***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.
