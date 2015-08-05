######################################################################
SHD: Sparse Hypergraph Dualization      Aug/8/2007
     Coded by Takeaki Uno,   e-mail:uno@nii.jp, 
        homepage:   http://research.nii.ac.jp/~uno/index.html
######################################################################

** This program is available for only academic use, basically.   **
** Anyone can modify this program, but he/she has to write down  **
** the change of the modification on the top of the source code. **
** Neither contact nor appointment to Takeaki Uno is needed.     **
** If one wants to re-distribute this code, do not forget to     **
** refer the newest code, and show the link to homepage          **
** of Takeaki Uno, to notify the news about SHD for the users.   **
** For the commercial use, please make a contact to Takeaki Uno. **

################################
####   Problem Definition   ####
################################

(The readers who are not familiar to the notion of hypergraph, see the
equivalent problems written below.)

Let H=(V,F) be a hypergraph with vertex set V and hyperedge set F.
A hyperedge is a subset of vertex set V, thus F is a subset family
defined on V. F may include several identical hyperedges. A subset H
of V is called a hitting set of F if the intersection of H and any is
non-empty. A hitting set is called minimal if it is not included in
any other hitting set. The dual H'=(V,F') of H is the hypergraph such
that F' is the correction of minimal hitting sets of F. For example,
when V={1,2,3,4}, F={ {1,2}, {1,3}, {2,3,4} }, {1,3,4} is a hitting set
but not minimal, and {2,3} is a minimal hitting set. The dual of H=(V,F)
is given by F'={ {1,2}, {1,3}, {1,4}, {2,3} }. The problem dualization
is to compute the dual of the given hypergraph H=(V,F).

Dualization is equivalent to the following problems:

== (1) minimal hitting set enumeration ==
Let V be a set of items. A transaction is a subset of V. A transaction
database is a collection of transactions which may include several
identical transactions. A transaction database is a subset family
defined on V. The problem is to output all minimal hitting sets of 
given transaction database (or equivalently subset family).

== (2) minimal set covering enumeration ==
For a subset family Y defined on a set X, a set cover S is a subset of
Y such that the union of the member of S is equal to Y (Y=\cup_{A\in S}).
A set covering is called minimal if it is included in no other set
covering. Let us regard Y as a vertex set, and consider a hyperedge
B(x) for each element x of X such that B(x) is the collection of
subsets in X which include x. Then, for the hyperedge set (set family)
F={B(x) | x\in X}, a hitting set of F is a set cover of Y, and vice versa.
Thus, enumerating minimal set coverings is equivalent to the dualization.

== (3) minimal uncovered set enumeration ==
For a subset family Y defined on a set X, let an uncovered set
be a subset of X which is not included in any member of Y. A minimal
uncovered set is one which is included in no other uncovered set.
The problem is to enumerate minimal uncovered sets of Y.
Let !Y be the complement of Y, which is the collection of the complement
of subsets in Y, that is, !Y = {!y | y\in Y}, where !y = X-y is the
complement of y. For a subset y and S, S is not included in y if and
only if S and !y have non-empty intersection. Thus, an uncovered set
of Y is a hitting set of !Y, and vice versa, thus the minimal uncovered
set enumeration is equivalent to the minimal hitting set enumeration.

== (4) circuit enumeration for independent system ==
A subset family F is called an independent system if for any member X
of F, any its subset is also a member of F. A member of F is called an
independent set. An independent set is called a maximal independent set
if it is included in no other independent set. A set is called dependent
if it is not in F. A circuit is a minimal dependent set, i.e., a dependent
set which properly contains no other dependent set. If an independent 
system is given by the set of maximal independent sets of F, then 
the enumeration of circuits of F is equivalent to the enumeration of 
uncovered set of F. The problem of enumerating maximal independent 
sets from the set of circuits is also equivalent to dualization, 
because for !F, the circuit set of !F is the set of maximal independent
sets of F, and the set of maximal independent sets of !F is the circuit
set of F.

== (5) Computing negative border from positive border ==
A function is called Boolean function if it maps members of sets 2^V
to 0 or 1. A Boolean function B is called monotone (resp., anti-monotone)
if it satisfies that for any set X with B(X)=0 (resp., B(X)=1), 
any subset X' of X satisfies B(X)=0 (resp., B(X)=1).
For a monotone function B, a subset X is called positive border
if B(X)=0 and no Y, B(Y)=0 properly includes X, and is called negative
border if B(X)=1 and no Y, B(Y)=1 is properly included in X. For given 
the set of positive borders, the problem of enumerating negative borders
is equivalent to dualization, because the problem is equivalent to 
uncovered set enumeration (3) and circuit enumeration (4).
The enumeration of positive borders from the negative borders can be 
done by enumerating uncovered sets for the collection of the complement
of negative borders.

