from hsgraph import HsGraph
from node import Node
from collections import defaultdict
from ..util.debug import __line__, __function__
import sys
import time

first = lambda x: iter(x).next()
def second(x):
    i = iter(x)
    i.next()
    return i.next()

# ===================================================================================================================== #

class Greiner(object):
    
    """
    Standard implementation of the HS-DAG algorithm as in ``A correction to the algorithm in Reiter's Theory of Diagnosis''
    by Greiner et al. "Standard" means that this version works for normal (N)AB predicates and not with fault modes. 
    In fact, it works with sets of arbitrary immutable objects which can be compared (i.e. numbers, tuples, ...) and 
    calculates the corresponding hitting sets. 
    
    Note that this version uses the "split" theorem prover interface (i.e. a separate consistency check 
    for a node and call for a new conflict set). The other two versions below are variations using different
    approaches. 
    """
    
    def __init__(self, oracle, tree, options):
        self.oracle = oracle
        self.tree = tree
        self.max_card = options['max_card']
        self.max_time = options['max_time']
        self.prune = options['prune']
        self.cache = options['cache']
        self.debug_pruning = options['debug_pruning']
        self.max_num_solutions = options.get('max_num_solutions', None)
        self.debug_pruning_step = 1
        self.cache_hits = 0
        self.cache_hits_fm = 0
        self.cache_misses = 0
        self.oracle.set_options(**options)
        self.i = 0
        self.time_map = {}
        self.start_time = None
        self.timeout = False
        self.inferred_nodes = 0
        self.pruned_edges = 0

    def hsdag(self):
        """
        the main function to be called after initialization
        """
    
        tree = self.tree
        max_time = self.max_time
        self.start_time = time.time()
        
        if tree is None:
            self.tree = tree = HsGraph()
            root = Node(0, None, 0, 0)
            root.tree = tree
            root.h = frozenset()
            tree.add_node(root)
    
        if tree.worklist is None:
            tree.worklist = [root]
             
        worklist = tree.worklist
        if max_time:
            end_time = time.time() + max_time
        else:
            end_time = None
        
        while 1:
            node = worklist[0]
            del worklist[0]
            if end_time and  time.time() > end_time:
                self.timeout = True
                break
            if node.tree and node.state == Node.State.OPEN:
                worklist += self.process_node(node)
            if len(worklist) == 0:
                break
            if self.max_num_solutions is not None and len(self.time_map) >= self.max_num_solutions:
                break
        
        return tree
    
    def process_node(self, node):
        """
        processes a single node according to the algorithm rules. 
        """
        tree = self.tree
        oracle = self.oracle
        max_card = self.max_card
        
        if node.level >= 2:
            # closing
            if self.h_issuperset(node):
                node.state = Node.State.CLOSED
                return []
        
        sigma = None
        
        if oracle.check_consistency(node.h):
            node.state = Node.State.CHECKED
            if self.time_map is not None:
                self.time_map[node.h] = time.time() - self.start_time
            return []
        
        if max_card and node.level >= max_card:
            return []
        
        if self.cache:
            h = node.h
            sigma = self.query_cache(h)
        
        if sigma is None:
            sigma = oracle.get_conflict_set(node.h)
            
            if sigma is None:
                print "ERROR: oracle seems inconsistent: node's h is not consistent, but also no new conflict set returned (%s,%d,%s)" % (__file__, __line__(), __function__())

        if self.cache:
            tree.cs_cache.append(sigma)
    
        node.set = sigma
        
        if self.prune or self.debug_pruning:
            self.prune_tree(node)      # pruning
    
        if node.tree is None: # (it might have been removed from the tree during pruning)
            return []
        
        
        generated_nodes = self.expand_node(node, tree)
        return generated_nodes
    
    def expand_node(self, node, tree):
        """
        constructs the children of a node according to its conflict set. 
        """
        result = []
        for s in node.set:
            
            new_h = node.h | set([s])
            if new_h in tree.h_cache:           # reusing
                target = tree.h_cache[new_h]
                tree.add_edge_without_checks((node, target), 1, s)
            else:
                n = Node(node.level+1, None, 0, 0)
                n.tree = tree
                n.h = new_h
                tree.add_node(n)
                tree.add_edge_without_checks((node, n), 1, s)
                result.append(n)
        return result
    
    def prune_tree(self, node):
        """
        checks if the given node is subject to pruning or results in any other
        existing node to be pruned (i.e. if a subset conflict set has been found 
        all the unnecessary edges and the corresponding sub-trees are removed).  
        """
        tree = self.tree
        sigma = node.set
        for s_prime in tree.set_cache.keys():
            if sigma < s_prime:
                self.i += 1
                for n_prime in list(tree.set_cache.get(s_prime, [])):
                    n_prime.set = sigma
                    if n_prime.tree is not None:
                        for n in list(tree.neighbors(n_prime)):  
                            if tree.edge_label((n_prime, n)) not in n_prime.set:
                                self.cut_edge((n_prime, n))
    
    
    def cut_edge(self, edge):
        """
        deletes the given edge and prunes the descendant nodes, except for those who have another ancestor 
        which is not being pruned
        """
        first_node = edge[1]
        descendants = set([first_node])
        if self.tree.incidents(first_node) == [edge[0]]:    # do not prune if first_node has another parent!
            self.get_descendants(first_node, descendants)
            if self.prune:
                for n in descendants:
                    if self.debug_pruning:
                        write_hsdag(self.tree, 'hs-dag-step-%d.pdf'%self.i, highlight={'pink':[n]}, title="deleting node %s"%(n))
                        self.i += 1
                    self.tree.del_node(n)
    
    def get_descendants(self, node, pruned_nodes):
        """
        gets all descendants of a node being currently pruned, except for those who have another ancestor 
        which is not being pruned
        """
        if (node in pruned_nodes) or self.all_parents_pruned(node, pruned_nodes):
            pruned_nodes.add(node)
            for n in self.tree.neighbors(node):
                self.get_descendants(n, pruned_nodes)
    
    def all_parents_pruned(self, node, pruned_nodes):
        """
        returns True if all parents of ``node'' are in ``pruned_nodes''
        """
        return not filter(lambda n: n not in pruned_nodes, self.tree.incidents(node))
    
    def h_issuperset(self, node):
        """
        Checks if `node's h is a superset of any other node's h in the graph.
        We have to check levels 1 to |h|-1 only. 
        """
        for n in xrange(1, node.level):
            if n in self.tree.checked_nodes:
                for other_node in self.tree.checked_nodes[n]:
                    if other_node.h and node.h > other_node.h:
                        return True

    def query_cache(self, h):
        for cs in self.tree.cs_cache:
            if len(cs & h) == 0:
                self.cache_hits += 1
                return cs
        self.cache_misses += 1
        return None

