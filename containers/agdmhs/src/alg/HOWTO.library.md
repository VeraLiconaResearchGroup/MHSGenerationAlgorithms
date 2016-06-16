# Using `AGDMHS` as a library
## MHS algorithms
The algorithms in this repository are suitable for inclusion in other projects as a library.
Each algorithm is provided as a class in the `agdmhs` namespace extending the `MHSAlgorithm` purely virtual class.
Each provides a method `transversal` with the following signature:

    Hypergraph transversal(const Hypergraph& H)

Some of the algorithms support multiple threads, and some of them support cutoff enumeration.
See the constructors for details.

## Hypergraphs
We provide the `Hypergraph` class for working with hypergraphs.
Internally, it represents each edge as a Boost [`dynamic_bitset`][bitset].

A `Hypergraph` can be constructed in several ways.
One is to construct it from an input file as described in the [README.md](README.md) using the following constructor:

    Hypergraph (const boost::filesystem::path& input_file)

Another is to provide a `std::vector<boost::dynamic_bitset>` representing the edges:

    Hypergraph (const std::vector<boost::dynamic_bitset>& edges)

If you prefer to generate a `Hypergraph` dynamically, you can start from empty, optionally specifying the numbers of vertices and edges you expect:

    Hypergraph (size_t num_verts = 0, size_t num_edges = 0)

and then add edges one at a time:

    void Hypergraph::add_edge(const boost::dynamic_bitset& edge)

See [include/hypergraph.hpp](include/hypergraph.hpp) and [src/hypergraph.cpp](src/hypergraph.cpp) for more details.
See [src/bin-agdmhs.cpp](src/bin-agdmhs.cpp) for an example of computing the transversals and then working with the results.

[bitset]: http://www.boost.org/doc/libs/1_59_0/libs/dynamic_bitset/dynamic_bitset.html
