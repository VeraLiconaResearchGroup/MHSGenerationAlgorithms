#!/bin/zsh
echo Processing input ${1}
./benchmark.py ${2} $1 results/${1:h:t} -n 3 -s -v -j 1 2 4 6 8 12 16 -c 0 5 7 10 -t 3600 -a
