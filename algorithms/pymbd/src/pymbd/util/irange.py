def irange(*args):
    """
    A range function where start and stop is inclusive, e.g.:
    
    >>> irange(5)
    [1, 2, 3, 4, 5]
    
    >>> irange(2, 7)
    [2, 3, 4, 5, 6, 7]
    
    >>> irange(7, 2)
    []
    
    >>> irange(10, 5, -1)
    [10, 9, 8, 7, 6, 5]
        
    >>> irange(9, 5, -2)
    [9, 7, 5]
    
    >>> irange(10, 15, 3)
    [10, 13]

    
    """
    
    if len(args) == 1:
        start = 1
        stop = args[0]
        step = 1
    elif len(args) == 2:
        (start, stop) = args
        step = 1
    elif len(args) == 3:
        (start, stop, step) = args
    else:
        raise TypeError("Expected 1-3 args, got", len(args))
    if step > 0:
        return range(start, stop + 1, step)
    else:
        return range(start, stop - 1, step)

if __name__ == "__main__":
    import doctest
    doctest.testmod()
    