from collections import defaultdict
from math import ceil

def powerset(s):
    """
    Returns all the subsets of this set. This is a generator.
    From http://www.technomancy.org/python/powerset-generator-python/
    
    >>> print [x for x in powerset(fs(1,2,3))]
    [frozenset([1, 2, 3]), frozenset([2, 3]), frozenset([1, 3]), frozenset([3]), \
frozenset([1, 2]), frozenset([2]), frozenset([1]), frozenset([])]
    
    """
    if len(s) <= 1:
        yield frozenset(s)
        yield frozenset()
    else:
        ss = set(s)
        i = ss.pop()
        for item in powerset(ss):
            yield frozenset([i]) | item
            yield item

def fs(*elems):
    """
    Creates a frozeset from the given arguments, e.g.

    >>> fs(1,2,3)
    frozenset([1, 2, 3])

    This is just a shortcut.
    """
    return frozenset(elems)
        
        
def write_sets(object):
    """
    >>> write_sets(set([frozenset([1,2,3]), frozenset(['a', 'b', 'c'])]))
    '{{a,c,b},{1,2,3}}'
    
    >>> write_sets([frozenset([1,2,3]), frozenset(['a','b','c'])])
    '[{1,2,3},{a,c,b}]'
    """
    
    if isinstance(object, set) or isinstance(object, frozenset):
        return '{' + ','.join([write_sets(x) for x in object]) + '}'
    if isinstance(object, list):
        return '[' + ','.join([write_sets(x) for x in object]) + ']'
    else:
        return str(object)
    
def write_sets_java(object):
    """
    >>> write_sets_java(set([frozenset([1,2,3]), frozenset(['a', 'b', 'c'])]))
    '[[na,nc,nb],[n1,n2,n3]]'
    
    >>> write_sets_java([frozenset([1,2,3]), frozenset(['a','b','c'])])
    '[[n1,n2,n3],[na,nc,nb]]'
    """
    
    if isinstance(object, set) or isinstance(object, frozenset):
        return '[' + ','.join([write_sets_java(x) for x in object]) + ']'
    if isinstance(object, list):
        return '[' + ','.join([write_sets_java(x) for x in object]) + ']'
    else:
        return "n" + str(object)

def read_sets(string):
    """
    >>> read_sets("{}")
    frozenset([])
    
    >>> read_sets("{1}")
    frozenset([1])
    
    >>> read_sets("{{}, {}}")   # invalid, outer set contains two equal sets
    frozenset([frozenset([])])
    
    >>> read_sets("{{{1}, {2}}, {3}}")
    frozenset([frozenset([frozenset([2]), frozenset([1])]), frozenset([3])])
    
    >>> read_sets("{1, 2,3}")
    frozenset([1, 2, 3])
    
    >>> read_sets("{{1, 2}, {3, 4}}")
    frozenset([frozenset([1, 2]), frozenset([3, 4])])
    
    >>> read_sets("{a,b,c}")
    frozenset(['a', 'c', 'b'])
    
    >>> read_sets('[{1,2,3},{a,c,b}]')
    [frozenset([1, 2, 3]), frozenset(['a', 'c', 'b'])]
    
    >>> read_sets('{a}')
    frozenset(['a'])
    
    >>> read_sets('{{x1,x2},{x3}}')
    frozenset([frozenset(['x2', 'x1']), frozenset(['x3'])])
    
    >>> read_sets('{{23gat,24gat}}')
    frozenset([frozenset(['23gat', '24gat'])])
    """
    from pyparsing import nestedExpr, alphas, Word, nums, ParserElement, delimitedList
    ParserElement.setDefaultWhitespaceChars(" ,")
    element = Word(alphas + nums).setParseAction(parse_elem) 
    elements = delimitedList(element)
    setofsets = nestedExpr("{", "}", content=elements).setParseAction(lambda x: frozenset(x[0]))
    listofsets = nestedExpr("[", "]", content=setofsets)
    expr = setofsets | listofsets
    return expr.parseString(string).asList()[0]