== (6) DNF<->CNS transformation ==
A DNF is a formula all whose clauses are composed of literals
connected by "or", and all clauses are connected by "and". A CNF is a
formula all whose clauses are composed of literals connected by "and",
and all clauses are connected by "or". Any formula can be represented
in both DNF formula and CNF formula. Let D be a DNF formula composed
of variables x1,...,xn and clauses C1,...,Cm. A DNF/CNF is called
monotone if no clause contains literal with ``not''. Then, S is a
hitting set to the clauses of D if and only if the assignment obtained
by setting literals in S to true gives a true assignment of D. Let H
be a minimal CNF formula equivalent to D. H has to include any minimal
hitting set as its clause, since any clause has to contain at least
one literal of each clause of D. Thus, computing minimal CNF
equivalent H has to include all minimal hitting sets of D. In the
same reason, computing the minimal DNF from a CNF is equivalent to
the dualization.


##################################
####   Mathematical Aspects   ####
##################################

- Finding the minimum cardinality hitting set is known to be NP-hard,
 it is the same for finding the minimum cardinality set covering.
- Finding one minimal hitting set is done in O(||F||) time.
- There is no known output polynomial time algorithm for dualization.
 Its existence is a famous open problem.


#####################
####    Usage    ####
#####################

    ==== How to Compile ====
Unzip the file into arbitrary directory, and execute "make".
Then you can see "shd" (or shd.exe) in the same directory.

    ==== Command Line Options  ====
To execute SHD, just type shd and give some parameters as follows.

% shd 09Dcq input-filename [output-filename]

"%" is not needed to type. It is a symbol to represent command line.
To see a simple explanation, just execute "shd" without parameters.

"input-filename" is the filename of the input transaction database.
The 1st letter of input-filename must not be '-'. Otherwise it is 
regarded as an option. The input file format is written below. 
"output-filename" is the name of file to write the hyperedge (minimal
hitting set) the program finds. You can omit the output file to
see only the number of minimal hitting sets classified by the
cardinality. If the output file name is "-", the solutions will be
output to the standard output.

The first parameter is given to the program to indicate the task and
the method.

 _: no output to standard output (including messages w.r.t. input data)
 (solve uncovered set enumeration problem)
 %: show progress of the computation
 +: if the output file exists, append the solutions to the output file
 0: normal version (reverse search approach)
 9: straightforward version
 D: depth-first search version
 P: do not execute the pruning algorithm
 c: take the complement of the input file
 t: transpose the database so that item i will be transaction i, thus
    if item i is included in j-th transaction, then item j will be
    included in i-th transaction.

0 and 9 can not be given simultaneously, but any other combination
is accepted. D command switches to the algorithm to a branch-and-bound
type depth first search algorithm, and P and R inactivate puring 
algorithm and data reduction algorithm, respectively. These commands
are basically for evaluating the performance of the algorithm, thus
usually just giving 0 or D is sufficient.

 -# [num]:stop after outputting [num] solutions
 -, [char]: give the separator of the numbers in the output
    the numbers in the output file are separated by the given
    character [char].
 -Q [filename]: replace the output numbers 
    according to the permutation table written in the file of
    [filename], replace the numbers in the output. The numbers in the
    file can be separated by any non-numeric character such as newline
    character. 
 -l [num]:output trees/graphs with at least [num] vertices
 -u [num]:output trees/graphs with at most [num] vertices

Examples)

- dualize "tt.dat" by normal version, and output to "out"

% shd 0 tt.dat out

- dualize "ttt.dat" by straightforward depth-first search version, and 
make no output file

% shd 9D ttt.dat

- enumerate minimal uncovered sets of "tt.dat" and output to "min.out"
 by depth-first search version

% shd Dc tt.dat min.out


###############################
####   Input File Format   ####
###############################

The vertex set must be numbers ranging from 0 to n. They do not have to be
consecutive numbers from 0 to n, but the program considers that the vertex
set is {0,...,n}, thus uses memory linear in n. Thus they should be
consecutive, for the efficiency. In the case of uncovered set enumeration,
all numbers from 0 to n can be a part of uncovered sets.

