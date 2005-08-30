// -*- C++ -*-

// ObjectGraphIterator.h
//
// Copyright (C) 2005 Nicolas Chapados 
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
// 
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
// 
//  3. The name of the authors may not be used to endorse or promote
//     products derived from this software without specific prior written
//     permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
// NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// This file is part of the PLearn library. For more information on the PLearn
// library, go to the PLearn Web site at www.plearn.org

/* *******************************************************      
   * $Id: .pyskeleton_header 544 2003-09-01 00:05:31Z plearner $ 
   ******************************************************* */

// Authors: Nicolas Chapados

/*! \file ObjectGraphIterator.h */


#ifndef ObjectGraphIterator_INC
#define ObjectGraphIterator_INC

// From C++ stdlib
#include <string>
#include <vector>
#include <utility>

// From boost
#include <boost/call_traits.hpp>

// From PLearn
#include <plearn/base/OptionBase.h>


namespace PLearn {

// Forward-declare
class Object;


//######################  CLASS  OBJECTOPTIONSITERATOR  #######################

/**
 *  An ObjectOptionsIterator iterates across all accessible sub-objects of a
 *  given \c PLearn::Object.  The iteration is carried out by going through the
 *  list of options within the object, and traversing vectors (and other
 *  indexable options, such as maps, eventually) as appropriate.
 *
 *  The iterator is a model of the STL ForwardIterator.  Default-constructing
 *  this object makes an "invalid" iterator which is always considered the end.
 */
class ObjectOptionsIterator
{
public:
    //! Default constructor: invalid iterator
    ObjectOptionsIterator();

    //! Iterate along all sub-objects of the given object
    ObjectOptionsIterator(const Object* root);

    // Default assignment, copy constructor, destructor

    //! Equality testing
    bool operator==(const ObjectOptionsIterator& rhs) const;

    //! Inequality testing
    bool operator!=(const ObjectOptionsIterator& rhs) const
    {
        return ! (*this == rhs);
    }

    //! Dereference: access the current sub-object.  Note that if
    //! this object MAY be null if the option is indeed null.
    const Object* operator*() const;

    /**
     *  Return the name of the current option such that code
     *  of the following form always succeeds:
     *
     *  @code
     *  root->getOption(current_option_name)
     *  @endcode
     */
    string getCurrentOptionName() const;
    
    // Go to next object or "invalid" state if end of iteration
    const ObjectOptionsIterator& operator++();

    //! Post-increment: implemented in terms of pre-increment.
    //! Note that this is expensive; don't use if not necessary.
    ObjectOptionsIterator operator++(int)
    {
        ObjectOptionsIterator old(*this);
        ++(*this);
        return old;
    }

protected:
    bool m_invalid;                          //!< If true: invalid state
    const Object* m_object;                  //!< Object we are pointing to
    const OptionList* m_options;             //!< OptionList of that object
    int m_cur_option;                        //!< Which option we're at
    int m_cur_index;                         //!< If cur_option is indexable,
                                             //!<   current index within option
    int m_max_index;                         //!< If cur_option is indexable,
                                             //!<   maximum possible inxex+1
};


//#######################  CLASS  OBJECTGRAPHITERATOR  ########################

/**
 *  An ObjectGraphIterator iterates through all objects through options.
 *  From a starting "root" object, the iterator returns pointers to each
 *  encountered object in the graph that is accessible through options.  Since
 *  this is "real" graph traversal, the same object is not returned twice.
 *
 *  The following options are available:
 *
 *  - Either breadth-first or depth-first traversal is supported.  For
 *    depth-first, only pre-order traversal is supported so far.  Note that the
 *    "root" node is visited just like any other node.
 *
 *  - We can filter the returned objects by base-class type; in other words,
 *    the iterator can be forced to return only objects that inherit from a
 *    given base class.
 *
 *  The current implementation starts out by building a traversal list which
 *  records the set of all objects in the graph in the order in which they
 *  should be returned.  Then the iteration per se is simply an STL-like
 *  forward iterator that performs the appropriate filterings.
 *
 *  The iterator skips ALL NULL POINTERS in unused options.
 */
class ObjectGraphIterator
{
public:
    //! Specify the kinds of supported traversals
    enum TraversalType {
        BreadthOrder  = 1,
        DepthPreOrder = 2
    };

public:
    //! Default constructor makes an "invalid" iterator
    ObjectGraphIterator();
    
    /**
     *  Usual constructor to iterate over an object graph.
     *
     *  @param root               Starting object from which to start the iteration
     *  @param tt                 Type of traversal, e.g. \c BreadthOrder
     *                            or \c DepthPreOrder
     *  @param compute_optnames   Whether the function getCurrentOptionName()
     *                            returns something meaningful; this slows
     *                            down computations a bit
     *  @param base_class_filter  Name of base class to filter results; it is
     *                            guaranteed that the returned objects will be
     *                            of a class derived from this one
     */
    ObjectGraphIterator(const Object* root, TraversalType tt = DepthPreOrder,
                        bool compute_optnames = false,
                        const std::string& base_class_filter = "");
    
    // Default assignment, copy constructor, destructor

    //! Return true if the iterator is in an invalid state;
    bool invalid() const
    {
        return m_it == m_end;
    }
    
    //! Equality testing; two invalid iterators always compare equal
    bool operator==(const ObjectGraphIterator& rhs) const;

    //! Inequality testing
    bool operator!=(const ObjectGraphIterator& rhs) const
    {
        return ! (*this == rhs);
    }

