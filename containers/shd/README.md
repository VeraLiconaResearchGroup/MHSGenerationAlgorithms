# SHD algorithm
This directory contains sources for the [`compsysmed/shd`](//hub.docker.com/r/compsysmed/shd) container.
It implements the SHD algorithms (MMCS and RS) for the minimum hitting set generation problem.

For details of the algorithms, see [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) by Murakami and Uno.

## Implementation
The implementation provided in [src/alg](src/alg) is C code written by [Takeaki Uno]<uno@nii.jp> and distributed at his [website](//research.nii.ac.jp/~uno/code/shd.html), where the latest version is always available.
It is redistributed here without modification under the terms of the notice at the top of that page:

> This program is available for only academic use, basically.
> Anyone can modify this program, but he/she has to write down the change of the modification on the top of the source code.
> Neither contact nor appointment to Takeaki Uno is needed.
> If one wants to re-distribute this code, do not forget to refer the newest code, and show the link to homepage of Takeaki Uno, to notify the news about SHD for the users.
> For the commercial use, please make a contact to Takeaki Uno.

## Building
To build the container yourself, run the following from this directory:

    docker build -t compsysmed/shd:latest .

**You do not need to build the container yourself to use the algorithms.**
You can fetch a prebuilt copy of the container by running the following:

    docker pull compsysmed/shd:latest