Each line (row) of the input file is corresponding to a hyperedge (subset,
or transaction). The vertices (items, or elements) included in a hyperedge
is listed in a line. The separator of numbers can be any non-numeric letter,
such as ","  " " ":" "a", etc.

Example)  ( "[EOF]" is the end of file )
0 1 2
1
2 3 4
4,1 2 3
2,1
[EOF]

#################################################################
####    Use General Names for Variables and Other Formats    ####
#################################################################

We can transform variable names in general strings to numbers so that we
can input the data to the program, by some script files. 

-- transnum.pl table-file [separator] < input-file > output-file
Read file from standard input, and give a unique number to each name written
by general strings (such as ABC, ttt), and transform every string name to 
a number, and output it to standard output. The mapping from string names to
numbers is output to table-file. The default character for the separator of 
the string names is " "(space). It can be changed by giving a character for
the option [separator]. For example, A,B is a string name, if the separator 
is space, but if we set the separator to ",", it is regarded as two names 
A and B. This is executed by "transnum.pl table-file "," < input-file...".

-- untransnum.pl table-file < input-file > output-file
According to the table-file output by transnum.pl, un-transform numbers to 
string names. The output of the program is composed of numbers, thus 
it is used if we want to transform to the original string names.
It reads file from standard output, and output to the standard output.

-- appendnum.pl
When we want to distinct the same words in different columns, use this 
script. This append column number to each words, so we can distinct them.
Then, by using transnum.pl, we transform the strings to numbers.

-- transpose.pl
Transpose the file. In the other words, consider the file as an adjacency
matrix of a bipartite graph, and output the transposed matrix, or exchange
the positions of vertices and hyperedges. For an input file, output the
file in which the i-th line corresponds to item i, and includes the
numbers j such that i is included in the j-th line of the input file.

#########################################
####   Batch Files for Simple use    ####
#########################################

For general string names, we have several batch files scripts for basic usages
"exec_shd", "exec_shd_", "sep_shd", or "sep_shd_". For example, when a hypergraph
with "general item names" is, 

dog pig cat
cat mouse
cat mouse dog pig
cow horse
horse mouse dog
[EOF]

All these replace strings in the input database by numbers, execute SHD,
and replace the numbers in the output file by the original strings.
The usage of the scripts are

% exec_shd [FCMfIq] input-filename support output-filename [options]

You have to specify F, C or M, and output filename. The separator of the
items is " " (blank, space). If you want to use other character as a
separator, use "sep_shd". The usage is 

% sep_shd separator [FCMfIq] input-filename support output-filename [options]

Almost same as "exec_shd" but you have to specify separator at the fourth 
parameter. "exec_lcm_" and "sep_lcm_" are both for the aim to distinct 
the same items in the different columns. For example, it is used to the
database such that different items are there in different columns, but 
some special symbols, such as "- is for missing data", are used commonly.
An example is;

A true small
C true -
A false middle
B - -
C - middle
A true -
[EOF]

In the output file, the items are followed by "." and numbers where 
the numbers are the column number. For example, "dog.0" means the item
"dog" on the 0th(first) column.

The usage of them are the same as "exec_shd" and "sep_shd", respectively.
The scripts use files of the names "__tmp1__", "__tmp2__", and "__tmp3__", 
The file of these names will be deleted after the execution.

Example)

% exec_shd 0 test2.dat out.dat

% sep_shd_ "," Dc test3.dat out.dat


#############################
####    Output Format    ####
#############################

When the program is executed, the program prints out the #items,
#transactions, and other features of the input database to standard
error. After the termination of the enumeration, it outputs the total
number of itemsets found (frequent/closed/maximal itemsets), and the 
numbers of itemsets of each size. For example, if there are 4 frequent
itemsets of size 1, 2 frequent itemsets of size 3, and 1 frequent itemset
of size 3, then the output to standard output will be,

9   <= total #hyperedges
0   <= #hyperedges of size 0 
4   <= #hyperedges of size 1
3   <= #hyperedges of size 2
1   <= #hyperedges of size 3

If "q" is given in the first parameter, these do not appear in the
standard output.

If output-filename was given, then the hyperedges found are written to
the output file. Each line of the output file is the list of vertices
included in a hyperedge, separated by " ". For example,

1 5 10 2 4

which means hyperedge {1,2,4,5,10} is in the dual.
In the output file, the vertices in each row are not sorted. If you want
to sort it, use the script "sortout.pl". The usage is just,

% sortout.pl < input-file > output-file

