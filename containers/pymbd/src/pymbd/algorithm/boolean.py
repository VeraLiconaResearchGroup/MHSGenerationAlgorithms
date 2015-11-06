from collections import namedtuple
from math import ceil
from ..util.sethelper import fs
import sys
import time


WorklistItem = namedtuple('WorklistItem', ['h', 'c'])
"""
A WorklistItem is a tuple with two elements: 
    h = the current parts of the resulting hitting set that are already fixed, i.e. in 1*H(/2 /3), h=fs(1)
    c = the set of conflict sets to be processed, i.e. the argument of H, fs(2,3) in the example above. 
"""

class Boolean(object):

    def __init__(self, oracle, options):
        self.oracle = oracle
        self.max_card = options['max_card']
        self.min_card = options['min_card']
        self.previous_worklist = options.get('previous_worklist',None)
        self.max_time = options.get('max_time',None)
        self.heuristic = options.get('heuristic',1)
        self.heuristic_time = 0
        self.recursive_heuristics = {
            0: self.recursive_heuristic_0, 
            1: self.recursive_heuristic_1,
            2: self.recursive_heuristic_2,
            3: self.recursive_heuristic_3,
            4: self.recursive_heuristic_4,
            5: self.recursive_heuristic_5
        }
        self.iterative_heuristics = {
            0: self.iterative_heuristic_0, 
            1: self.iterative_heuristic_1,
            2: self.iterative_heuristic_2,
            3: self.iterative_heuristic_3,
            4: self.iterative_heuristic_4,
            5: self.iterative_heuristic_5
        }
        self.num_h_calls = 0
        self.options = options
        self.options['stopping_optimization'] = options.get('stopping_optimization', False)


    def boolean_iterative(self):
        
        """
        Boolean Hitting Set Algorithm as described by Li Lin and Yunfei Jiang in "The computation of hitting sets: Review and 
        new algorithms". Note that rule (4) of the algorithm has been slightly modified to reduce the number of non-minimal 
        hitting sets generated. 
        
        >>> boolean_iterative(Oracle(*set([fs(2,4),fs(2,11), fs(0,2,10), fs(0,2,9,17), fs(1,2,3,4), fs(2,8,11,14,15,18)])), (1,sys.maxint), None, None)[1]
        frozenset([frozenset([9, 10, 11, 4]), frozenset([17, 10, 11, 4]), frozenset([0, 11, 4]), frozenset([2])])
        
        >>> boolean_iterative(Oracle(*set([fs(1, 2), fs(2,3)])), (1,sys.maxint), None, None)[1]
        frozenset([frozenset([1, 3]), frozenset([2])])
            
        """
        
        previous_worklist = self.previous_worklist
        oracle = self.oracle
        max_time = self.max_time
        max_card = self.max_card
        min_card = self.min_card
    
        if previous_worklist is not None:
            worklist, finished = previous_worklist
        else:
            worklist = {}
            worklist[0] = [WorklistItem(frozenset(), oracle.sets)]
            finished = {}
        
        if max_time:
            end_time = time.time() + max_time
        else:
            end_time = None
        
        card = 0       
        while card < len(worklist):
            self.process(worklist, finished, card, end_time)
            card += 1
            if card > max_card:
                break
    
        self.minimize(finished)
    
        if self.options['stopping_optimization']:
            finished_filtered = finished
        else:
            finished_filtered = dict((k,v) for k,v in finished.iteritems() if min_card <= k <= max_card)
        return (worklist,finished), self.join(finished_filtered)
    
    def process(self, worklist, finished, card, end_time):
        """
        processes the group of worklist entries with given cardinality while end_time is not elapsed
        """
        while worklist[card]:
            if end_time and  time.time() > end_time:
                return
            item = worklist[card].pop(0)
            result = self.h_iterative(item)
            for r in result:
                if len(r.c) == 0:
                    if len(r.h) in finished:
                        finished[len(r.h)].add(r.h)
                    else:
                        finished[len(r.h)] = set([r.h])
                else:
                    if len(r.h) in worklist:
                        worklist[len(r.h)].append(r)
                    else:
                        worklist[len(r.h)] = [r]
    
    def join(self, hs_dict):
        return set(set().union(*hs_dict.values()))
    
    def boolean_recursive(self):
        """
        >>> boolean_recursive(Oracle(fs(1,2), fs(1,2,3,4)), (1,1))
        frozenset([frozenset([2]), frozenset([1])])
        
        >>> boolean_recursive(Oracle(fs(1,2), fs(1,2,3,4)), (2,2))
        frozenset([])
        """
        oracle = self.oracle        
        
        min_card = self.min_card
        max_card = self.max_card
        
        sys.setrecursionlimit(2**16)    # this is for really large examples, the default limit is like 1000.
        
            
        if self.options['stopping_optimization']:
            return self.join(self.minimize(self.shs_as_dict_by_length(filter(lambda x: min_card <= len(x), self.h_recursive(set(oracle.sets), 0, max_card)))))
        else:
            return self.join(self.minimize(self.shs_as_dict_by_length(filter(lambda x: min_card <= len(x) <= max_card, self.h_recursive(set(oracle.sets), 0, max_card)))))
    
    def first(self, set):
        """
        returns the first element of a set (or, any other iterable).
        """
        return iter(set).next()
    
    def minimize(self, hs_dict):
        """
        removes supersets from a set of hitting sets organized in a by-length dict.
        >>> minimize({1: set([frozenset([2])]), 2: set([frozenset([1, 2])])})
        {1: set([frozenset([2])]), 2: set([])}
        """
        for l in hs_dict:
            for hs in set(hs_dict[l]):
                self.check_superset(l, hs, hs_dict)
                
        return hs_dict
    
    def shs_as_dict_by_length(self, shs):
        """
        returns a dict containing the sets in shs arranged by their length, e.g.
        >>> shs_as_dict_by_length(fs(fs(1,2), fs(2)))
        {1: set([frozenset([2])]), 2: set([frozenset([1, 2])])}
        """
        hs_dict = {}
        for hs in shs:
            if len(hs) in hs_dict:
                hs_dict[len(hs)].add(hs)
            else:
                hs_dict[len(hs)] = set([hs])
        return hs_dict
    
    def check_superset(self, l, hs, hs_dict):
        """
        used by minimize internally.
        """
        for i in range(1, l):
            if i in hs_dict:
                for other in hs_dict[i]:
                    if hs >= other:
                        hs_dict[l].remove(hs)
                        return
    
    def get_most_common_element(self, scs):
        """
        >>> get_most_common_element(read_sets('{{3,2,1}, {1,3}, {2}, {1,4}}'))
        1
        """
        t0 = time.time()
        comp = set().union(*scs)
        count = dict([(c, 0) for c in comp])
        for cs in scs:
            for s in cs:
                count[s] += 1
        max = -1
        result = None
        for e,c in count.items():
            if c > max:
                max = c
                result = e
        t1 = time.time()
        self.heuristic_time += (t1-t0)
        return result
    
    def get_shortest_cs(self, scs):
        s = iter(scs).next()
        l = len(s)
        for cs in scs:
            if len(cs) < l:
                l = len(cs)
                s = cs
        return s
    
    
    ################################################################################################
    ##############                  ITERATIVE ALGORITHM                 ############################
    ################################################################################################
    
    def h_iterative(self, item):
        """
        Iterative implementation of the boolean hitting set algorithm (calculates only one step at a time)
        
        Rule 1:
        >>> h_iterative(WorklistItem(h=fs(1), c=fs()))
        [WorklistItem(h=frozenset([1]), c=frozenset([]))]
        
        Rule 2:
        >>> h_iterative(WorklistItem(h=fs(1), c=fs(fs(2))))
        [WorklistItem(h=frozenset([1, 2]), c=frozenset([]))]
        
        Rule 3:
        >>> h_iterative(WorklistItem(h=fs(1), c=fs(fs(2,3))))
        [WorklistItem(h=frozenset([1, 2]), c=frozenset([])), WorklistItem(h=frozenset([1]), c=frozenset([frozenset([3])]))]
        
        Rule 4:
        >>> h_iterative(WorklistItem(h=fs(1), c=fs(fs(2), fs(2,3))))
        [WorklistItem(h=frozenset([1, 2]), c=frozenset([]))]
        
        Rule 5:
        without heuristic:
        # >>> h_iterative(WorklistItem(h=fs(), c=fs(fs(1,2), fs(2,3))))
        # [WorklistItem(h=frozenset([1]), c=frozenset([frozenset([2, 3])])), WorklistItem(h=frozenset([]), c=frozenset([frozenset([2, 3]), frozenset([2])]))]
        
        with heuristic:
        >>> h_iterative(WorklistItem(h=fs(), c=fs(fs(1,2), fs(2,3))))
        [WorklistItem(h=frozenset([2]), c=frozenset([])), WorklistItem(h=frozenset([]), c=frozenset([frozenset([3]), frozenset([1])]))]
        """
        scs = item.c
        self.num_h_calls += 1
        
        # rule (1): H(F) = T
        if len(scs) == 0 or (len(scs) == 1 and len(self.first(scs)) == 0):
