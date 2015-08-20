
generated_nodes = 0

class Node(object):
    """
    Represents a node in the HsGraph of HS-DAG
    """
    
    next_node_number = 0

    __slots__ = ['number', '_set', 'index', 'max', '_state', '_h', 'tree', 'level']

    class State: 
        OPEN = 1
        CLOSED = 2
        CHECKED = 3
    
    def __init__(self, level, set, index, max):
        self.number = Node.next_node_number
        Node.next_node_number += 1
        self._set = set
        self.index = index
        self.max = max
        self._state = Node.State.OPEN
        self._h = None
        self.tree = None
        self.level = level
        global generated_nodes
        generated_nodes += 1

    def __repr__(self):
        return "n" + str(self.number)
        
    def label(self):
        return "n" + str(self.number) + ":" + self.short_label()

        
    def short_label(self):
        label = ""
        sep = ""
        if self.set is not None:
            label += "{"
            for el in self.set:
                label += sep + str(el)
                sep = ","
            label += "}"
        else:
            label += "-"
        label += "/" + str(self.index) + '/' + str(self.max)
        return label
        
    def short_text(self):
        label = self.short_label()
        if self.state == Node.State.OPEN:
            pass
        elif self.state == Node.State.CLOSED:
            label += "(X)"
        elif self.state == Node.State.CHECKED:
            label += "(V)"
        return label
    
    def get_state(self):
        return self._state
    
    def set_state(self, state):
        if self.tree:
            if self._state != Node.State.CHECKED and state == Node.State.CHECKED:
                n = len(self.h)
                if n in self.tree.checked_nodes:
                    self.tree.checked_nodes[n].add(self)
                else:
                    self.tree.checked_nodes[n] = set([self]) 
            if self._state == Node.State.CHECKED and state != Node.State.CHECKED:
                n = len(self.h)
                if n in self.tree.checked_nodes:
                    self.tree.checked_nodes[n].remove(self)
                else:
                    # something's wrong...
                    pass # fail?
            if self._state != Node.State.CLOSED and state == Node.State.CLOSED:
                self.tree.closed_nodes.add(self)
            if self._state == Node.State.CLOSED and state != Node.State.CLOSED:
                self.tree.closed_nodes.remove(self)
        self._state = state        
    
    state = property(get_state, set_state)

    def get_h(self):
        return self._h

    def set_h(self, h):
        if self._h in self.tree.h_cache:
            del self.tree.h_cache[self._h]
        self._h = h
        self.tree.h_cache[h] = self
        
    h = property(get_h, set_h)
    
    def get_set(self):
        return self._set
    
    def set_set(self, new_set):
        if self.tree:
            if self._set in self.tree.set_cache:
                self.tree.set_cache[self._set].remove(self)
                if len(self.tree.set_cache[self._set]) == 0:
                    del self.tree.set_cache[self._set]
            
        self._set = new_set
        
        if self.tree:
            if new_set not in self.tree.set_cache:
                self.tree.set_cache[new_set] = set()
            self.tree.set_cache[new_set].add(self)
        
    
    set = property(get_set, set_set)
        
        