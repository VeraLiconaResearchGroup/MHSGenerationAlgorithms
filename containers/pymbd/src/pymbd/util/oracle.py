from ..util.sethelper import fs
import time

class Oracle(object):
    """
    For testing the hitting set algorithm, this plays the role of the oracle 
    knowing all conflict sets and returning for a given set h a new conflict 
    set s such that h & s == {} if such a set s exists, or None otherwise.
    
    >>> Oracle(fs(1, 2), fs(1, 3), fs(2, 3)).get_conflict_set(fs(3))
    frozenset([1, 2])
    
    >>> Oracle(fs(1, 2), fs(1, 3), fs(2, 4)).get_conflict_set(fs(2))
    frozenset([1, 3])

    >>> Oracle(fs(1, 2), fs(1, 3), fs(2, 4)).get_conflict_set(fs(1,2))
    
    New: one can also use the h_include argument to obtain sets that 
    include certain elements, e.g.
    >>> Oracle(fs(1,2),fs(1,3),fs(1,2,4),fs(3,4)).get_conflict_set(fs(3), fs(4))
    frozenset([1, 2, 4])
    """
    
    def __init__(self, *sets):
        sort = True
        if(sort):
            self.sets = sorted(sets, key=len)
        else:
            self.sets = sets
        self.comp_calls = 0
        self.check_calls = 0
        self.comp_time = 0
        self.check_time = 0
        self.components = sorted(list(set().union(*sets)))

    def get_first_conflict_set(self, additional_info=None):
        return self.get_conflict_set(set())
    
    def check_consistency(self, h, additional_info=None):
        t0 = time.time()
        self.check_calls += 1
        for s in self.sets:
            if len(s & h) == 0:
                t1 = time.time()
                self.check_time += t1-t0
                return False
        t1 = time.time()
        self.check_time += t1-t0
        return True

    def get_conflict_set(self, h, additional_info=None):
        t0 = time.time()
        self.comp_calls += 1
        for s in self.sets:
            if len(s & h) == 0:
                t1 = time.time()
                self.comp_time += t1-t0
                return s
        t1 = time.time()
        self.comp_time += t1-t0
        return None
    
    def get_num_components(self):
        return max(self.components)+1
    
if __name__ == "__main__":
    import doctest
    doctest.testmod()
    