def read_sets_java(string):
    from pyparsing import nestedExpr, alphas, Word, nums, ParserElement, delimitedList
    ParserElement.setDefaultWhitespaceChars(" ,")
    element = Word(alphas + nums).setParseAction(parse_elem_java) 
    elements = delimitedList(element)
    setofsets = nestedExpr("[", "]", content=elements).setParseAction(lambda x: frozenset(x[0]))
    return setofsets.parseString(string).asList()[0]

def parse_elem(x):
    try:
        result = int(x[0])
        return result
    except ValueError:
        return eval("'" + x[0] + "'")

def parse_elem_java(x):
    try:
        result = int(x[0][1:])
        return result
    except ValueError:
        return eval("'" + x[0][1:] + "'")   

def encode_sets(setOfSets):
    """
    Encode a set of sets (e.g., a set of conflict sets or a set of hitting sets) in a binary format, where each 
    of the sets is represented as a word whose bits indicate which elements are part of the set. 
    The first 64bit unsigned big-endian integer indicates the number of bytes used per set. 
    
    For example, the set of sets {{a,c},{a,b}} contains three components: [a, b, c]. 
    As we only use full bytes (i.e. 8 bits, 16 bits, ...), we use 1 byte to encode each set in this example. 
    Therefore, the first 64 bit of the encoded value are 0x0000000000000001. 
    
    The set {a,c} contains components number 1 and 3:
    a = 0b001
    c = 0b100
    ---------
      = 0b101 = 0x5 = 0x05
    
    The set {a,b} contains components number 1 and 2:
    a = 0b001
    b = 0b010
    ---------
      = 0b011 = 0x3 = 0x03
    
    Thus, the encoded value of {{a,c},{a,b}} is 0x00000000000000010503.
    
    Examples:
    
    >>> b = encode_sets(read_sets('{{a,c},{a,b}}')); BitStream(bytes=b).hex
    '0x00000000000000010503'
    
    >>> b = encode_sets(read_sets('{{0,2},{0,1}}')); BitStream(bytes=b).hex
    '0x00000000000000010503'
    
    Corner case: setOfSets contains just one empty set:
    >>> b = encode_sets(frozenset([frozenset([])])); BitStream(bytes=b).hex
    '0x000000000000000100'
    
    Corner case: setOfSets is empty itself:
    >>> b = encode_sets(frozenset([])); BitStream(bytes=b).hex
    '0x0000000000000001'
    """
    from bitstring.bitstream import BitStream, pack
    comp = sorted(list(set().union(*setOfSets)))
    bytesperset = max(1,int(ceil(float(len(comp))/8)))
    bs = BitStream()
    bs.append(pack('uint:64', bytesperset))
    for aSet in setOfSets:
        result = 0
        for element in aSet:
            result |= 2**(comp.index(element))
        bs.append(pack('uint:%d' % (bytesperset*8), result))
    return bs.tobytes()

def decode_sets(bytes):
    """
    Decodes a set of sets encoded using encode_sets (see above). 
    
    >>> b = BitStream(hex='0x00000000000000010503').tobytes(); write_sets(decode_sets(b))
    '{{0,2},{0,1}}'
    
    >>> b = BitStream(hex='0x000000000000000100').tobytes(); write_sets(decode_sets(b))
    '{{}}'
    
    >>> b = BitStream(hex='0x0000000000000001').tobytes(); write_sets(decode_sets(b))
    '{}'
    """
    from bitstring.bitstream import BitStream, pack
    bs = BitStream(bytes=bytes)
    bytesperset, content = bs.readlist('uint:64, bits')
    setOfSets = set()
    for bits in content.cut(bytesperset*8):
        aSet = set()
        bits.reverse()
        for i,b in enumerate(bits):
            if b is True:
                aSet.add(i)
        setOfSets.add(frozenset(aSet))
    return setOfSets

def relabel_sets(setOfSets):
    """
    Relabels sets of sets so that they consist of elements made from a continuous sequence of integers, starting from 0.
    
    >>> write_sets(relabel_sets(read_sets('{{1,4},{1,3}}')))
    '{{0,2},{0,1}}'
    
    >>> write_sets(relabel_sets(read_sets('{{a,b},{a,c}}')))
    '{{0,2},{0,1}}'
    """
    comp = sorted(list(set().union(*setOfSets)))
    mapping = dict([(c, i) for i, c in enumerate(comp)])
    newSetOfSets = set()
    for aSet in setOfSets:
        newSetOfSets.add(frozenset(map(lambda x: mapping[x], aSet)))
    return newSetOfSets


