# Benchmark results
The results of our benchmarks are recorded here.
These benchmarks were performed on a workstation-class computer with an Intel Xeon E5-2630v3 processor with eight cores (with HyperThreading allowing 16 concurrent threads) at 2.4 GHz and 32GB of ECC DDR4 RAM.
Algorithms which support cutoff enumeration (that is, finding only hitting sets of size up to some fixed c) were run with c = 5, 7, and 10 as well as full enumeration.
Algorithms which support multiple threads were run with t = 1, 2, 4, 6, 8, 12, and 16 threads.

For each of the [benchmark_inputs](../benchmark_inputs) subdirectories, there is a corresponding directory here holding the results of those benchmarks.
These files are JSON encoded and record a variety of data in keys:
* `algs`: the list of algorithms run in the benchmark
* `runtimes`: for each algorithm, thread count, and cutoff count (as applicable), the times taken by three runs of the algorithm in seconds (or Infinity if the algorithm crashed or failed to complete in one hour)
* `transversal_counts`: the number of transversals found by each algorithm (for sanity checking)

The script [collator.py](collator.py) can process these files into CSV tables for analysis.
