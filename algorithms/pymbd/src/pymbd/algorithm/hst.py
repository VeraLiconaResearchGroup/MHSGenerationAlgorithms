from collections import defaultdict
from blist import sortedset
from ..util.debug import __line__, __function__
import time

OPEN = 1
CLOSED = 2
CHECKED = 3

generated_nodes = 0

class HSTNode(object):
    
    __slots__ = ['level', 'parent', 'children', 'state', 'index', 'max', 'h', 'closed_children']

    next_node_number = 0

    def __init__(self, level, index, max):
        global generated_nodes
        self.level = level
        self.parent = None
        self.children = {}
        self.state = OPEN
        self.index = index
        self.max = max
        self.h = None
        self.closed_children = 0
        generated_nodes += 1
        HSTNode.next_node_number += 1
        
    def num_children(self):
        sum = 0
        for c in self.children.values():
            sum += c.num_children()
        return sum+1
    
    def get_children(self):
        children = [self]
        for c in self.children.values():
            children.extend(c.get_children())
        return children
    
class HSTree(object):
    
    def __init__(self):
        self.root = None
        self.worklist = None
        self.checked_nodes = defaultdict(set)
        self.labels = {}
        self.max = 0
        self.seen_comps = set()
        self.cs_cache = sortedset(key=len)
        self.cs_cache.append = lambda v: self.cs_cache.add(v)
        
    def num_nodes(self):
        return self.root.num_children()
    
    def nodes(self):
        return self.root.get_children()