"input-file" is the name of file to which SHD outputs, and the sorted
output will be written in the file of the name "output-file".
The vertices of each hyperedge will be sorted in the increasing order of
the vertex ID's, and all the hyperedges (lines) will be also sorted, by the
lexicographical order (considered as a string).
(Actually, you can specify separator like sortout.pl ",").



###########################
####    Performance    #### 
###########################

The performance of SHD is stable, for both computation time and memory use.
The initialization and preprocess time of SHD is linear in the size of 
input hypergraph. The computation time is intuitively linear in the product
of (output hyperedges) and (number of input hyperedges).

Memory usage of SHD is very stable. It is an advantage compared to other 
implementations. The memory usage of SHD is almost linear in the size of
the input database. Approximately SHD uses integers at most three times
as much as the database size, which is the sum of the sizes of each hyperedge.
The memory usage of the other implementations increases as the increase
of the number of hyperedges, but that of SHD does not.


\######################################################
####    Introductions to Hypergraph Dualization    ####
#######################################################

under construction.



###################################################
####    Algorithms and Implementation Issue    #### 
###################################################

The algorithm consists two search methods and one fast minimality
checking algorithm. First we explain fast minimality check.


  === minimality check ===

When we have a set S, it is easy to confirm that S is a hitting set of F
or not. First, put a mark to all vertices included in S, and for each 
hyperedge, look at the marks of all vertices included in it. In this way,
the computation time is O(||F||) where ||F|| is the sum of the sizes
of the hyperedges in F.
If there is a hyperedge such that no vertex of it has a mark, then 
S is not a hitting set. Confirming that S is a minimal hitting set or
not is the next task. A straightforward method for the task is to check
whether S-v is a hitting set or not for all vertices v in S. In this way
we need O(||F|| \times |S|) time. Instead of that, we use a
characterization for the minimality. For a vertex v in S, if a hyperedge H
includes only v among vertices of S, i.e., S\cap H = {v}, we call H 
"critical hyperedge" of v. If v has no critical hyperedge, S-v is also 
a hitting set, thus the existence of critical hyperedge assures that
v can not remove from the set. Thus, S is a minimal hitting set if and
only if every vertex in S has a critical hyperedge.

Let us see an example. Suppose that hypergraph H=(V,F) is 
V={1,2,3,4}, F={ {1,2}, {1,3}, {2,3,4} }, and S = {1,3,4} is a hitting set.
{1,2} is a critical hyperedge of 1, {1,3} is a critical hyperedge of 3, 
but 4 has no critical hyperedge. It means that {1,3,4} - 4 has intersection
to all hyperedges. Actually, {1,3} is a hitting set. For {1,3}, again, 
1 has a critical hyperedge {1,2}, and 3 has a critical hyperedge {1,3}, 
then the removal of any vertex from {1,3} yields non-hitting set, thus 
{1,3} is a minimal hitting set.

We denote the set of critical hyperedges of vertex v by crit(v,S).
A hyperedge can be a critical hyperedge for at most one vertex, thus 
the sum ||crit()|| of the sizes of crit() for all vertices does not
exceed |F|. For a hyperedge H, computing the vertex v such that H is a
critical hyperedge of v, or confirm that H is critical hyperedge of no
vertex, can be done in O(|H|) time by computing S\cap H (by looking at
marks on each vertex of H). Thus, computing all critical hyperedges
for all vertices can be done in O(||F||) time.

For each vertex v, let Occ(v) be the set of hyperedges including v.
For a vertex set S, let uncov(S) be the set of hyperedges having empty
intersection to S. S is a hitting set if and only if uncov(S) is an
empty set. Suppose that S is not a hitting set, and each vertex in S
has at least one critical hyperedge, i.e., for any u in S, crit(u) !=
emptyset, and consider an addition of vertex v to S. Then, we can see
(a) uncov(S+v) = uncov(S) - Occ(v)
(b) crit(v,S+v) = uncov(S) \cap Occ(v)
(c) for u in S, crit(u,S+v) = crit(u,S) - Occ(v)
Then, by marking all hyperedges in Occ(v), we can compute all crit() 
in O(|Occ(v)| + ||crit()|| ).

Let us see an example. Suppose that hypergraph H=(V,F) is 
V={1,2,3,4,5}, F = { {1,3}, {1,4}, {1,2,3}, {2,3}, {2,4,5}, {3,4}, {4,5} },
and S={1,2}, v=3. Then, uncov(S) = { {3,4}, {4,5} }, crit(1,S) =
{ {1,3}, {1,4} }, crit(2,S) = { {2,3}, {2,4,5} }, and Occ(v) =
{ {1,3}, {1,2,3}, {2,3}, {3,4} }. Then, for S+3, 