# ===================================================================================================================== #
    
class GreinerN(Greiner):
    """
    This is a variant of HS-DAG which uses the cache for (non-)consistency checking. That is, 
    before any consistency checking is done, the cache is queried for a hit. A hit means, 
    no consistency check is necessary and a corresponding conflict set has already been found. 
    A miss means that we have to proceed as usual (consistency check + compute conflict). 
    """

    def process_node(self, node):
        tree = self.tree
        oracle = self.oracle
        max_card = self.max_card
               
        if node.level >= 2:
            # closing
            if self.h_issuperset(node):
                node.state = Node.State.CLOSED
                return []
        
        sigma = self.query_cache(node.h)
        
        if sigma:
            # node is inconsistent!
            if max_card and node.level >= max_card:
                # but we stop at this level
                return []
            else:
                node.set = sigma
                # pruning
                if self.prune or self.debug_pruning:
                    self.prune_tree(node)
                if node.tree is None: # (it might have been removed from the tree during pruning)
                    return []
                # expand node
                generated_nodes = []
                for s in sigma:
                    new_h = node.h | set([s])
                    if new_h in tree.h_cache:           # reusing
                        target = tree.h_cache[new_h]
                        tree.add_edge_without_checks((node, target), 1, s)
                    else:
                        n = Node(node.level+1, None, 0, 0)
                        n.tree = tree
                        n.h = new_h
                        tree.add_node(n)
                        tree.add_edge_without_checks((node, n), 1, s)
                        generated_nodes.append(n)
                return generated_nodes
        else:
            # node may/may not be consistent
            if max_card and node.level >= max_card:
                # just see if this is consistent using the TP
                if oracle.check_consistency(node.h):
                    node.state = Node.State.CHECKED
                    if self.time_map is not None:
                        self.time_map[node.h] = time.time() - self.start_time
                    return []
                else:
                    return []
            else:
                # maybe we need to expand
                if oracle.check_consistency(node.h):
                    # oh, its consistent, says TP
                    node.state = Node.State.CHECKED
                    if self.time_map is not None:
                        self.time_map[node.h] = time.time() - self.start_time
                    return []
                else:
                    # get new conflict set from TP
                    sigma = oracle.get_conflict_set(node.h)
                    # add to cache
                    tree.cs_cache.append(sigma)
                    node.set = sigma
                    # pruning
                    if self.prune or self.debug_pruning:
                        self.prune_tree(node)
                    if node.tree is None: # (it might have been removed from the tree during pruning)
                        return []
                    # ok, expand....
                    generated_nodes = []
                    for s in sigma:
                        new_h = node.h | set([s])
                        if new_h in tree.h_cache:           # reusing
                            target = tree.h_cache[new_h]
                            tree.add_edge_without_checks((node, target), 1, s)
                        else:
                            n = Node(node.level+1, None, 0, 0)
                            n.tree = tree
                            n.h = new_h
                            tree.add_node(n)
                            tree.add_edge_without_checks((node, n), 1, s)
                            generated_nodes.append(n)
                    return generated_nodes



