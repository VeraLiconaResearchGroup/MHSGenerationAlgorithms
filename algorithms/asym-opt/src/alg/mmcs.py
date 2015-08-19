# Python implementation of the MMCS algorithm
# Copyright Vera-Licona Research Group (C) 2015
# Author: Andrew Gainer-Dewar, Ph.D. <andrew.gainer.dewar@gmail.com>

from celery import Celery, group
from kombu import Queue

from hypergraph import Hypergraph
from bitarray import bitarray

app = Celery('mmcs', backend='cache+memcached://', broker='amqp://')
app.conf.update(
    CELERY_ACCEPT_CONTENT=['pickle']
)

# Helper methods to find vertices in edges
def indices_in_bitset(edge):
    return (i for (i, b) in enumerate(edge) if b)

def index_in_bitset(vert, edge):
    return edge[vert]

# TODO: implement uncov as bitarray over edgelist

@app.task
def extend_or_confirm_set(H, S, CAND, crit, uncov):
    """
    Generate extended vertex sets to test

    Keyword arguments:
    H -- a Hypergraph
    S -- a bitarray of vertices of H which will be built into a hitting set
    CAND -- a bitarray of vertices of H which are eligible to be added to S
    crit -- a dict with crit[v] = bitarray(edges e such that e is critical for v in S)
    uncov -- a bitarray of edges of H which are uncovered by S
    """
    # if uncov is empty, S is a hitting set
    if uncov.count() == 0:
        return S

    # if CAND is empty, S cannot be extended, so we're done
    if CAND.count() == 0:
        return

    # Otherwise, get an uncovered edge and remove its elements from CAND
    # TODO: Implement the optimization of Murakami and Uno
    e = next(H.edges()[i] for i in indices_in_bitset(uncov))
    C = CAND & e # Intersection
    CAND = CAND ^ e # Difference

    for v in reversed(list(indices_in_bitset(C))):
        test_vert.delay(H, S, CAND, crit, uncov, v)
        CAND[v] = True

@app.task
def test_vert(H, S, CAND, crit, uncov, v):
    """
    Test a candidate vertex for a hitting set

    Keyword arguments:
    H -- a Hypergraph
    S -- a bitarray of vertices of H which will be built into a hitting set
    CAND -- a bitarray of vertices of H which are eligible to be added to S
    crit -- a dict with crit[v] = bitarray(edges e such that e is critical for v in S)
    uncov -- a bitarray of edges of H which are uncovered by S
    v -- a vertex of H which may be added to S
    """
    # Update uncov and crit
    for e in filter(lambda e: index_in_bitset(v, e), H.edges()):
        e_ind = H.edges().index(e)
        for vert in H.vertices():
            if index_in_bitset(e_ind, crit[vert]):
                crit[vert][e_ind] = False
        if index_in_bitset(e_ind, uncov):
            uncov[e_ind] = False
            crit[v][e_ind] = True

    # If minimality condition holds, process S+v
    S[v] = True
    if all(crit[vert].count() > 0 for vert in indices_in_bitset(S)):
        if uncov.count() == 0:
            return S
        else:
            extend_or_confirm_set.delay(H, S, CAND, crit, uncov)

def run_mmcs(H):
    # Initialize variables
    S = H.no_verts()
    CAND = H.all_verts()
    crit = {v: bitarray([False]*H.n_edges()) for v in H.vertices()}
    uncov = bitarray([True]*H.n_edges())

    # Run the algorithm
    r = extend_or_confirm_set.delay(H, S, CAND, crit, uncov)

    # Gather the results and return them
    hitting_sets = filter(lambda result: result is not None, (result[1] for result in r.collect()))
    return hitting_sets