def to_binary(setOfSets):
    comp = sorted(list(set().union(*setOfSets)))
    print comp
    result = ""
    for s in setOfSets:
        line = ""
        for c in comp:
            if c in s:
                line += "1"
            else:
                line += "0"
        result += line[::-1] + "\n"
    return result

def custom_hash(setOfSetsOfNumbers):
    """
    A hash function to create an integer number from a set of conflict/hitting sets consisting of integer (0...M) elements. 
    This function is built such that two SCS/SHS comparing equal using "==" also return the same custom_hash value. 
    Therefore, XOR function is used together with some large (prime) numbers such that the order of elements does not matter. 
    
    >>> custom_hash([fs(1),fs(3,2),fs(0,1,2),fs(2,0,3),fs(2,4)]) == custom_hash([fs(0,1,2),fs(2,0,3),fs(2,4),fs(1),fs(3,2)])
    True
    
    """
    hash = 1927868237
    for setOfNumbers in setOfSetsOfNumbers:
        h = 0
        for num in setOfNumbers:
            h = h ^ num
        hash ^= (h ^ (h << 16) ^ 89869747)  * 3644798167;
    hash = hash * 69069 + 907133923;
    return hash
        
def sum_all(setOfSetsOfNumbers):
    """
    A function to create an integer number from a set of conflict/hitting sets consisting of integer (0...M) elements 
    by just summing up all elements. 
    
    >>> sum_all([fs(1),fs(3,2),fs(0,1,2),fs(2,0,3),fs(2,4)]) == sum_all([fs(0,1,2),fs(2,0,3),fs(2,4),fs(1),fs(3,2)])
    True
    
    NOTE THAT THIS FUNCTION MAY RETURN THE SAME SUM FOR DIFFERENT SETS! USE ONLY IF THIS IS NO PROBLEM FOR YOU!!!
    
    >>> sum_all([fs(1),fs(3,2)]) != sum_all([fs(2),fs(3,1)])
    False
    
    """
    return sum(map(sum, setOfSetsOfNumbers))

def minimize_sets(set_of_sets):
    """
    removes all non-subset-minimal sets from set_of_sets
    
    >>> write_sets(minimize_sets(read_sets('{{1,2,3,4,5},{1,2},{4},{1,4,6}}')))
    '{{1,2},{4}}'

    >>> write_sets(minimize_sets(read_sets('{{1,2,3},{1,2,3},{4,5,6},{4,5},{1,2,3,4,5,6}}')))
    '{{1,2,3},{4,5}}'

    >>> write_sets(minimize_sets(read_sets('{{1}}')))
    '{{1}}'
    
    >>> write_sets(minimize_sets(read_sets('{{3},{4},{5},{6},{7},{8},{9},{10},{3,5,8,9,10,12,13,14},{1,4,5,6,8,9,11,12,13,14},{0,12,5,6,13},{0,2,6,7,8,9,11,12,14},{0,1,3,4,5,9,10,11,14},{0,9,6,7},{0,1,2,3,4,6,7,10,11,14},{3,5,6,7,9,12,13,14},{0,1,2,3,4,7,8,13},{1,3,4,6,7,10,11,13,14}}')))
    '{{7},{5},{10},{3},{6},{8},{4},{9}}'
    
    >>> write_sets(minimize_sets(read_sets('{{0},{1,2},{0,2},{1},{0,1,2},{2},{0,1}}')))
    '{{2},{0},{1}}'

    """
    scs = defaultdict(set)
    for cs in set_of_sets:
        scs[len(cs)].add(cs)
    supersets = set()
    for l in scs:
        for csi in scs[l]:
            for m in filter(lambda x: x > l, scs.keys()):
                supersets = set()
                for csj in scs[m]:
                    if csi <= csj:
                        supersets.add(csj)
                scs[m] -= supersets
                
    new_set_of_sets = set()
    for scsi in scs.values():
        new_set_of_sets |= scsi
    return new_set_of_sets

if __name__ == "__main__":
    import doctest
    doctest.testmod()

