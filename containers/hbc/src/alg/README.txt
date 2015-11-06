                                     
                                    MT MINER 

Synopsis
--------
mtminer levelwisely computes the minimal transversals of a hypergraph.
It is a C++ implementation of the HBC algorithm  [1].

Example
-------
$ cat > /tmp/test
1 2 
2 3 
^D
$ mtminer /tmp/test
2 
# depth 2
1 3 

Maintaner
---------
mtminer is maintaned by
Francois Rioult - CNRS GREYC UMR6072 Francois.Rioult@unicaen.fr
as a part of the KD-Ariane project https://forge.greyc.fr/projects/kdariane
See https://forge.greyc.fr/projects/kdariane/wiki/Mtminer.

Coders
------
Céline Hébert - CNRS GREYC UMR6072
Arnaud Soulet - 

Reference
---------
[1] Céline Hébert, Alain Bretto, Bruno Crémilleux: Optimizing
hypergraph transversal computation with an anti-monotone
constraint. SAC 2007: 443-444
