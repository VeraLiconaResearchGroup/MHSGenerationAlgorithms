# Benchmark input data
This directory contains a variety of input data for MHS algorithms.
We give a few details for each here.

## Accident
Anonymized information about several hundred thousand accidents in Flanders during the period 1991–2000.
Originally published in [_Profiling of high-frequency accident locations by use of association rules_][guerts] by Guerts et al.
Converted into an MHS problem by the authors of [_Efficient algorithms for dualizing large-scale hypergraphs_][murakami].
The edges are the complements of maximal frequent itemsets with specified threshold 1000θ for θ ∈ {70, 90, 110, 130, 150, 200}.
MHSes of each hypergraph then correspond to minimal infrequent itemsets.

All hypergraphs have 441 vertices; numbers of edges range from 81 (for $\theta = 200$) to 10968 (for $t = 70$).
This formulation was downloaded from the [Hypergraph Dualization Repository][hdr] and converted into our JSON format.

## E. coli
Metabolic reaction networks from *E. coli*.
Reaction networks for producing acetate, glucose, glycerol, and succinate, along with the combined network, are provided as `Metatool` [examples][ecoli].
These were analyzed to find their "elementary modes" using [`Metatool` 4.9][metatool].
We then constructed a hypergraph with an edge for each elementary mode and encoded it in JSON format.
MHSes of these hypergraphs correspond to "minimal cut sets" of the original networks, which are of interest in studying and controlling these networks.

## OCSANA
Interventions in cell signalling networks.
We obtained network models of two cell signalling networks: EGFR from [_The logic of EGFR/ErbB signaling: theoretical properties and analysis of high-throughput data_][samaga] by Samaga et al. and HER2+ from [_OCSANA: optimal combinations of interventions from network analysis_][veralicona] by Vera-Licona et al.
These networks were analyzed to find their "elementary pathways" using `OCSANA`, using three different algorithms of increasing resolution: shortest paths only (SHORT), including \enquote{suboptimal} paths (SUB), and including all paths up to length 20 (ALL).
For each of these sets of pathways, we then constructed a hypergraph with an edge for each path and encoded it in JSON format.
MHSes of these hypergraphs correspond to "optimal combinations of interventions" in the original networks, which are of interest in studying and controlling these networks.

## Scripts
A variety of scripts are provided for converting files between formats.
These can be used, for example, to convert new inputs into our JSON format.

[guerts]: //dx.doi.org/10.3141/1840-14
[murakami]: //doi.org/10.1016/j.dam.2014.01.012
[ecoli]: //pinguin.biologie.uni-jena.de/bioinformatik/networks/metatool/metatool5.0/ecoli_networks.html
[metatool]: //pinguin.biologie.uni-jena.de/bioinformatik/networks/metatool/new_metatool/new_metatool.html
[samaga]: //dx.doi.org/10.1371/journal.pcbi.1000438
[veralicona]: //dx.doi.org/10.1016/S0959-8049(12)71297-2

[hdr]: //research.nii.ac.jp/~uno/dualization.html
