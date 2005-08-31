// -*- C++ -*-

// ParentableObject.h
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

/*! \file ParentableObject.h */


#ifndef ParentableObject_INC
#define ParentableObject_INC

#include <plearn/base/Object.h>

namespace PLearn {

/**
 *  Object which maintains a "parent" pointer as part of an object graph.
 *
 *  The purpose of \c ParentableObject is to facilitate the building of complex
 *  graphs of \c Objects.  The basic ideas are as follows:
 *
 *  - The object contains a member called \c parent, which is simply a
 *    backpointer to the "parent" object in the graph.  This is a dumb
 *    pointer to avoid cycles as the forward pointers will usually be PP's.
 *
 *  - The \c build_() method looks at all the options of itself, and for those
 *    objects that are \c ParentableObject, it sets their \c parent pointer
 *    to \c this.  (This mechanism could conceptually be put in \c Object
 *    itself, but out of caution we leave it in \c ParentableObject for now).
 *
 *  In other words, this class both provides the backpointer in an object
 *  graph, and provides the mechanism to update the backpointer according to
 *  arbitrary forward pointers (as long as the forward pointers are accessible
 *  as options through an \c ObjectOptionsIterator.)
 */
class ParentableObject : public Object
{
    typedef Object inherited;

public:
    //#####  Public Member Functions  #########################################

    //! Default constructor
    ParentableObject();

    //! Accessor for parent object
    Object* parent()                       { return m_parent; }
    const Object* parent() const           { return m_parent; }
    
    
    //#####  PLearn::Object Protocol  #########################################

    // Declares other standard object methods.
    PLEARN_DECLARE_ABSTRACT_OBJECT(ParentableObject);

    // Simply calls inherited::build() then build_() 
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

protected:
    Object* m_parent;                        //!< Backpointer to parent

private: 
    //! Traverse the options of *this, find the ParentableObjects, and update
    //! _their_ backpointers to point to *this
    void build_();
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(ParentableObject);
  
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