#            print "-> ", [WorklistItem(h=item.h, c=frozenset())]
            return [WorklistItem(h=item.h, c=frozenset())]
        
        # new: do not check any other rules on the last level as they only produce longer hitting sets
        if self.max_card and len(item.h) >= self.max_card:
#            print "-> ", []
            return []
        
        # rule (2): H(/e) = e
        cs = self.first(scs)
        if len(scs) == 1 and len(cs) == 1:
            e = self.first(cs)
#            print "-> ", [WorklistItem(h=item.h | set([e]), c=frozenset())]
            return [WorklistItem(h=item.h | set([e]), c=frozenset())]
        
        # rule (3): H(/eC) = e + H(C)
        if len(scs) == 1 and len(cs) > 1:
            e = self.first(cs)
            C = fs(cs - fs(e))
#            print "-> ", [WorklistItem(h=item.h | fs(e), c=frozenset()), WorklistItem(h=item.h, c=C)]
            return [WorklistItem(h=item.h | fs(e), c=frozenset()), WorklistItem(h=item.h, c=C)]
        
        # rule (4): H(/e + C) = e H(C1) with C1 = {all c in C such that /e not in c} 
        # NOTE: this rule has been modified from the original algorithm description where the result 
        #       of this rule was e H(C). This reduces the number of non-minimal hitting sets
        for cs in scs:
            if len(cs) == 1:
                e = self.first(cs)
                if self.options.get('old_rule4',False) == True:
                    C1 = frozenset(scs) - fs(cs)
                else:
                    C1 = frozenset([i for i in scs if e not in i])
