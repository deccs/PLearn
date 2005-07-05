__version_id__ = "$Id$"

from toolkit                     import quote_if, doc

def c_iterator( container, itype="iteritems" ):
    if hasattr(container, itype ):
        return container.iteritems()
    return iter(container)

class Bindings:
    """Acts like a Python dictionary but keeps the addition order."""
    def __init__(self, container=[], value=None):
        self.ordered_keys  = []
        self.internal_dict = {}

        iterator = c_iterator( container )                    
        for item in iterator:
            pair = None

            ## Key
            if len(item) == 1:
                pair = (item, value)

            ## Pair
            elif len(item) == 2:
                pair = item

            else:
                raise ValueError( "The Bindings constructor should be provided "
                                  "a list of keys or pairs or a mapping "
                                  "object supporting iteritems."
                                  )
            
            self.__setitem__( *pair )

    ## Emulating containers ##########################################    
    def __len__(self):
        return len(self.ordered_keys)

    def __contains__(self, key):
        return key in self.ordered_keys
    
    def __setitem__(self, key, value):
        self.internal_dict[key] = value
        if not key in self.ordered_keys:
            self.ordered_keys.append(key)

    def __getitem__( self, key ):
        return self.internal_dict[key]

    def __delitem__(self, key):
        self.pop(key)

    def __str__(self):
        if len(self) == 0:
            return "{}"

        s = None        
        for (key, val) in self.iteritems( ):
            if s is None:
                s = "{%s: %s"
            else:
                s += ", %s: %s"
            s = s % (quote_if(key), quote_if(val))
        s += "}"
        return s        

    def __repr__( self ):
        return "%s([ %s ])" % ( self.__class__.__name__,
                                ', '.join([ str(item) for item in self.items() ])
                                )

    ## Iterators  ####################################################
    class item_iterator:
        def __init__( self, bindings, return_pairs=True ):
            self.bindings     = bindings
            self.return_pairs = return_pairs

            self.cur      = -1
            self.keys     = bindings.ordered_keys

        def __iter__(self):
            return self

        def next(self):
            self.cur += 1
            if len(self.bindings) <= self.cur:
                raise StopIteration

            curkey = self.keys[self.cur]
            curval = self.bindings[curkey] 

            if self.return_pairs:
                return (curkey, curval)
            else:
                return curval
        
    def __iter__(self):
        return self.iterkeys()

    def iteritems(self):
        return Bindings.item_iterator( self )

    def iterkeys(self):
        return iter(self.ordered_keys)
            
    def itervalues(self):
        return Bindings.item_iterator( self, False )

    ## Iterator related methods ######################################
    def items( self ):
        return [ item for item in self.iteritems() ]

    def keys( self ):
        return [ k for k in self.iterkeys( ) ]

    def values( self ):
        return [ value for value in self.itervalues() ]

    ## Other dictionnary methods #####################################
    def clear( self ):
        del self.ordered_keys[:]
        self.internal_dict.clear()

    def copy( self ):
        return Bindings( self )
        
    def fromkeys( cls, container, value=None ):               
        return cls( c_iterator( container, "iterkeys" ), value )               
    fromkeys = classmethod( fromkeys )

    def get( self, key, x=None ):
        if key in self:
            return self.__getitem__( key )
        return x

    def has_key( self, key ):
        return key in self.ordered_keys

    def pop( self, key, x=None ):
        if key in self:
            self.ordered_keys.remove(key)
            return self.internal_dict.pop(key)

        if x is None:
            raise KeyError("Binding object does not contain any '%s' key.") 
        return x
        
    def popitem( self ):
        (key, val) = self.internal_dict.popitem(key)
        self.ordered_keys.remove(key)
        return (key, val)

    def setdefault( self, k, x=None ):
        if key in self:
            return self.__getitem__( key )
        self.__setitem__( k, x )
        return x        
        
    def update( self, pairs ):
        try:
            # Dictionnary
            iterator = pairs.iteritems()
        except AttributeError:
            # List of pairs
            iterator = iter( pairs )
            
        for k, val in iterator:
            self.__setitem__( k, val )

if __name__ == "__main__":
    print "\nEmbedded test/tutorial for Bindings.py.\n"

    print "Bindings: "
    print doc(Bindings)
    print 
    
    def print_bindings(b):
        print "\nBindings: %s" % b

        print "Keys: ",
        for bkey in b.iterkeys():
            print bkey,
        print

        print "Values: ",
        for bval in b.itervalues():
            print quote_if(bval),
        print

        print "Items: ",
        for bitem in b.iteritems():
            print bitem,
        print

        print "\n"

    bind = Bindings( [(1, "a"), (2, "b"), (3, "c")] )
    print 'After\n    bind = Bindings( [(1, "a"), (2, "b"), (3, "c")] )'
    print_bindings(bind)
    
    bind[4] = "d"
    print 'Then doing\n   bind[4] = "d"' 
    print_bindings(bind)

    bind[3] = "*C*"
    print 'Note that binding an existing argument keeps the original order of keys, i.e. after'
    print 'bind[3] = "*C*"'    
    print_bindings(bind)

    del bind[3]
    bind[3] = "newC"
    print "Therefore, to change the order, one should do"
    print '    del bind[3]'
    print '    bind[3] = "newC"'
    print_bindings(bind)

    
    
