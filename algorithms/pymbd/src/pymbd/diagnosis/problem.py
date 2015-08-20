from description import Description
from engine import ENGINES
from ..util.sethelper import read_sets
import random

class Problem(object):
    '''
    Represents a whole hitting set computation problem and works as a facade for easier use of 
    the python modules involved. 
    '''

    def __init__(self):
        self.compute_called = False
    
    def compute_from_string(self, scs_string, engine_name, **options):
        '''
        compute hitting sets to the given set of conflict sets (as string, e.g. '{{1,2,3},{4,2}}')
        with engine_name and options
        '''
        return self.compute(read_sets(scs_string), engine_name, **options)
    
    def compute(self, scs, engine_name, **options):
        '''
        compute hitting sets to the given set of conflict sets with engine_name and options
        '''
        options['sort_conflict_sets'] = options.get('sort_conflict_sets', True)
        return self.compute_with_description(Description(scs, sort=options['sort_conflict_sets']), engine_name, **options)
    
    def compute_with_description(self, description, engine_name, **options):
        '''
        compute hitting sets using the given system description with engine_name and options
        '''
        randstate = random.getstate()
        self.compute_called = True
        self.description = description
        if engine_name not in ENGINES:
            raise Exception("Unknown Hitting Set Engine: %s" % engine_name)
        self.engine = ENGINES[engine_name](self.description, options)
        self.engine.start()
        self.result = self.engine.get_result()
        random.setstate(randstate)
        return self.result
        
    def compute_more(self, **options):
        '''
        compute hitting sets again with new options
        '''
        randstate = random.getstate()
        if not self.compute_called:
            raise Exception("compute_more must not be called before compute/compute_with_description")
        
        self.engine.set_options(options)
        self.engine.start()
        self.result = self.engine.get_result()
        random.setstate(randstate)
        return self.result

