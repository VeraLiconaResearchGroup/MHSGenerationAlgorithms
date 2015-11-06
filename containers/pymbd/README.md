# PyMBD
This directory contains sources for the [`compsysmed/pymbd`](//hub.docker.com/r/compsysmed/pymbd) container.
It implements various algorithms for the minimum hitting set generation problem.
They include:
* `bool-iterative` from [The computation of hitting sets: review and new algorithms](//dx.doi.org/10.1016/S0020-0190(02)00506-9) by Lin and Jiang,
* `hsdag` from [A correction to the algorithm in Reiter's theory of diagnosis](//dx.doi.org/10.1016/0004-3702(89)90079-9) by Greiner, Smith, and Wilkerson,
* `hst` from [A variant of Reiter's hitting-set algorithm](//dx.doi.org/10.1016/S0020-0190(00)00166-6) by Wotawa, and
* `staccato` from "A low-cost approximate minimal hitting set algorithm and its application to model-based diagnosis" ([PDF](//haslab.uminho.pt/ruimaranhao/files/sara09.pdf)) by Abreu and Gemund.

## Implementation
The implementations provided in [src/alg](src/alg) are Python code written by Thomas Quaritsch and Ingo Pill for "PyMBD: a library of MBD algorithms and a light-weight evaluation platform" ([PDF](//dx-2014.ist.tugraz.at/papers/DX14_Tue_PM_tool_paper2.pdf).
The complete PyMBD distribution may be downloaded as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.
Portions of it are redistributed here under the terms of the author's license; see the [LICENSE](src/pymbd/LICENSE) file for details.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/pymbd:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/pymbd:latest
