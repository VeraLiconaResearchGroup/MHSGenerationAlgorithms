

def makestruct(name, fields):
    """
    creates a struct-like object with efficient named access to members, e.g.
    >>> Info = makestruct("Info", "a b c d e f")
    >>> i = Info(a=1, b=2, c=3)
    >>> i.a = 5
    >>> print i.a
    5
    
    uninitialized members are set to None, indexing can also be used but is slow, 
    e.g.
    >>> print i[0]
    5
    
    >>> Info(a=1) == Info(a=1)
    True
    
    adopted from http://stackoverflow.com/questions/2646157/what-is-the-fastest-to-access-struct-like-object-in-python/2648186#2648186
    """
    fields = fields.split()
    template="""\
class {name}(object):
    __slots__ = {fields!r}
    def __init__(self, **args): 
        {self_fields} = {get_fields}
    def __getitem__(self, idx): 
        return getattr(self, fields[idx])
    def __repr__(self):
        return "{name}(" + ", ".join(map(lambda f: str(f) + '=' + getattr(self, f).__repr__(), fields)) + ")"
    def __cmp__(self, other):
        return sum(map(lambda f: 2**f[0] * cmp(getattr(self, f[1]), getattr(other, f[1])), enumerate(fields)))
    def dict(self):
        return dict(map(lambda f: (str(f),getattr(self, f)), fields))
""".format(name=name, fields=fields, 
       args=','.join(fields), 
       self_fields=', '.join('self.'+f for f in fields), 
       get_fields=', '.join('args.get("' + f + '", None)' for f in fields))
    d = {'fields':fields}
#    print template
    exec template in d
    return d[name]

#Info = makestruct("Info", "a b c d e f")
#i = Info(a=1, b=2, c=3)
#print i.dict()
#print Info(**i.dict())
# i.a = 5
# 
# j = [1, 2, 3, 4, 5, 6]
# key = 5

# import timeit
# print timeit.timeit('i.f', "from __main__ import i")
# print timeit.timeit('j[5]', "from __main__ import j")


if __name__ == "__main__":
    import doctest
    doctest.testmod()
