# Algorithms in the repository
The repository includes a large collection of algorithm implementations.
We give a references and a few details for each here.
For full details, see our [paper][].

The listing below is sorted taxonomically, rather than according to which container provides each algorithm; information about the containers can be found in their respective directories.

Some of these algorithms support 'cutoff' enumeration—that is, searching for MHSes of size no larger than some specified bound.

## Edge iteration
### Berge
*(Supports cutoff)*
A sequential algorithm based on an algebraic decomposition of the input hypergraph.
First published in 1984 by Berge in _Hypergraphs: combinatorics of finite sets_.

An implementation in C++ by A. Gainer-Dewar is provided in the [`compsysmed/agdmhs`](//hub.docker.com/r/compsysmed/agdmhs) container in the [`agdmhs`](agdmhs/) directory.
You can run this container online on the [Algorun server](http://agdmhs.algorun.org).

### HS-DAG
*(Supports cutoff)*
A corrected version of Reiter's algorithm, published in [_A correction to the algorithm in Reiter's theory of diagnosis_](//dx.doi.org/10.1016/0004-3702(89)90079-9) by Greiner, Smith, and Wilkerson.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.

The implementation is provided in the [`compsysmed/pymbd`](//hub.docker.com/r/compsysmed/pymbd) container in the [`pymbd`](pymbd/) directory under the terms of the authors' license.
You can run this container online on the [Algorun server](http://pymbd.algorun.org).

### HST
*(Supports cutoff)*
A corrected version of Reiter's algorithm, published in [_A variant of Reiter's hitting-set algorithm_](//dx.doi.org/10.1016/S0020-0190(00)00166-6) by Wotawa.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.

The implementation is provided in the [`compsysmed/pymbd`](//hub.docker.com/r/compsysmed/pymbd) container in the [`pymbd`](pymbd/) directory under the terms of the authors' license.
You can run this container online on the [Algorun server](http://pymbd.algorun.org).

### BMR
An improved Berge's algorithm published in [_A fast algorithm for computing hypergraph transversals and its application in mining emerging patterns_](//doi.org/10.1109/ICDM.2003.1250958).

An implementation in C by K. Murakami is available from the [Hypergraph Dualization Repository][hdr] and is provided in the [`compsysmed/bmr`](//hub.docker.com/r/compsysmed/bmr) container in the [`bmr`](bmr/) directory by permission from the authors.
You can run this container online on the [Algorun server](http://bmr.algorun.org).

### DL
An improved Berge's algorithm published in [_Mining border descriptions of emerging patterns from dataset pairs_](//doi.org/10.1007/s10115-004-0178-1) by Dong and Li.

An implementation in C by K. Murakami is available from the [Hypergraph Dualization Repository][hdr] and is provided in the [`compsysmed/dl`](//hub.docker.com/r/compsysmed/dl) container in the [`dl`](dl/) directory by permission from the authors.
You can run this container online on the [Algorun server](http://dl.algorun.org).

### KS
A variation of Berge's algorithm which searches depth first and thus avoids the need to store large intermediate hypergraphs in memory.
Published in [_An efficient algorithm for the transversal hypergraph generation_](//doi.org/10.7155/jgaa.00107) by Kavvadias and Stavroupoulos.

A Pascal implementation by D. Kavvadias and E. Stavropoulos is available from the [Hypergraph Dualization Repository][hdr] and the [author's page](//lca.ceid.upatras.gr/~estavrop/transversal/) and is provided in the [`compsysmed/ks`](//hub.docker.com/r/compsysmed/ks) container in the [`ks`](ks/) directory by permission from the authors.
You can run this container online on the [Algorun server](http://ks.algorun.org).

## Divide and conquer
### FK-A
The A algorithm from [_On the complexity of dualization of monotone disjunctive normal forms_](//doi.org/10.1006/jagm.1996.0062), Fredman, M. and Khachiyan, L, as improved in [_An efficient implementation of a quasi-polynomial algorithm for generating hypergraph transversals and its application in joint generation_](//doi.org/10.1.1.85.6762), E. Boros et al.
Has the best known asymptotic bounds on runtime of any serial algorithm with a public implementation.

An implementation in compiled C is available from the [Hypergraph Dualization Repository][hdr] and is provided in the [`compsysmed/fka-begk`](//hub.docker.com/r/compsysmed/fka-begk) container in the [`fka-begk`](fka-begk/) directory by permission from the authors.
You can run this container online on the [Algorun server](http://fka-begk.algorun.org).

An implementation in C++ of the original, unmodified algorithm is provided in the [`compsysmed/agdmhs`](//hub.docker.com/r/compsysmed/agdmhs) container in the [`agdmhs`](agdmhs/) directory.
You can run this container online on the [Algorun server](http://agdmhs.algorun.org).

### BOOL
*(Supports cutoff)*
A divide-and-conquer algorithm introduced in [_The computation of hitting sets: review and new algorithms_](//dx.doi.org/10.1016/S0020-0190(02)00506-9) by Lin and Jiang.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.

The implementation is provided in the [`compsysmed/pymbd`](//hub.docker.com/r/compsysmed/pymbd) container in the [`pymbd`](pymbd/) directory under the terms of the authors' license.
You can run this container online on the [Algorun server](http://pymbd.algorun.org).

### STACCATO
*(Supports cutoff)*
A divide-and-conquer approximation algorithm, published in _A low-cost approximate minimal hitting set algorithm and its application to model-based diagnosis_ ([PDF](//haslab.uminho.pt/ruimaranhao/files/sara09.pdf)) by Abreu and Gemund.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.

The implementation is provided in the [`compsysmed/pymbd`](//hub.docker.com/r/compsysmed/pymbd) container in the [`pymbd`](pymbd/) directory under the terms of the authors' license.
You can run this container online on the [Algorun server](http://pymbd.algorun.org).

### ParTran
A parallel algorithm using a Berge-like calculation on divided subhypergraphs, published in [_Parallel computation of the minimal elements of a poset_](//doi.org/10.1145/1837210.1837221) by Leiserson et al.
An implementation in Cilk++ by those authors is available from the BPASlib project's [webpage](//bpaslib.org).

The implementation is provided in the [`compsysmed/partran`](//hub.docker.com/r/compsysmed/partran) container in the [`partran`](partran/) directory under the terms of the authors' license.
You can run this container online on the [Algorun server](http://partran.algorun.org).

### KNUTH
An algorithm based on ZDD representations of hypergraphs or boolean functions.
Introduced as solution to exercise 237 of §7.1.4 of Volume 4a of *The Art of Computer Programming* (1st ed.) by Donald E. Knuth.

An implementation in C by the author of [Hypergraph transversal computation with binary decision diagrams](//doi.org/10.1007/978-3-642-38527-8_10) is available from his [website](//www.sd.is.uec.ac.jp/toda/htcbdd.html) and is provided in the [`compsysmed/htcbdd`](//hub.docker.com/r/compsysmed/htcbdd) container under the terms of the GPLv3 license.
This container is not hosted on the AlgoRun server due to resource constraints.

### HTC-BDD
An improved version of `KNUTH` which uses both BDDs and ZDDs.
Introduced in [Hypergraph transversal computation with binary decision diagrams](//doi.org/10.1007/978-3-642-38527-8_10) by Takahesi Toda.

An implementation in C by Toda is available from his [website](//www.sd.is.uec.ac.jp/toda/htcbdd.html) and is provided in the [`compsysmed/htcbdd`](//hub.docker.com/r/compsysmed/htcbdd) container under the terms of the GPLv3 license.
This container is not hosted on the AlgoRun server due to resource constraints.

### MHS²
*(Supports cutoff)*
A parallel algorithm from _An efficient distributed algorithm for computing minimal hitting sets_ ([PDF](//dx-2014.ist.tugraz.at/papers/DX14_Mon_PM_S1_paper1.pdf)) by Cardoso and Abreu.

An implementation in C++ by those authors is available from their [GitHub repository](//github.com/npcardoso/MHS2) and is provided in the [`compsysmed/mhs2`](//hub.docker.com/r/compsysmed/mhs2) container in the [`mhs2`](mhs2/) directory under the terms of the GPLv2 license.
You can run this container online on the [Algorun server](http://mhs2.algorun.org).

## Transversal buildup
### HBC
*(Supports cutoff)*
A vertex-wise algorithm inspired by ideas from data mining and published in [_A data mining formalization to improve hypergraph minimal transversal computation_](//www.infona.pl/resource/bwmeta1.element.baztech-article-BUS5-0014-0020) ([PDF](//cremilleux.users.greyc.fr/papers/FundInfoFinal07.pdf)) by Hébert, Bretto, and Crémilleux.

An implementation in C++, written by C. Hébert and now maintained by F. Rioult, is available as [MtMiner](//forge.greyc.fr/projects/kdariane/wiki/Mtminer) and is provided in the [`compsysmed/hbc`](//hub.docker.com/r/compsysmed/hbc) container in the [`hbc`](hbc/) directory by permission from the authors.
You can run this container online on the [Algorun server](http://hbc.algorun.org).

### OCSANA-Greedy
*(Supports cutoff)*
A vertex-wise "greedy" algorithm published in [_OCSANA: optimal combinations of interventions from network analysis_](//doi.org/10.1093/bioinformatics/btt195).
An implementation in Java by M. Kordi, originally written for a refactoring of [OCSANA](//bioinfo-out.curie.fr/projects/ocsana/OCSANA.html), is provided in the [`compsysmed/ocsana`](//hub.docker.com/r/compsysmed/ocsana) container in the [`ocsana`](ocsana/) directory.

***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.
Accordingly, this container is not hosted on the AlgoRun server.

### MMCS
*(pMMCS supports cutoff)*
Highly performant algorithm published in [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) in 2014 by Murakami and Uno.

Those authors provide a C implementation at the [Hypergraph Dualization Repository][hdr], which is redistributed here in the in the [`compsysmed/shd`](//hub.docker.com/r/compsysmed/shd) container in the [`shd`](shd/) directory bypermission from the authors.
You can run this container online on the [Algorun server](http://shd.algorun.org).

An implementation in C++ which takes advantage of multiple cores using OpenMP is provided in the [`compsysmed/agdmhs`](//hub.docker.com/r/compsysmed/agdmhs) container in the [`agdmhs`](agdmhs/) directory.
You can run this container online on the [Algorun server](http://agdmhs.algorun.org).

### RS
*(pRS supports cutoff)*
Highly performant algorithm published in [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) in 2014 by Murakami and Uno.

Those authors provide a C implementation at the [Hypergraph Dualization Repository][hdr], which is redistributed here in the in the [`compsysmed/shd`](//hub.docker.com/r/compsysmed/shd) container in the [`shd`](shd/) directory bypermission from the authors.
You can run this container online on the [Algorun server](http://shd.algorun.org).

An implementation in C++ which takes advantage of multiple cores using OpenMP is provided in the [`compsysmed/agdmhs`](//hub.docker.com/r/compsysmed/agdmhs) container in the [`agdmhs`](agdmhs/) directory.
You can run this container online on the [Algorun server](http://agdmhs.algorun.org).

## Full cover
### BM
A "global parallel" algorithm based on a full cover decomposition of the input hypergraph.
Published in [_A fast and simple parallel algorithm for the monotone duality problem_](//doi.org/10.1007/978-3-642-02927-1_17) by E. Boros and K. Makino.

An implementation in C++ using OpenMP by A. Gainer-Dewar is provided in the [`compsysmed/agdmhs`](//hub.docker.com/r/compsysmed/agdmhs) container in the [`agdmhs`](agdmhs/) directory.
You can run this container online on the [Algorun server](http://agdmhs.algorun.org).

## Other
### Primary decomposition
A method based on computational algebra.

An implementation in Python and Macaulay2 is provided in the [`compsysmed/primdecomp`](//hub.docker.com/r/compsysmed/primdecomp) container in the [`primdecomp`](primdecomp/) directory.
You can run this container online on the [Algorun server](http://primdecomp.algorun.org).

[hdr]: //research.nii.ac.jp/~uno/dualization.html
[paper]: TODO