#                print "-> ", [WorklistItem(h=item.h | fs(e), c=C1)]
                return [WorklistItem(h=item.h | fs(e), c=C1)]
        
        # rule (5): H(C) = e H(C1) + H(C2) where 
        # C1 = {all c in C such that /e not in c} (i.e. all conflict sets not containing /e), and
        # C2 = {all c such that (c U /e in C) OR (c in C and /e not in c)}, (i.e. all conflict sets 
        #       with /e removed and all conflict sets not containing /e (=C1))
#        print "-> ", self.iterative_heuristics[self.heuristic](item)

        # OPTIMIZATION: only call rule number 5 at levels _before_ max_card-1
        # at level max_card-1, the only possible solutions are those elements hitting _all_ remaining
        # conflict sets, i.e. lying in the intersection of all conflict sets

        if len(item.h) + 1 < self.max_card or self.options['stopping_optimization'] == False:
            return self.iterative_heuristics[self.heuristic](item)
        else:
            solutions = frozenset.intersection(*item.c)
            return [WorklistItem(h=item.h | fs(e), c={}) for e in solutions]
        #return self.iterative_heuristics[self.heuristic](item)
        
        
    def iterative_heuristic_0(self, item):
        # standard rule 5, use some random element (here: the first element from the first CS)
        cs = self.first(item.c)
        e = self.first(cs)
        c1 = frozenset([i for i in item.c if e not in i])
        c2 = frozenset([i - fs(e) for i in item.c])
        return [WorklistItem(h=item.h | fs(e), c=c1), WorklistItem(h=item.h, c=c2)]
    
    def iterative_heuristic_1(self, item):
        # standard rule 5, but use the most common element across all CS to split SCS
        e = self.get_most_common_element(item.c)        
        c1 = frozenset([i for i in item.c if e not in i])
        c2 = frozenset([i - fs(e) for i in item.c])
        return [WorklistItem(h=item.h | fs(e), c=c1), WorklistItem(h=item.h, c=c2)]
        
    def iterative_heuristic_2(self, item):
        # improved rule 5, work through the first CS, in each step remove an element from SCS
        # (here: d) and construct the "left" subtrees like in standard rule 5
        pivot = self.first(item.c)
        d = item.c
        result = []
        for e in pivot:
            #check if rule 4 applies:
            for di in d:
                if len(di) == 1:
                    e = self.first(di)
                    if self.options.get('old_rule4',False) == True:
                        c = d - fs(di)
                    else:
                        c = frozenset([i for i in d if e not in i]) 
                    result += [WorklistItem(h=item.h | fs(e), c=c)]
                    return result
            c = frozenset([i for i in d if e not in i])
            d = frozenset([i - fs(e) for i in d])
            result += [WorklistItem(h=item.h | fs(e), c=c)]
        return result
    
    def iterative_heuristic_3(self, item):
        # like heuristic 2 above, but work through the shortest CS
        pivot = self.get_shortest_cs(item.c)
        #print "pivot element is %s" % pivot
        d = item.c
        result = []
        for e in pivot:
            c = frozenset([i for i in d if e not in i])
            d = frozenset([i - fs(e) for i in d]) 
            result += [WorklistItem(h=item.h | fs(e), c=c)]
            #print "result:", result
        return result
    
    def iterative_heuristic_4(self, item):
        # back-port of the improvements to standard rule 5:
        # use some (here: the first) element of the _shortest_ CS to split on
        cs = self.get_shortest_cs(item.c)
        e = self.first(cs) 
        c1 = frozenset([i for i in item.c if e not in i])
        c2 = frozenset([i - fs(e) for i in item.c])
        return [WorklistItem(h=item.h | fs(e), c=c1), WorklistItem(h=item.h, c=c2)]
    
    def iterative_heuristic_5(self, item):
        # 
        pivot = list(self.get_shortest_cs(item.c))
        result = []
        d = item.c
        for e in pivot[:-1]:
            c1 = frozenset([i for i in d if e not in i])
            d = frozenset([i - fs(e) for i in d]) 
            result += [WorklistItem(h=item.h | fs(e), c=c1)]
            
        # now we have a CS with a single element remaining, rule 4 applies
        e = pivot[-1]
        if self.options.get('old_rule4',False) == True:
            c1 = frozenset(d) - fs(fs(e))
        else:
            c1 = frozenset([i for i in d if e not in i])
        result += [WorklistItem(h=item.h | fs(e), c=c1)]
        return result
    
    ################################################################################################
    ##############                  RECURSIVE ALGORITHM                 ############################
    ################################################################################################
    
    def h_recursive(self, scs, length, max_length=None):
        """
        Recursive implementation of the H(...)-function of the boolean hitting set algorithm.
        
        >>> h_recursive(set([fs(1)]), 0) 
        frozenset([frozenset([1])])
    
        >>> h_recursive(set([]), 0)
        frozenset([frozenset([])])
    
        >>> h_recursive(set([fs(1,2)]), 0)
        frozenset([frozenset([2]), frozenset([1])])
        
        >>> h_recursive(set([fs(1), fs(2,3)]), 0)
        frozenset([frozenset([1, 3]), frozenset([1, 2])])
        
        >>> h_recursive(set([fs(1, 2), fs(2,3)]), 0)
        frozenset([frozenset([1, 3]), frozenset([2])])
        
        Note: I think it is not possible to stop the recursion _exactly_ at the correct 
        length, so there might be some hitting sets that need to be filtered out afterwards.
        Example:
        >>> h_recursive(set([fs(1, 2), fs(2,3)]), 0, 1)
        frozenset([frozenset([1, 3]), frozenset([2])])
        
        >>> h_recursive(set([fs(1,2), fs(1,2,3,4)]), 0, 2)
        frozenset([frozenset([2]), frozenset([1])])
        """
        # fs(a,b,...) is a shortcut for frozenset([a, b, ...]) 
        child_node_id = None
        self.num_h_calls += 1
