# MHS generation algorithms
C++ implementations of several algorithms for minimal hitting set generation.
These implementations can be run from a standalone binary and are suitable for inclusion in other projects as a library (see [HOWTO.library.md](HOWTO.library.md)).

Benchmarks of these and many other algorithms for MHS generation are available from our [benchmark repository][benchmark] and [paper][].

This implementation is available in an [AlgoRun][] container, which can be downloaded to your computer with `docker pull compsysmed/agdmhs` or run [online][agdmhs-algorun]

## The minimal hitting set generation problem
Consider a family *H* of sets *E₁*, *E₂*, … *Eₙ*.
A **hitting set** of *H* is a set *S* with the property that *S* intersects every one of the *E*s.
A **minimal hitting set** is a hitting set which cannot be made smaller without losing this property.
The **minimal hitting set generation problem** is to compute all the minimal hitting sets of the given family *H*.

For example, suppose that *H* is [[1, 2, 5], [2, 3, 4], [1, 3]].
Then [1, 2, 3] is a hitting set, because every set in *H* has at least one of these elements.
However, it is not a *minimal* hitting set, because we could remove either 2 or 3!
[1, 2] and [1, 3] are both minimal hitting sets of *H*.

Minimal hitting sets are important in a wide array of applications, ranging from very abstract mathematics (Boolean algebra and hypergraph theory) to high-tech applications (computational biology and data mining).
Hitting sets have even been used to prove that [there is no 16-clue sudoku](//dx.doi.org/10.1080/10586458.2013.870056)!
As a result, many algorithms have been developed to generate minimal hitting sets.
We provide implementations of five of these algorithms, including two with state-of-the-art performance.

## The algorithms
We provide implementations of five algorithms for MHS generation.
We describe each below.
Some of the algorithms have the option to generate only MHSes up to a certain size; we refer to this as "cutoff" enumeration.
Some of the algorithms also support multiple processors.

### pMMCS
*(Supports cutoff and multithreading)*
Highly performant algorithm published in [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) in 2014 by Murakami and Uno.
We provide a parallelized implementation.

### pRS
*(Supports cutoff and multithreading)*
Highly performant algorithm published in [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) in 2014 by Murakami and Uno.
We provide a parallelized implementation.

### Berge
*(Supports cutoff)*
A sequential algorithm based on an algebraic decomposition of the input hypergraph.
First published in 1984 by Berge in _Hypergraphs: combinatorics of finite sets_.
(We find that this algorithm is not competitive with `pMMCS` and `pRS` on practical inputs.
This algorithm is provided for testing purposes only.)

### FK-A
The A algorithm from [_On the complexity of dualization of monotone disjunctive normal forms_](//doi.org/10.1006/jagm.1996.0062), Fredman, M. and Khachiyan, L, as improved in [_An efficient implementation of a quasi-polynomial algorithm for generating hypergraph transversals and its application in joint generation_](//doi.org/10.1.1.85.6762), E. Boros et al.
Has the best known asymptotic bounds on runtime of any serial algorithm with a public implementation.
(However, we find that it is not very efficient on practical inputs.
This algorithm is provided for testing purposes only.)

### BM
*(Supports multithreading)*
A "global parallel" algorithm based on a full cover decomposition of the input hypergraph.
Published in [_A fast and simple parallel algorithm for the monotone duality problem_](//doi.org/10.1007/978-3-642-02927-1_17) by E. Boros and K. Makino.
Has the best known asymptotic bounds on runtime of any parallel algorithm.
(However, we find that it is not very efficient on practical inputs.
This algorithm is provided for testing purposes only.)

## Compiling the code
This project depends on several libraries from the [Boost][] project.
To install the required dependencies on a Debian system, run

    sudo apt-get install libboost-program-options-dev libboost-log-develop

It also uses the [`moodycamel::ConcurrentQueue`][concurrentqueue] library, which is included here under the terms of the author's "Simplified BSD license".

To download the code to your own computer, you should clone it using Git.
To do this, run the following in your terminal:

    git clone https://github.com/VeraLiconaResearchGroup/Minimal-Hitting-Set-Algorithms.git

This will download the code and extract it into a directory called `Minimal-Hitting-Set-Algorithms`.
You can then compile the program by running `make` in that directory.
(If you have multiple cores or processors, you can build in parallel by running `make -j` instead.)

To update the software, run `git pull` from the `Minimal-Hitting-Set-Algorithms` directory, then run `make` again.

## Running the code
Once you have compiled the software, you're ready to generate minimal hitting sets.
First, you need an input file representing your hypergraph.
The format is simple and matches that used by the [Hypergraph Dualization Repository][shd].
For each edge of your hypergraph, your input file should contain a single line which gives the vertices as positive integers separated by spaces.
For example, the hypergraph [[1, 2, 5], [2, 3, 4], [1, 3]] is represented by the following input file:

    1 2 5
    2 3 4
    1 3

We provide this example in [`example-input.dat`][example-input.dat].
To generate the MHSes of this hypergraph and store them in `out.dat`, run the following:

    ./agdmhs example-input.dat out.dat -a pmmcs

You can replace `pmmcs` with `prs`, `berge`, `fka`, or `bm` to run a different algorithm.

### Cutoff enumeration
If you are only interested in hitting sets up to a certain size *k*, you can obtain them more efficiently.
Add the option `-c *k*` to the command above to run in this mode.
(This applies to `pmmcs`, `prs`, and `berge` only.)

### Multithreaded enumeration
If your computer has multiple cores or processors, you can run in multithreaded mode to speed things up.
To use *t* threads, add the option `-t *t*` to the command above.
(This applies to `pmmcs`, `prs`, and `bm` only.)

## License
This project uses the [`moodycamel::ConcurrentQueue`][concurrentqueue] implementation by Cameron Desrochers.
It is included here (as [include/concurrentqueue.h](include/concurrentqueue.h)) under the terms of the author's license.

This project uses the [Catch][catch] unit testing framework.
It is included here (as [include/catch.hpp](include/catch.hpp)) under the terms of the author's license.

All other code is provided by the author under the terms of the GPLv3 license.

[agdmhs-algorun]: http://agdmhs.algorun.org
[algorun]: http://algorun.org
[benchmark]: http://github.com/VeraLiconaResearchGroup/MHSGenerationAlgorithms
[boost]: http://www.boost.org
[catch]: http://www.github.com/philsquared/Catch
[concurrentqueue]: http://github.com/cameron314/concurrentqueue
[paper]: http://arxiv.org/abs/1601.02939
[shd]: http://research.nii.ac.jp/~uno/code/shd.html
