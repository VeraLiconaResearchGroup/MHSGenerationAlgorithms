import time

class Description(object):
    '''
    Implements the Input description of a Hitting Set Problem. This implementation can be used 
    if the set of conflict sets is known in advance. Any derived class must implement 
    get_conflict_set, get_components, get_num_components, get_num_calls and get_total_time 
    accordingly.
    '''

    def __init__(self, scs, **options):
        '''
        Constructs a description object from the set of conflict sets (must be a set of frozensets)
        If sort=True, the conflict sets are stored sorted in ascending length internally (and thus 
        shorter conflict sets are returned first by get_conflict_set).
        '''
        self.comp_calls = 0
        self.check_calls = 0
        self.comp_time = 0
        self.check_time = 0
        self.scsSorted = options.get('sort', True)
        if self.scsSorted:
            self.scs = sorted(scs, key=len)
        else:
            self.scs = list(scs)
        self.components = sorted(list(set().union(*scs)))
        self.options = options
        
    def set_options(self, **options):
        self.options.update(options)

    def get_first_conflict_set(self):
        return self.get_conflict_set(set())
    
    def check_consistency(self, h):
        t0 = time.time()
        self.check_calls += 1
#        print "PY: Description: check_consistency(%s) " % str(h)
        for s in self.scs:
            if len(s & h) == 0:
                t1 = time.time()
                self.check_time += t1-t0
                return False
        t1 = time.time()
        self.check_time += t1-t0
        return True

    def get_conflict_set(self, h):
        '''
        Returns a conflict set from scs which has no intersection with the set h. if no such set is 
        found, returns None
        '''
        t0 = time.time()
        self.comp_calls += 1
        for s in self.scs:
            if len(s & h) == 0:
                t1 = time.time()
                self.comp_time += t1-t0
                return s
        t1 = time.time()
        self.comp_time += t1-t0
        return None    
    
    def get_components(self):
        return self.components

    def get_num_components(self):
        return max(self.components)+1
    
    def get_num_calls(self):
        return self.check_calls + self.comp_calls
    
    def get_total_time(self):
        return self.check_time + self.comp_time
    
    def get_conflict_sets(self):
        return self.scs
    
    def get_comp_time(self):
        return self.comp_time
    
    def get_check_time(self):
        return self.check_time
    
    def get_check_calls(self):
        return self.check_calls
    
    def get_comp_calls(self):
        return self.comp_calls
    
    sets = property(get_conflict_sets)
    
    
