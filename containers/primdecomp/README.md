# Primary decomposition
This directory contains sources for the [`compsysmed/primdecomp`](//hub.docker.com/r/compsysmed/primdecomp) container.
It implements an algorithm for MHS generation based on computational algebra in [Macaulay2][].
Specifically, we construct a monomial ideal with one generator for each edge of the input hypergraph, then compute its associated primes, which correspond to MHSes.

For example, suppose our input hypergraph is H = [[1, 2, 5], [2, 3, 4], [1, 3]].
In Macaulay2, we first construct the monomial ring:

    > k = QQ[x1, x2, x3, x4, x5]

We then construct the ideal representing H:

    > I = monomialIdeal(x1*x2*x5, x2*x3*x4, x1*x3)

Finally, we compute the associated primes, which correspond to the MHSes [[1, 2], [1, 3], [1, 4], [2, 3], [3, 5]].

    > P = associatedPrimes I
    {monomialIdeal (x1, x2), monomialIdeal (x1, x3), monomialIdeal (x1, x4), monomialIdeal (x2, x3), monomialIdeal (x3, x5)}


## Implementation
The [src/mhs](src/mhs) script, which generates a Macaulay 2 program for an input hypergraph, is Python code written by Andrew Gainer-Dewar and copyright of the Vera-Licona Research Group.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/primdecomp:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/primdecomp:latest

## License
The code in this directory is available under the terms of the GPLv3 license.

[macaulay2]: //www.math.uiuc.edu/Macaulay2/