(a) uncov(S+3) = {{3,4}, {4,5}} - {{1,3}, {1,2,3}, {2,3}, {3,4}} = {{4,5}}
(b) crit(3,S+3) = {{3,4}, {4,5}} \cap {{1,3}, {1,2,3}, {2,3}, {3,4}} = {{3,4}}
(c) crit(1,S+3) = {{1,3}, {1,4}} - {{1,3}, {1,2,3}, {2,3}, {3,4}} = {{1,4}}
(c) crit(2,S+3) = {{2,3}, {2,4,5}} - {{1,3}, {1,2,3}, {2,3}, {3,4}} = {{2,4,5}}


  === simple depth-first search method ===
Using the above minimality check, we develop an depth-first search
type algorithm. The strategy is very simple. We start by setting S to
an empty set, and adding a vertex to S one by one, with keeping the
condition that any vertex in S has at least one critical hyperedge.
When S is a hitting set, S is a minimal hitting set, thus we output it.
To avoid the duplication, in each iteration, we add only vertices 
larger than the maximum index vertex in S, denoted by tail(S).
The algorithm is written as follows.

Algorithm SHDdfs_straight (S, crit(), uncov(S))
1.  if uncov(S) is empty, then output S and return;
2.  for each vertex v>tail(S),
 2-1.   compute uncov(S+v) and crit() for S+v,
 2-2.   if crit(u,S+v) is non-empty for any vertex u in S
           then call SHDdfs_straight (S+v, crit(), uncov(S+v))

We can examine this algorithm by giving options 9D for SHD, i.e.,
 execute "shd 9D input-file output-file". 
To improve the efficiency, we use several techniques for maintaining crit,
explained as follows, and a pruning technique. Their combination gives 
our first algorithm.

The correctness of the algorithm is as follows. For hypergraph H=(V,F), 
let Z(H) be the set of vertex sets S such that any vertex in S has at
least one critical hyperedge. Observe that by removing a vertex from S,
no vertex loses its critical hyperedge, i.e., crit(u,S) is included in
crit(u, S-v) for any vertex v. This comes from above property (c).
Thus, any X in Z(H), any subset of X is also in Z(H), thereby monotone.
For example, for hypergraph H(V,E), V={1,2,3,4}, F={ {1,2}, {1,3}, {2,3,4} },
 Z(H) = { {1}, {2} , {3}, {4}, {1,2}, {1,3}, {1,4}, {2,3} }
A minimal hitting set is a maximal element (positive border) in Z(H). 
Thus, by the algorithm, any X in Z(H) is generated by X-tail(X), thus
it enumerates all the elements in Z(H).

  === finding violating vertices ===
For vertex set S in Z(H), we call a vertex v addible if S+v is in Z(H).
The algorithm adds vertices and update crit, 
to check whether the addition is in Z(H) or not. Here we show an efficient
way for the check. For a vertex u in S and v not in S, crit(u, S+v) is 
an empty set if and only if any critical hyperedge of u includes v, i.e., 
crit(u,S)\subseteq Occ(v), equivalently crit(u,S)\cap Occ(v) = crit(u,S).
For given u, computing crit(u,S)\cap Occ(v) for all v can be done 
in O(||cirt(u,S)||) time by occurrence deliver. Thus, by performing 
occurrence deliver for critical hyperedges of all vertices in S, 
we can obtain the vertices v satisfying that S+v is in Z(H).
This can be done O(||cirt()||) time, which is faster than 
computing crit() for each vertex v not in S.


  === pruning technique ===
Suppose that we have a set S each whose vertex has at least one critical
hyperedge, and E={v1,...,vk} be a hyperedge in uncov(S). Then, we know
that any minimal hitting set includes at least one vertex in E. Thus,
we can divide the minimal hitting sets into several groups G1,...,Gk
where Gi is the group of minimal hitting sets including vi but not
including v1,...,v{i-1}. For enumerating minimal hitting sets in Gi,
we put marks on vertices v1,...,v{i-1} not to choose them. Then, the
algorithm is written as follows. The marks of vertices are cleared at 
the initialization.

Algorithm SHDdfs (S, crit(), uncov(S))
1.  if uncov(S) is empty, then output S and return
2.  compute all unmarked addible vertices to S
3.  choose a hyperedge E from uncov(S) which includes the least
       unmarked addible vertices
