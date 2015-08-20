# PyMBD
This directory contains an Algorun container which wraps the PyMBD suite of algorithms.
It includes implementations of the Boolean algorithms from [_The computation of hitting sets: review and new algorithms_](//dx.doi.org/10.1016/S0020-0190(02)00506-9) by Lin and Jiang, the HS-DAG algorithm from [_A correction to the algorithm in Reiter's theory of diagnosis_](//dx.doi.org/10.1016/0004-3702(89)90079-9) by Greiner, Smith, and Wilkerson, the HST algorithm from [_A variant of Reiter's hitting-set algorithm_](//dx.doi.org/10.1016/S0020-0190(00)00166-6) by Wotawa, and the STACCATO algorithm from _A low-cost approximate minimal hitting set algorithm and its application to model-based diagnosis_ ([PDF](//haslab.uminho.pt/ruimaranhao/files/sara09.pdf)) by Abreu and Gemund.

## Implementation
The implementation provided in [src/alg](src/alg) is Python code written by Thomas Quaritsch and Ingo Pill.
The complete PyMBD distribution may be downloaded as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.
Portions of it are redistributed here under the terms of the author's license; see the [LICENSE](src/pymbd/LICENSE) for details.
