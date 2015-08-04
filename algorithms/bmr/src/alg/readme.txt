### how to compile ###
% g++ -O3 -o BMR BMR.cpp

"%" is not needed to type. It is a symbol to represent command line.

### how to execute ###
% ./BMR input-filename output-filename

Example)
% ./BMR in.dat out.dat

### inputfile format ###
The vertex set must be numbers ranging from 0 to n ({0,1,...,n}).
Each line (row) of the input file is corresponding to a hyperedge (subset,
or transaction). The vertices (items, or elements) included in a hyperedge
is listed in a line. The separator of numbers can be some non-numeric letter,
such as ","  " " , etc.

Example)  ( "[EOF]" is the end of file )
0 1 2
1
2 3 4
4,1 2 3
2,1
[EOF]
