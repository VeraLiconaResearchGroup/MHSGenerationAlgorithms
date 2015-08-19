# Python implementation of hypergraphs
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

from bitarray import bitarray

# TODO: Edge class to avoid bitarray semantics

class Hypergraph(object):
    """
    Simple Python representation of a hypergraph backed by bitarray

    Keyword arguments:
    edges -- an Iterable of edges, each an Iterable of non-negative
             vertex indices
    """
    def __init__(self, edges):
        # Find all the vertices used in the edges
        vertex_set = tuple(set().union(*edges))
        largest_vert = max(vertex_set)
        vertex_set = range(largest_vert + 1)

        if any(vert < 0 for vert in vertex_set):
            raise ValueError("All vertex indices must be non-negative")

        # Build the edges
        edge_set = []
        for edge in edges:
            edge_ba = bitarray([False]*(largest_vert + 1))
            for vert in edge:
                edge_ba[vert] = True
            edge_set.append(edge_ba)

        # Store the edges and vertices
        self._vertices = vertex_set
        self._edges = edge_set

    def vertices(self):
        return self._vertices

    def edges(self):
        return self._edges

    def n_verts(self):
        return len(self.vertices())

    def n_edges(self):
        return len(self.edges())

    def size(self):
        # Sum of edge sizes, equal to vertex count with multiplicity
        return sum(e.count() for e in self.edges())

    def all_verts(self):
        return bitarray([True] * self.n_verts())

    def no_verts(self):
        return bitarray([False] * self.n_verts())

    def __str__(self):
        return "Hypergraph with {0} vertices and {1} edges".format(self.n_verts(), self.n_edges())

    def __eq__(self, other):
        return self.__dict__ == other.__dict__

    def __ne__(self, other):
        return not (self == other)