#        print length, ": h_recursive(%s,%d,%d)"%(write_sets(scs),length,max_length)
        
        if max_length and length > max_length:
#            print "  "*length, "-> ", fs()
            return fs()
        
        # rule (1): H(F) = T
        if len(scs) == 0 or (len(scs) == 1 and len(self.first(scs)) == 0):
#            print "  "*length, "-> ", fs(fs())
            return fs(fs())
        
        # new: do not check any other rules on the last level as they only produce longer hitting sets
        if max_length and length >= max_length:
#            print "  "*length, "-> ", fs()
            return fs()
        
        # rule (2): H(/e) = e
        cs = self.first(scs)
        if len(scs) == 1 and len(cs) == 1:
#            print "  "*length, "-> ", fs(fs(self.first(cs)))
            return fs(fs(self.first(cs)))
        
        # rule (3): H(/eC) = e + H(C)
        if len(scs) == 1 and len(cs) > 1:
            e = self.first(cs)
            c = fs(cs - fs(e))
            return fs(fs(e)) | self.h_recursive(c, length, max_length)
        
        # rule (4): H(/e + C) = e H(C1) with C1 = {all c in C such that /e not in c} 
        # NOTE: this rule has been modified from the original algorithm description where the result 
        #       of this rule was e H(C). This reduces the number of non-minimal hitting sets
        for cs in scs:
            if len(cs) == 1:
                e = self.first(cs)
                if self.options.get('old_rule4',False) == True:
                    c1 = frozenset(scs) - fs(cs)
                else:
                    c1 = frozenset([i for i in scs if e not in i]) 
                if len(c1) > 0:
                    h = self.h_recursive(c1, length+1, max_length)
                    return frozenset([i | fs(e) for i in h])
                else:
                    return frozenset([fs(e)])

        ############################### 
        # rule (5): H(C) = e H(C1) + H(C2) where 
        # C1 = {all c in C such that /e not in c} (i.e. all conflict sets not containing /e), and
        # C2 = {all c such that (c U /e in C) OR (c in C and /e not in c)}, (i.e. all conflict sets 
        #       with /e removed and all conflict sets not containing /e (=C1))