4.  for each unmarked addible vertex v in E, put a mark
5.  compute crit\cap Occ(v) ,uncov(S)\cap Occ(v) for all addible
        unmarked vertices by occurrence deliver
6.  for each unmarked addible vertex v in E,
 6-1      call SHDdfs_straight (S+v, crit(), uncov(S+v))
 6-2      clear bucket of v, and erase mark on v

We can execute this algorithm by giving option as
"shd D input-file output-file".


  === reverse search method ===
The next search strategy is based on a technique so called reverse search.
Suppose that hyperedges of H is {E1,...,Em}, and we denote the hypergraph
having hyperedges E1,...,Ei by Hi. Let mincrit(v,S) be the minimum index 
hyperedge in crit(v,S). For a vertex subset S in Z(H),
we define the core hyperedge core_h(S) by the hyperedge Ei such that 
For hyperedges E1,...,Ei, S is not in Z(H{i-1}) but is in Z(Hi).
Ei is maximum one among the minimum critical hyperedges of all vertices,
i.e., Ei = max_{u\in S} mincrit (u,S), thus core_h(S) is
defined uniquely. Thereby, the core hyperedge is always a critical 
hyperedge of a vertex. We say the vertex core vertex and denote it by 
core_v(S). We define the parent P(S) of S by S - core_v(S).

For example, for hypergraph H(V,E), V={1,2,3,4}, F={ {1,2}, {1,3}, {2,3,4} },
 Z(H) = { emptyset, {1}, {2} , {3}, {4}, {1,2}, {1,3}, {1,4}, {2,3} },
the parent of {1}, {2}, {3}, {4} is the emptyset, and the parent of {1,2},
{1,3}, {1,4} is {1}, and the parent of {2,3} is {2}.
The core hyperedge of {2,3} is {1,3}, and the core vertex is 3.

This parent child relation is acyclic, and except for the empty set,
any vertex set in Z(H) has its parent, thus it forms a tree.
Here we consider a search method which moves from parent to children 
recursively in a depth-first search manner. Suppose that S+v is a child
of S. Then, the core hyperedge of S+v is the minimum index hyperedge E in 
uncov(S). Thus, v has to be included in E. Conversely, for S in Z(H), 
let E be the minimum index hyperedge in uncov(S). Then, any child of S
is obtained by adding a vertex v in E to S. Suppose that v is a vertex
in E. If S+v is in Z(H), then mincrit(v,S) = Ei. When hyperedge set 
F = { {1,2}, {1,3}, {2,3,4} } and S={2}, the minimum hyperedge in
uncov(S) is {1,3}. Both S+1 and S+3 are in Z(H). The core hyperedge of
{2,3} is {1,3} and the core vertex is 3, thus {2} is the parent of {2,3}.
However, the core hyperedge of {1,2} is {2,3,4}, it differs from {1,3}.
The core vertex of {1,2} is 2, thus the parent of {1,2} is {1}.
Therefore, S+v is a child of S if and only if mincrit(u,S+v) has no larger
index than mincrit(u,S+v). This holds when no vertex u in S loses all
its critical hyperedges by adding v, i.e., for each vertex u in S, 
at least one critical hyperedge whose index is less than mincrit(v,S+v)
(minimum hyperedge in uncov(S)) does not include v. This can be checked
by occurrence deliver in the same way. The following algorithm traverses
the tree induced by this parent child relation.

Algorithm SHD (S, crit(), uncov(S))
1.  if uncov(S) is empty, then output S and return;
2.  Ei := minimum index hyperedge in uncov(S)
3.  for each vertex v in Ei, 
 3-1.   compute uncov(S+v) and crit() for S+v,
 3-2.   if crit(u,S+v) is non-empty for any vertex u in S
           and mincrit(u,S+v) is less than Ei, 
               then call SHD (S+v, crit(), uncov(S+v))

We can examine this algorithm by giving options 9 for SHD, i.e.,
 execute "shd 9 input-file output-file". 
To improve the efficiency, we use the same technique above.
We can examine the improved algorithm by giving options 0 for SHD, i.e.,
 execute "shd 0 input-file output-file". 


###############################
####    Acknowledgments    #### 
###############################

We thank to Ken Satoh of National Institute of Informatics Japan, Hiroki 
Arimura of Hokkaido University for their contribution for the research,
A part of the research of SHD is supported by Grant-in-aid from the
Ministry of Education, Science, Sport and Culture of Japan 
(Monbu-Kagaku-Sho). We also thank Dr. Renato Vimieiro for a bug report.



##########################
####    References    #### 
##########################


