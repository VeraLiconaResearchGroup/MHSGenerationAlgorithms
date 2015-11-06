try:
    from numpy import *
    numpy_missing = False
except:
    numpy_missing = True

subsetChecks = 0

def strip_component(matrix, index):
    matrix[:,index] = ma.masked

def strip(matrix, index):
    deleted_cs = ma.where(matrix[:,index] == True)
    matrix[deleted_cs,:] = ma.masked
    strip_component(matrix, index)
    
def subsumed(d, j):
    global subsetChecks
    removed = set()
    for dd in d:
#        print "d: %s, j: %s" % (d, j)
        subsetChecks += 1
        if j >= dd:
            for r in removed:
                d.remove(r)
            return True
        elif dd >= j:
            removed.add(dd)
    for r in removed:
        d.remove(r)
    return False

def build_matrix(scs):
    comp = array(list(set().union(*scs)))
    a = zeros((len(scs), comp.size), dtype=bool)
    a = ma.array(a, mask=zeros(a.shape, dtype=bool))
    for row,cs in enumerate(scs):
        for c in cs:
            a[row,where(comp == c)] = True
    return a, comp

def staccato(o):
    """
    Simple recursive implementation of the Staccato algorithm ("A Statistics-directed Minimal Hitting 
    Set Algorithm" by Rui Abreu and Arjan J.C. van Gemund) using numpy matrices. This version computes
    _all_ solutions, i.e. lambda = 1, L = infinity.  
    
    """
    if numpy_missing:
        raise Exception("Please install numpy in order to run STACCATO/Py")
    a, comp = build_matrix(o.sets)
    hs = staccato_recursive(a)
    return set(frozenset(comp[list(h)]) for h in hs)
    

def staccato_recursive(a):    
    d = set()
    
    # determine cols and rows in a that are not masked completely 
    comps_alive = nonzero(ma.count(a, 0))[0]
    cs_alive = nonzero(ma.count(a, 1))[0]

    # count the number of occurrences of the components in the conflict sets
    counts = array([len(ma.nonzero(a[:,c])[0]) for c in comps_alive])
    
    in_all = where(counts == len(cs_alive))[0]
    in_none = where([(ma.where(a[:,c] == False)[0]).size == len(cs_alive) for c in comps_alive])[0]

    # add components that are in all conflict sets to the result `d' 
    if in_all.size > 0:
        d = d.union([frozenset([c]) for c in comps_alive[in_all]])
    
    # do not process components that are either in all or in no conflict set
    to_delete = union1d(in_all, in_none)
    strip_component(a, comps_alive[to_delete])
    comps_alive = delete(comps_alive, to_delete)
    counts = delete(counts, to_delete)
    
    # build the component ranking
    order = argsort(counts)[::-1]
    ranking = comps_alive[order]
    
    mask = a.mask.copy() 
    
    # call recursively for remaining components
    for j in ranking:
        strip(a, j)
        d_prime = staccato_recursive(a)
        a.mask = mask
        for j_prime in d_prime:
            j_prime = frozenset([j]) | j_prime
            if not subsumed(d, j_prime):
                d.add(j_prime)
    
    return d