#        print "Rule (5) with ", write_sets(scs)
#        print "  "*length, "-> ", self.recursive_heuristics[self.heuristic](scs, length, max_length)

        # OPTIMIZATION: only call rule number 5 at levels _before_ max_card-1
        # at level max_card-1, the only possible solutions are those elements hitting _all_ remaining
        # conflict sets, i.e. lying in the intersection of all conflict sets
        if length + 1 < max_length or self.options['stopping_optimization'] == False:
            return self.recursive_heuristics[self.heuristic](scs, length, max_length)
        else:
            solutions = frozenset.intersection(*scs)
            return frozenset(fs(e) for e in solutions)
        
    def recursive_heuristic_0(self, scs, length, max_length):
        # standard rule 5, use some random element (here: the first element from the first CS)
        cs = self.first(scs)
        e = self.first(cs)
        c1 = frozenset([i for i in scs if e not in i])
        c2 = frozenset([i - fs(e) for i in scs])
        h1 = self.h_recursive(c1, length+1, max_length)
        h2 = self.h_recursive(c2, length, max_length)      
        return frozenset([i | fs(e) for i in h1]) | h2  
    
    def recursive_heuristic_1(self, scs, length, max_length):
        # standard rule 5, but use the most common element across all CS to split SCS
        e = self.get_most_common_element(scs)        
        c1 = frozenset([i for i in scs if e not in i])
        c2 = frozenset([i - fs(e) for i in scs])
        h1 = self.h_recursive(c1, length+1, max_length)
        h2 = self.h_recursive(c2, length, max_length)
        return frozenset([i | fs(e) for i in h1]) | h2
        
    def recursive_heuristic_2(self, scs, length, max_length):
        # improved rule 5, work through the first CS, in each step remove an element from SCS
        # (here: d) and construct the "left" subtrees like in standard rule 5
        pivot = self.first(scs)
        d = scs
        result = set()
        for e in pivot:
            #check if rule 4 applies:
            for di in d:
                if len(di) == 1:
                    e = self.first(di)
                    if self.options.get('old_rule4',False) == True:
                        c = d - fs(di)
                    else:
                        c = frozenset([i for i in d if e not in i]) 
                    if len(c) > 0:
                        h = self.h_recursive(c, length, max_length)
                        result |= frozenset([i | fs(e) for i in h])
                    else:
                        result |= frozenset([fs(e)])
                    return frozenset(result)
            
            c = frozenset([i for i in d if e not in i])
            d = frozenset([i - fs(e) for i in d])
            h = self.h_recursive(c, length+1, max_length)
            result |= frozenset([i | fs(e) for i in h])
        return frozenset(result)
    
    def recursive_heuristic_3(self, scs, length, max_length):
        # like heuristic 2 above, but work through the shortest CS
        pivot = self.get_shortest_cs(scs)
        d = scs
        result = set()
        for e in pivot:
            c = frozenset([i for i in d if e not in i])
            d = frozenset([i - fs(e) for i in d]) 
            h = self.h_recursive(c, length+1, max_length)
            result |= frozenset([i | fs(e) for i in h])
        return frozenset(result)
    
    def recursive_heuristic_4(self, scs, length, max_length):
        # back-port of the improvements to standard rule 5:
        # use some (here: the first) element of the _shortest_ CS to split on
        cs = self.get_shortest_cs(scs)
        e = self.first(cs) 
        c1 = frozenset([i for i in scs if e not in i])
        c2 = frozenset([i - fs(e) for i in scs])
        h1 = self.h_recursive(c1, length+1, max_length)
        h2 = self.h_recursive(c2, length, max_length)
        return frozenset([i | fs(e) for i in h1]) | h2
    
    def recursive_heuristic_5(self, scs, length, max_length):
        pivot = list(self.get_shortest_cs(scs))
        result = set()
        d = scs
        for e in pivot[:-1]:
            c1 = frozenset([i for i in d if e not in i])
            d = frozenset([i - fs(e) for i in d])
            h1 = self.h_recursive(c1, length+1, max_length)
            result |= frozenset([i | fs(e) for i in h1]) 
            
        # now we have a CS with a single element remaining, rule 4 applies
        e = pivot[-1]
        if self.options.get('old_rule4',False) == True:
            c1 = frozenset(d) - fs(fs(e))
        else:
            c1 = frozenset([i for i in d if e not in i])
        if len(c1) > 0:
            h = self.h_recursive(c1, length+1, max_length)
            result |= frozenset([i | fs(e) for i in h])
        else:
            result |= frozenset([fs(e)])
        return result


if __name__ == "__main__":
    import doctest
    doctest.testmod()
    
