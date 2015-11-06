# Algorithms in the repository
The repository includes a large collection of algorithm implementations.
We give a references and a few details for each here.
The listing below is sorted taxonomically, rather than according to which container provides each algorithm; information about the containers can be found in their respective directories.

## Edge iteration
### Berge
A sequential algorithm based on an algebraic decomposition of the input hypergraph.
First published in 1984 by Berge in _Hypergraphs: combinatorics of finite sets_.
An implementation in C++ by A. Gainer-Dewar is provided in the [agdmhs][] container.

### HS-DAG
A corrected version of Reiter's algorithm, published in [_A correction to the algorithm in Reiter's theory of diagnosis_](//dx.doi.org/10.1016/0004-3702(89)90079-9) by Greiner, Smith, and Wilkerson.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.
The implementation is provided here in the [pymbd][] container under the terms of the authors' license.

### HST
A corrected version of Reiter's algorithm, published in [_A variant of Reiter's hitting-set algorithm_](//dx.doi.org/10.1016/S0020-0190(00)00166-6) by Wotawa.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.
The implementation is provided here in the [pymbd][] container under the terms of the authors' license.

### BMR
An improved Berge's algorithm published in [_A fast algorithm for computing hypergraph transversals and its application in mining emerging patterns_](//doi.org/10.1109/ICDM.2003.1250958).
An implementation in C by K. Murakami is available from the [Hypergraph Dualization Repository][hdr] and is provided here in the [bmr][] container by permission from the authors.

### DL
An improved Berge's algorithm published in [_Mining border descriptions of emerging patterns from dataset pairs_](//doi.org/10.1007/s10115-004-0178-1) by Dong and Li.
An implementation in C by K. Murakami is available from the [Hypergraph Dualization Repository][hdr] and is provided here in the [bmr][] container by permission from the authors.

### KS
A variation of Berge's algorithm which searches depth first and thus avoids the need to store large intermediate hypergraphs in memory.
Published in [_An efficient algorithm for the transversal hypergraph generation_](//doi.org/10.7155/jgaa.00107) by Kavvadias and Stavroupoulos.
A Pascal implementation by D. Kavvadias and E. Stavropoulos is available from the [Hypergraph Dualization Repository][hdr] and the [author's page](//lca.ceid.upatras.gr/~estavrop/transversal/) and is provided here in the [ks][] container by permission from the authors.

***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.

## Divide and conquer
### FK-A
The A algorithm from [_On the complexity of dualization of monotone disjunctive normal forms_](//doi.org/10.1006/jagm.1996.0062), Fredman, M. and Khachiyan, L, as improved in [_An efficient implementation of a quasi-polynomial algorithm for generating hypergraph transversals and its application in joint generation_](//doi.org/10.1.1.85.6762), E. Boros et al.
Has the best known asymptotic bounds on runtime of any serial algorithm with a public implementation.
An implementation in compiled C is available from the [Hypergraph Dualization Repository][hdr] and is provided here in the [fka-begk][] container by permission from the authors.
An implementation in C++ of the original, unmodified algorithm is provided here in the [agdmhs][] container.

***WARNING***: Testing has revealed that the BEGK software does not accurately generate all MHSes.

### BOOL
A divide-and-conquer algorithm introduced in [_The computation of hitting sets: review and new algorithms_](//dx.doi.org/10.1016/S0020-0190(02)00506-9) by Lin and Jiang.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.
The implementation is provided here in the [pymbd][] container under the terms of the authors' license.

### STACCATO
A divide-and-conquer approximation algorithm, published in _A low-cost approximate minimal hitting set algorithm and its application to model-based diagnosis_ ([PDF](//haslab.uminho.pt/ruimaranhao/files/sara09.pdf)) by Abreu and Gemund.
An implementation in Python by T. Quaritsch and I. Pill is available as part of PyMBD, which is distributed as a [zip bundle](//modiaforted.ist.tugraz.at/downloads/pymbd.zip) from Pill's website.
The implementation is provided here in the [pymbd][] container under the terms of the authors' license.

### ParTran
A parallel algorithm using a Berge-like calculation on divided subhypergraphs, published in [_Parallel computation of the minimal elements of a poset_](//doi.org/10.1145/1837210.1837221) by Leiserson et al.
An implementation in Cilk++ by those authors is available from the BPASlib project's [webpage](//bpaslib.org) and is provided here in the [partran][] container.

***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.

### MHS²
A parallel algorithm from _An efficient distributed algorithm for computing minimal hitting sets_ ([PDF](//dx-2014.ist.tugraz.at/papers/DX14_Mon_PM_S1_paper1.pdf)) by Cardoso and Abreu.
An implementation in C++ by those authors is available from their [GitHub repository](//github.com/npcardoso/MHS2) and is provided here in the [mhs2][] container under the terms of the GPL3 open-source license.

## Transversal buildup
### HBC
A vertex-wise algorithm inspired by ideas from data mining and published in [_A data mining formalization to improve hypergraph minimal transversal computation_](//www.infona.pl/resource/bwmeta1.element.baztech-article-BUS5-0014-0020) ([PDF](//cremilleux.users.greyc.fr/papers/FundInfoFinal07.pdf)) by Hébert, Bretto, and Crémilleux.
An implementation in C++, written by C. Hébert and now maintained by F. Rioult, is available as [MtMiner](//forge.greyc.fr/projects/kdariane/wiki/Mtminer) and is provided here in the [hbc][] container by permission from the authors.

***WARNING***: Testing has revealed that this software does not accurately generate all MHSes.

### OCSANA-Greedy
A vertex-wise "greedy" algorithm published in [_OCSANA: optimal combinations of interventions from network analysis_](//doi.org/10.1093/bioinformatics/btt195).
An implementation in Java by M. Kordi, originally written for a refactoring of [OCSANA](//bioinfo-out.curie.fr/projects/ocsana/OCSANA.html), is provided here in the [ocsana][] container.

### MMCS
Highly performant algorithm published in [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) in 2014 by Murakami and Uno.
Those authors provide a C implementation at the [Hypergraph Dualization Repository][hdr], which is redistributed here in the [shd][] container.
An implementation in C++ which takes advantage of multiple cores using OpenMP and supports efficient 'cutoff' enumeration is provided here in the [agdmhs][] container.

### RS
Highly performant algorithm published in [_Efficient algorithms for dualizing large-scale hypergraphs_](//doi.org/10.1016/j.dam.2014.01.012) in 2014 by Murakami and Uno.
Those authors provide a C implementation at the [Hypergraph Dualization Repository][hdr], which is redistributed here in the [shd][] container.
An implementation in C++ which takes advantage of multiple cores using OpenMP and supports efficient 'cutoff' enumeration is provided here in the [agdmhs][] container.

## Full cover
### BM
A "global parallel" algorithm based on a full cover decomposition of the input hypergraph.
Published in [_A fast and simple parallel algorithm for the monotone duality problem_](//doi.org/10.1007/978-3-642-02927-1_17) by E. Boros and K. Makino.
An implementation in C++ using OpenMP by A. Gainer-Dewar is provided here in the [agdmhs][] container.

[hdr]: //research.nii.ac.jp/~uno/dualization.html
[agdmhs]: agdmhs/
[bmr]: bmr/
[dl]: dl/
[fka-begk]: fka-begk/
[hbc]: hbc/
[ks]: ks/
[mhs2]: mhs2/
[ocsana]: ocsana/
[partran]: partran/
[pymbd]: pymbd/
[shd]: shd/