class GreinerO(Greiner):
    """
    This is a variant of HS-DAG which uses the "old" theorem-prover interface (i.e. not featuring a separate 
    consistency check). Not having a separate consistency check means that the cache must be queried 
    before computing any new conflict set (otherwise it would make no sense). In that sense, it is similar 
    to HSTN above, otherwise it works like the normal HST algorithm. 
    """

    def process_node(self, node):
        tree = self.tree
        oracle = self.oracle
        max_card = self.max_card
        cache = self.cache
               
        if node.level >= 2:
            # closing
            if self.h_issuperset(node):
                node.state = Node.State.CLOSED
                return []
        
        sigma = None
        if cache:
            sigma = self.query_cache(node.h)
        
        if sigma:
            # node is inconsistent!
            if max_card and node.level >= max_card:
                # but we stop at this level
                return []
            else:
                node.set = sigma
                # pruning
                if self.prune or self.debug_pruning:
                    self.prune_tree(node)
                if node.tree is None: # (it might have been removed from the tree during pruning)
                    return []
                # expand node
                generated_nodes = []
                for s in sigma:
                    new_h = node.h | set([s])
                    if new_h in tree.h_cache:           # reusing
                        target = tree.h_cache[new_h]
                        tree.add_edge_without_checks((node, target), 1, s)
                    else:
                        n = Node(node.level+1, None, 0, 0)
                        n.tree = tree
                        n.h = new_h
                        tree.add_node(n)
                        tree.add_edge_without_checks((node, n), 1, s)
                        generated_nodes.append(n)
                return generated_nodes
        else:
            # node may/may not be consistent
            if max_card and node.level >= max_card:
                # see if consistent, but use get_conflict_set
                sigma = oracle.get_conflict_set(node.h)
                if cache and sigma:
                    tree.cs_cache.append(sigma)
                if sigma is None:
                    node.state = Node.State.CHECKED
                    if self.time_map is not None:
                        self.time_map[node.h] = time.time() - self.start_time
                    return []
                else:
                    return []
            else:
                # maybe we need to expand
                sigma = oracle.get_conflict_set(node.h)
                if cache and sigma:
                    tree.cs_cache.append(sigma)
                if sigma is None:
                    # oh, its consistent, says TP
                    node.state = Node.State.CHECKED
                    if self.time_map is not None:
                        self.time_map[node.h] = time.time() - self.start_time
                    return []
                else:
                    node.set = sigma
                    # pruning
                    if self.prune or self.debug_pruning:
                        self.prune_tree(node)
                    if node.tree is None: # (it might have been removed from the tree during pruning)
                        return []
                    # ok, expand....
                    generated_nodes = []
                    for s in sigma:
                        new_h = node.h | set([s])
                        if new_h in tree.h_cache:           # reusing
                            target = tree.h_cache[new_h]
                            tree.add_edge_without_checks((node, target), 1, s)
                        else:
                            n = Node(node.level+1, None, 0, 0)
                            n.tree = tree
                            n.h = new_h
                            tree.add_node(n)
                            tree.add_edge_without_checks((node, n), 1, s)
                            generated_nodes.append(n)
                    return generated_nodes
                


