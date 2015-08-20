from blist import sortedset
from pygraph.classes.digraph import digraph
from ..util.two_way_dict import TwoWayDict

class HsGraph(digraph):
    """
    Extended DAG for HS-DAG
    """
    
    def __init__(self):
        super(HsGraph, self).__init__()
        
        self.checked_nodes = dict()
        """
        A dictionary containing entries in the form { level : [node, node, node] } where each entry stores the 
        checked nodes having a hitting-set with length `level`. 
        """
        
        self.closed_nodes = set()
        """
        The list of nodes that have been marked as closed (X)
        """
        
        self.worklist = None
        """
        The list of nodes still to be processed. Used if the algorithm is stopped and continued later.
        """
        self.labels = TwoWayDict()
        
        """
        A mapping of elements to their labels (which are numbers), e.g. { 'a' : 1, 'b': 2 }
        """
        
        self.h_cache = TwoWayDict()
        """
        A dictionary containing a mapping between hitting sets and nodes, e.g. { frozenset(['a']) : Node(...) }
        to speed up node reuse in Greiner's algorithm.
        """
        
        self.set_cache = dict()
        """
        A mapping of conflict sets to a list of nodes which have been assigned this conflict set, e.g.
        { frozenset(['a', 'b']) : [ Node(...), Node(...) ] }
        """
    
        # self.cs_cache = list()
        self.cs_cache = sortedset(key=len)
        self.cs_cache.append = lambda v: self.cs_cache.add(v)
        """
        The ordered set of conflict sets that have been returned by the "oracle" up to now. Used to 
        minimize oracle (= theorem-prover accesses) 
        """
        
    def __nonzero__(self):
        """
        If this graph is used as a boolean, we always treat it as True. This prevents the expensive counting 
        of the number of nodes, as in the default implementation (where __len__ is used).
        """
        return True
    
    def add_node(self, node, attrs = []):
        super(HsGraph, self).add_node(node, attrs)
        node.tree = self
        
    def add_edge_without_checks(self, edge, wt = 1, label="", attrs = []):
        """
        Adds an edge to the graph without checking if the given nodes are present or if the edge is already present. 
        Use with care!
        """
        
        u, v = edge
        self.node_neighbors[u].append(v)
        self.node_incidence[v].append(u)
#        self.set_edge_weight((u, v), wt)
#        self.add_edge_attributes( (u, v), attrs )
        self.set_edge_properties( (u, v), label=label, weight=wt )

    def del_node(self, node):
        super(HsGraph, self).del_node(node)
        node.tree = None
        if node in self.closed_nodes:
            self.closed_nodes.remove(node)
        for nodeset in self.checked_nodes.values():
            if node in nodeset:
                nodeset.remove(node)
        if node in self.h_cache.values():
            del self.h_cache[self.h_cache.get_key(node)]
        empty_cache_entries = []
        for set, nodelist in self.set_cache.iteritems():
            if node in nodelist:
                nodelist.remove(node)
                if len(nodelist) == 0:
                    empty_cache_entries += [set]
        for i in empty_cache_entries:
            del self.set_cache[i]

    def isRoot(self, node):
        return not self.incidents(node)
    
    def parent(self, node):
        if not self.isRoot(node):
            return self.incidents(node)[0]
        else:
            return None
    
    