class HST(object):
    
    """
    Implementation of the HST hitting set algorithm as described by Wotawa 2001 in "A variant of Reiter's 
    hitting-set algorithm". Note that this algorithm does NOT use the graph infrastructure in the "common"
    package, but uses the customized HSTNode above for performance reasons. It is pretty much optimized for
    speed and very similar to the ("original") Java implementation from Wotawa. Nevertheless, the Java 
    implementation is still faster. 
    
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
        self.cache_hits = 0
        self.cache_misses = 0
        self.oracle.set_options(**options)
        
    def hst(self):
        """
        Calculate the HST (hitting-set tree) according to wotawa2001variant
        """
        tree = self.tree
        oracle = self.oracle
        
        if tree is None:
            tree = self.tree = HSTree()
            first = oracle.get_first_conflict_set()
            if first is None: 
                return tree
            self.assign_labels(first, tree)
            tree.root = HSTNode(0, 0, tree.max) 
    
        if tree.worklist is None:
            tree.worklist = [tree.root]
             
        worklist = tree.worklist
        if self.max_time:
            end_time = time.time() + self.max_time
        else:
            end_time = None
        
        while 1:
            node = worklist[0]

            if end_time and  time.time() > end_time:
                break
            del worklist[0]
            if node.state == OPEN:
                worklist += self.process_node(node)
            if len(worklist) == 0:
                break
    
        return tree
    
    def process_node(self, node):
        """
        Processes a HST node by 
        1. calculating the set h from the path to the root
        2. calculating a new conflict set for h
        2a. if no conflict set found, close the node 
        2b. prune the tree
        3. assign labels for new components
        4. close the node if its index == max
        4a. prune the tree
        5. otherwise create children for index+1 ... max
        """
        tree = self.tree
        oracle = self.oracle 
        max_card = self.max_card
        
        close = self.check_close(node, set())
        if close:
            node.state = CLOSED
            if node.parent:
                node.parent.closed_children += 1
            if self.prune:
                self.prune_from_node(node)
            return []
        
        h = self.get_h(node, self.tree.labels)
        y = None
        
        if oracle.check_consistency(h):
            node.state = CHECKED
            node.h = h
            tree.checked_nodes[len(h)].add(node)
            return []
        
        if max_card and node.level >= max_card:
            return []
        
        if self.cache:
            y = self.query_cache(h)
        
        if y is None:
            y = oracle.get_conflict_set(h)
        
        if y is None:
            print "ERROR: oracle seems inconsistent: node's h is not consistent, but also no new conflict set returned (%s,%d,%s)" % (__file__, __line__(), __function__())

        if self.cache:
            tree.cs_cache.append(y)
    
        self.assign_labels(y, tree)
        generated_nodes = self.expand_node(node)
            
        return generated_nodes
    
    
    def check_close(self, node, h):
        """
        checks if `node' can be closed: (`h' is initially the empty set)
        - no, if it is the root of the tree or on level below (there cannot be a proper subset in this case)
        - yes if another subtree of the grandparent contains a hitting set which is a subset of h
        - otherwise go up one level and run check_close from there
        """
        if not node.parent:
            return False
        if not node.parent.parent:
            return False
        h.add(node.index)
        if self.check_tree(node.parent.parent, h):
            return True
        else:
            return self.check_close(node.parent, h) 
    
    
    def check_tree(self, node, h):
        """
        checks if in any subtree of `node' there is a hitting set (=path to a minimal node) which is 
        a subset of `h'. this is done by traveling down edges leading to nodes with an index in h
        """ 
        for i in h:
            if node.index < i <= node.max and i in node.children:
                n = node.children[i]
                if n.state == CHECKED:
                    return True
                elif n.state == OPEN:
                    h.remove(n.index)
                    if self.check_tree(n, h):
                        h.add(n.index)
                        return True
                    else:
                        h.add(n.index)
    
    
    def prune_from_node(self, node):
        """
        prunes nodes in the tree, starting from `node' (which was probably closed just before)
        by removing any node on the path to the root which is marked as closed or has no children.
        """
        parent = node.parent
        while parent:
            if node.state == CLOSED and parent.closed_children == len(parent.children):
                parent.children.clear()
                parent.state = CLOSED
                if parent.parent:
                    parent.parent.closed_children += 1
                
                node = parent
                parent = parent.parent
            else:
                node = None
                parent = None
    
    
    def get_h(self, node, labels):
        """
        constructs the hitting set of `node' by following the path to the root and adding all elements indicated 
        by the node.index values along the path.
        """
        h = set([labels[node.index]]) if node.index > 0 else set()
        parent = node.parent
        while parent:
            if parent.index > 0:
                h.add(labels[parent.index])
                parent = parent.parent
            else:
                parent = None
        return frozenset(h)
    
    
    def assign_labels(self, set, tree):
        """
        assigns new labels to components in `set', which have not been seen before and updates max value accordingly 
        """
        for comp in set:
            if comp not in tree.seen_comps:
                tree.max += 1
                tree.labels[tree.max] = comp
                tree.seen_comps.add(comp)

    def query_cache(self, h):
        """
        check if a conflict set is in the cache which is not hit by the hitting set h. if yes return it. 
        """
        for cs in self.tree.cs_cache:
            if len(cs & h) == 0:
                self.cache_hits += 1
                return cs
        self.cache_misses += 1
        return None


    def expand_node(self, node):
        """
        constructs the child nodes for a given node based on its state and max/index values. 
        This function is called by the process_node methods (both in this class and derivatives). 
        """
        tree = self.tree
        node.max = tree.max
        if node.index == node.max:
            node.state = CLOSED
            if node.parent:
                node.parent.closed_children += 1
            if self.prune:
                self.prune_from_node(node)
            return []
        
        generated_nodes = []
        for i in xrange(node.index+1, node.max+1):
            n = HSTNode(node.level+1, i, 0)
            node.children[i] = n
            n.parent = node
            generated_nodes.append(n)
            
        return generated_nodes

class HSTN(HST):
    """
    This is a variant of HST which uses the cache for (non-)consistency checking. That is, 
    before any consistency checking is done, the cache is queried for a hit. A hit means, 
    no consistency check is necessary and a corresponding conflict set has already been found. 
    A miss means that we have to proceed as usual (consistency check + compute conflict). 
    """

    def process_node(self, node):
        tree = self.tree
        oracle = self.oracle 
        max_card = self.max_card
        
        close = self.check_close(node, set())
        if close:
            node.state = CLOSED
            if node.parent:
                node.parent.closed_children += 1
            if self.prune:
                self.prune_from_node(node)
            return []
        
        h = self.get_h(node, self.tree.labels)
        y = self.query_cache(h)
        
        if y:
            # node is inconsistent
            if max_card and node.level >= max_card:
                return []
            else:
                self.assign_labels(y, tree)
                return self.expand_node(node)
        else: 
            # node node may/may not be consistent
            if max_card and node.level >= max_card:
                # just see if this is consistent using the TP
                if oracle.check_consistency(h):
                    node.state = CHECKED
                    node.h = h
                    tree.checked_nodes[len(h)].add(node)
                    return []
                else:
                    return []
            else:
                if oracle.check_consistency(h):
                    # oh, its consistent, says TP, okay...
                    node.state = CHECKED
                    node.h = h
                    tree.checked_nodes[len(h)].add(node)
                    return []
                else:
                    # get new conflict set from TP
                    y = oracle.get_conflict_set(h)
                    # add to cache
                    tree.cs_cache.append(y)
                    # and expand 
                    self.assign_labels(y, tree)
                    return self.expand_node(node)

class HSTO(HST):
    """
    This is a variant of HST which uses the "old" theorem-prover interface (i.e. not featuring a separate 
    consistency check). Not having a separate consistency check means that the cache must be queried 
    before computing any new conflict set (otherwise it would make no sense). In that sense, it is similar 
    to HSTN above, otherwise it works like the normal HST algorithm. 
    """

    def process_node(self, node):
        tree = self.tree
        oracle = self.oracle 
        max_card = self.max_card
        
        close = self.check_close(node, set())
        if close:
            node.state = CLOSED
            if node.parent:
                node.parent.closed_children += 1
            if self.prune:
                self.prune_from_node(node)
            return []
        
        h = self.get_h(node, self.tree.labels)
        y = None
        
        if self.cache:
            y = self.query_cache(h)
        
        if y:
            # node is inconsistent
            if max_card and node.level >= max_card:
                return []
            else:
                self.assign_labels(y, tree)
                return self.expand_node(node)
        else: 
            # node node may/may not be consistent
            if max_card and node.level >= max_card:
                # see if consistent, but use get_conflict_set
                y = oracle.get_conflict_set(h)
                if self.cache and y:
                    tree.cs_cache.append(y)
                if y is None:
                    node.state = CHECKED
                    node.h = h
                    tree.checked_nodes[len(h)].add(node)
                    return []
                else:
                    return []
            else:
                # maybe we need to expand
                y = oracle.get_conflict_set(h)
                if self.cache and y:
                    tree.cs_cache.append(y)
                if y is None:
                    # oh, its consistent, says TP, okay...
                    node.state = CHECKED
                    node.h = h
                    tree.checked_nodes[len(h)].add(node)
                    return []
                else:
                    # expand 
                    self.assign_labels(y, tree)
                    return self.expand_node(node)
                    