class GreinerFaultModes(Greiner):
    
    """
    A HS-DAG version able to deal with fault modes. That means that a conflict set is now made 
    up from tuples (x,y) where x specifies the component and y the corresponding mode. 
    The oracle is asked for the modes to expand a given component, where those components
    already present in the current hitting set h are ignored. Additionally, a conflict set 
    consists of TWO sets named ``top'' and ``bot'', where top contains those tuples with 
    fault mode 0 (the NOT ABNORMAL mode) and bot those with fault mode != 0. This enables 
    a faster cache query mechanism where not all tuples have to be compared. 
    
    Other than that, also the cache query mechanisms, superset checking and also the pruning 
    is slightly to reflect this setting.
    """
    
    def __init__(self, oracle, tree, options):
        super(GreinerFaultModes, self).__init__(oracle, tree, options)
        self.time_map = {}
    
    def expand_node(self, node, tree):
        result = []
        sigma = node.set
        for s,_ in sigma:
            if s not in map(first, node.h):
                modes = set(self.oracle.get_fault_modes_for(s)) - node.h
                for m in modes:
                    new_h = node.h | set([m])
                    if new_h in tree.h_cache:           # reusing
                        target = tree.h_cache[new_h]
                        tree.add_edge_without_checks((node, target), 1, m)
                    else:
                        if not self.issuperset(new_h, node.level+1):
                            n = Node(node.level+1, None, 0, 0)
                            n.tree = tree
                            n.h = new_h
                            tree.add_node(n)
                            tree.add_edge_without_checks((node, n), 1, m)
                            result.append(n)
        return result
    
    def query_cache(self, h):
        for cs in self.tree.cs_cache:
            if h == cs.bot: 
                self.cache_hits_fm += 1
                return cs
            elif not self.intersect(cs.top, h) and cs.bot <= h:
                self.cache_hits += 1
                return cs

        self.cache_misses += 1
        return None


    def intersect(self, cs, h):
        """
        for the intersection, only check the component part in this case, e.g. x in (x,y) tuple
        """
        for t in cs:
            for u in h:
                if t[0] == u[0]:
                    return True
        return False
    
    def issuperset(self, h, level):
        """
        Checks if `node's h is a superset of any other node's h in the graph.
        We have to check levels 1 to |h|-1 only. 
        """
        for n in xrange(1, level):
            if n in self.tree.checked_nodes:
                for other_node in self.tree.checked_nodes[n]:
                    if other_node.h and h > other_node.h:
                        return True
                    
    def prune_tree(self, node):
        """
        For the fault mode setting, again this uses only the component part 
        to decide which edges to cut away in case of a superset being found. 
        """
        
        tree = self.tree
        sigma = node.set
        for s_prime in tree.set_cache.keys():
            if sigma < s_prime:
                for n_prime in list(tree.set_cache.get(s_prime, [])):
                    n_prime.set = sigma
                    if n_prime.tree is not None:
                        # important: make a copy of the neighbors list because it 
                        # changes during iteration due to pruning
                        for n in list(tree.neighbors(n_prime)):  
                            if tree.edge_label((n_prime, n))[0] not in map(first,n_prime.set):              # <-- custom check for fault modes
                                self.cut_edge((n_prime, n))
                                

                                
class GreinerOFaultModes(GreinerFaultModes):

    def process_node(self, node):
        tree = self.tree
        oracle = self.oracle
        max_card = self.max_card
        cache = self.cache
               
        if node.level >= 2:
            # closing
            if self.h_issuperset(node):
                node.state = Node.State.CLOSED
                return []
        
        sigma = None
        if cache:
            sigma = self.query_cache(node.h)
        
        if sigma:
            # node is inconsistent!
            if max_card and node.level >= max_card:
                # but we stop at this level
                return []
            else:
                node.set = sigma
                # pruning
                if self.prune or self.debug_pruning:
                    self.prune_tree(node)
                if node.tree is None: # (it might have been removed from the tree during pruning)
                    return []
                generated_nodes = self.expand_node(node, tree)
                return generated_nodes
        else:
            # node may/may not be consistent
            if max_card and node.level >= max_card:
                # see if consistent, but use get_conflict_set
                sigma = oracle.get_conflict_set(node.h)
                if cache and sigma:
                    tree.cs_cache.append(sigma)
                if sigma is None:
                    node.state = Node.State.CHECKED
                    if self.time_map is not None:
                        self.time_map[node.h] = time.time() - self.start_time
                    return []
                else:
                    return []
            else:
                # maybe we need to expand
                sigma = oracle.get_conflict_set(node.h)
                if cache and sigma:
                    tree.cs_cache.append(sigma)
                if sigma is None:
                    # oh, its consistent, says TP
                    node.state = Node.State.CHECKED
                    if self.time_map is not None:
                        self.time_map[node.h] = time.time() - self.start_time
                    return []
                else:
                    node.set = sigma
                    # pruning
                    if self.prune or self.debug_pruning:
                        self.prune_tree(node)
                    if node.tree is None: # (it might have been removed from the tree during pruning)
                        return []
                    # ok, expand....
                    generated_nodes = self.expand_node(node, tree)
                    return generated_nodes
