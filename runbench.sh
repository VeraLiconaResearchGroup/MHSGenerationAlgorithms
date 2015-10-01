#!/bin/zsh
echo Processing input ${1}
./benchmark.py algorithms/all.json $1 results/${1:t:r}.json -n 3 -v -j 1 2 4 6 8 12 16 -c 0 3 5 7 9 -t 3600 -a