    //! Dereference: access the current sub-object.  Note that this never
    //! returns a null pointer.
    const Object* operator*() const
    {
        assert( !invalid() );
        return m_it->first;
    }

    //! Return the option "pathname" to retrieve the option using
    //! \getOption() on the root object.  This will be the empty
    //! string for the root object, or if the iterator was
    //! constructed with the option compute_optnames set to 'false'.
    const string& getCurrentOptionName() const
    {
        assert( !invalid() );
        return m_it->second;
    }
    
    //! Go to next object or "invalid" state if end of iteration
    const ObjectGraphIterator& operator++();

    //! Post-increment: implemented in terms of pre-increment.
    //! Note that this is expensive; don't use if not necessary.
    ObjectGraphIterator operator++(int)
    {
        ObjectGraphIterator old(*this);
        ++(*this);
        return old;
    }

protected:
    //! Build a traversal graph from a root node; filters are not yet taken
    //! into account at this point
    void buildTraversalGraph(const Object* root, TraversalType tt,
                             bool compute_optnames);

protected:
    typedef pair<const Object*, string> ObjectAndName;
    typedef std::vector<ObjectAndName> ObjectList;
    typedef bool (*ISA)(const Object*);
    
    ObjectList m_object_list;                //!< from buildTraversalGraph()
    std::string m_base_class_filter;
    ObjectList::iterator m_it;               //!< Iterator within m_object_list
    ObjectList::iterator m_end;              //!< End of m_object_list
    ISA m_isa_tester;                        //!< Predicate for sub-type filter
};


//#####  Broadcast  ###########################################################

/**
 *  @function object_broadcast
 *  @brief    Call a specific member function across a graph of \c Objects.
 *
 *  The global function object_broadcast is used to call a member function on a
 *  graph of \c Objects, but only for those objects that are of a class that
 *  can accept the member function.  Right now, forms with 0, 1 or 2 arguments
 *  are supported.
 *
 *  Both const and non-const forms are supported.
 *
 *  The return values of the individual functions called are ignored.
 *
 *  Implementation note: we use the Boost call_traits library to ensure that
 *  _references to references_ do not occur in the argument lists of
 *  object_broadcast, which would be outlawed by the C++ standard.
 */

// Zero argument, const
template <class T, class U>
void object_broadcast(const Object* o, U (T::*func)() const,
                      ObjectGraphIterator::TraversalType tt = ObjectGraphIterator::DepthPreOrder)
{
    ObjectGraphIterator grit(o, tt, false, T::_classname_()), grend;
    for ( ; grit != grend ; ++grit)
        if (const T* t = dynamic_cast<const T*>(*grit))
            (t->*func)();
}


// Zero argument, non-const
template <class T, class U>
void object_broadcast(Object* o, U (T::*func)(),
                      ObjectGraphIterator::TraversalType tt = ObjectGraphIterator::DepthPreOrder)
{
    ObjectGraphIterator grit(o, tt, false, T::_classname_()), grend;
    for ( ; grit != grend ; ++grit)
        if (T* t = const_cast<T*>(dynamic_cast<const T*>(*grit)))
            (t->*func)();
}


// One argument, const
template <class T, class U, class V>
void object_broadcast(const Object* o, U (T::*func)(V) const,
                      typename boost::call_traits<V>::param_type arg1,
                      ObjectGraphIterator::TraversalType tt = ObjectGraphIterator::DepthPreOrder)
{
    ObjectGraphIterator grit(o, tt, false, T::_classname_()), grend;
    for ( ; grit != grend ; ++grit)
        if (const T* t = dynamic_cast<const T*>(*grit))
            (t->*func)(arg1);
}


// One argument, non-const
template <class T, class U, class V>
void object_broadcast(Object* o, U (T::*func)(V),
                      typename boost::call_traits<V>::param_type arg1,
                      ObjectGraphIterator::TraversalType tt = ObjectGraphIterator::DepthPreOrder)
{
    ObjectGraphIterator grit(o, tt, false, T::_classname_()), grend;
    for ( ; grit != grend ; ++grit)
        if (T* t = const_cast<T*>(dynamic_cast<const T*>(*grit)))
            (t->*func)(arg1);
}


// Two arguments, const
template <class T, class U, class V, class W>
void object_broadcast(const Object* o, U (T::*func)(V,W) const,
                      typename boost::call_traits<V>::param_type arg1,
                      typename boost::call_traits<W>::param_type arg2,
                      ObjectGraphIterator::TraversalType tt = ObjectGraphIterator::DepthPreOrder)
{
    ObjectGraphIterator grit(o, tt, false, T::_classname_()), grend;
    for ( ; grit != grend ; ++grit)
        if (const T* t = dynamic_cast<const T*>(*grit))
            (t->*func)(arg1,arg2);
}


// Two arguments, non-const
template <class T, class U, class V, class W>
void object_broadcast(Object* o, U (T::*func)(V,W),
                      typename boost::call_traits<V>::param_type arg1,
                      typename boost::call_traits<W>::param_type arg2,
                      ObjectGraphIterator::TraversalType tt = ObjectGraphIterator::DepthPreOrder)
{
    ObjectGraphIterator grit(o, tt, false, T::_classname_()), grend;
    for ( ; grit != grend ; ++grit)
        if (T* t = const_cast<T*>(dynamic_cast<const T*>(*grit)))
            (t->*func)(arg1,arg2);
}


} // end of namespace PLearn

#endif


/*
  Local Variables:
  mode:c++
  c-basic-offset:4
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0))
  indent-tabs-mode:nil
  fill-column:79
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=79 :
