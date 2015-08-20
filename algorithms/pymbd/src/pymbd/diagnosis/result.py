

class Result(object):
    '''
    Encapsules the result of a hitting set computation. This implementation stores
    a set of hitting sets (= set of frozensets). Any derived class must implement
    get_diagnoses() and get_stats() accordingly. 
    '''


    def __init__(self, shs, **stats):
        '''
        Constructor
        '''
        self.shs = shs
        self.stats = stats
        
        
    def get_diagnoses(self):
        return self.shs
    
    def get_stats(self):
        return self.stats