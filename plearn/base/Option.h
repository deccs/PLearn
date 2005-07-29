// -*- C++ -*-4 1999/10/29 20:41:34 dugas

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio and University of Montreal
// Copyright (C) 2002 Frederic Morin

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
   * $Id$
   * This file is part of the PLearn library.
   ******************************************************* */


/*! \file PLearnLibrary/PLearnCore/Option.h */

#ifndef Option_INC
#define Option_INC

#include "OptionBase.h"
#include <plearn/base/lexical_cast.h>

namespace PLearn {
using std::string;

template <class T> class TVec;               //!< Forward-declare


//#####  Generic Option  ######################################################
  
//! Template class for option definitions
template<class ObjectType, class OptionType>
class Option: public OptionBase
{
  typedef OptionBase inherited;
  
protected:
  OptionType ObjectType::*ptr;

public:

  //! Most of these parameters only serve to provide the user 
  //! with an informative help text. (only optionname and saveit are really important)
  Option(const string& optionname, OptionType ObjectType::* member_ptr, 
         flag_t flags, const string& optiontype, const string& defaultval,
         const string& description)
    : inherited(optionname, flags, optiontype, defaultval, description),
      ptr(member_ptr)
    { }

  virtual void read(Object* o, PStream& in) const
    { in >> dynamic_cast<ObjectType*>(o)->*ptr; }

  virtual void read_and_discard(PStream& in) const
  { 
    string dummy;
    int c = in.smartReadUntilNext(";)", dummy);
    in.putback((char)c);
    //OptionType op; //dummy object that will be destroyed after read
    //in >> op; //read dummy object
  }

  virtual void write(const Object* o, PStream& out) const
    { out << dynamic_cast<ObjectType *>(const_cast<Object*>(o))->*ptr; }

  virtual Object* getAsObject(Object* o) const
    { return toObjectPtr(dynamic_cast<ObjectType*>(o)->*ptr); }

  virtual const Object* getAsObject(const Object* o) const
    { return toObjectPtr(dynamic_cast<const ObjectType*>(o)->*ptr); }

  virtual Object *getIndexedObject(Object *o, int i) const
    { return toIndexedObjectPtr(dynamic_cast<ObjectType*>(o)->*ptr, i); };

  virtual const Object* getIndexedObject(const Object *o, int i) const
    { return toIndexedObjectPtr(dynamic_cast<const ObjectType*>(o)->*ptr, i); };

  virtual string optionHolderClassName(const Object* o) const
    { return dynamic_cast<const ObjectType*>(o)->ObjectType::_classname_(); }
};


//#####  TVec-Specific Option  ################################################

// This is a special version of Option designed for TVec<T>.
// The only difference is that it supports indexed reads and writes.
template<class ObjectType, class VecElementType>
class TVecOption : public Option<ObjectType, TVec<VecElementType> >
{
  typedef Option<ObjectType, TVec<VecElementType> > inherited;
  
public:
  TVecOption(const string& optionname, TVec<VecElementType> ObjectType::* member_ptr, 
             OptionBase::flag_t flags, const string& optiontype, const string& defaultval,
             const string& description)
    : inherited(optionname, member_ptr, flags, optiontype, defaultval,
                description)
    { }

  virtual void readIntoIndex(Object* o, PStream& in, const string& index)
    {
      int i = tolong(index);
      in >> (dynamic_cast<ObjectType*>(o)->*(this->ptr))[i];
    }

  virtual void writeAtIndex(const Object* o, PStream& out, const string& index) const
    {
      int i = tolong(index);
      out << (dynamic_cast<ObjectType*>(const_cast<Object*>(o))->*(this->ptr))[i];
    }
};


//#####  declareOption and Friends  ###########################################

//! For flags, you should specify one of 
//! OptionBase::buildoption, OptionBase::learntoption or OptionBase::tuningoption
//! If the option is not to be serialized, you can additionally specify 
//! OptionBase::nosave
/*! The "type" printed in the help is given by TypeTraits<OptionType>::name().
    The "default value" printed in optionHelp() will be a serialization of 
    the value of the field in a default constructed instance, (which should be ok in most cases),
    unless you explicitly specify it as the last argument here (It is recomended that you *don't*
    specify it explicitly, unless you really must). */

template <class ObjectType, class OptionType>
inline void declareOption(OptionList& ol,                      //!< list to which this option should be appended 
                          const string& optionname,            //!< the name of this option
                          OptionType ObjectType::* member_ptr, //!< &YourClass::your_field
                          OptionBase::flag_t flags,            //! see the flags in OptionBase
                          const string& description,           //!< a description of the option
                          const string& defaultval="")         //!< default value for this option, as set by the default constructor
{
  ol.push_back(new Option<ObjectType, OptionType>(optionname, member_ptr, flags, 
                                                  TypeTraits<OptionType>::name(), 
                                                  defaultval, description));
}

// Partial specialization for pointers
template <class ObjectType, class OptionType>
inline void declareOption(OptionList& ol,
                          const string& optionname,
                          OptionType* ObjectType::* member_ptr,
                          OptionBase::flag_t flags,
                          const string& description,
                          const string& defaultval="")
{
  ol.push_back(new Option<ObjectType, OptionType *>(optionname, member_ptr, flags,
                                                    TypeTraits<OptionType *>::name(), 
                                                    defaultval, description));
}

// Partial specialization for TVec<T>
template <class ObjectType, class VecElementType>
inline void declareOption(OptionList& ol,
                          const string& optionname,
                          TVec<VecElementType> ObjectType::* member_ptr,
                          OptionBase::flag_t flags,
                          const string& description,
                          const string& defaultval="")
{
  ol.push_back(new TVecOption<ObjectType, VecElementType>(
                 optionname, member_ptr, flags,
                 TypeTraits< TVec<VecElementType> >::name(),
                 defaultval, description));
}


//! Allows one to redeclare an option differently
//! (e.g. in a subclass, after calling inherited::declareOptions).
template<class ObjectType, class OptionType>
inline void redeclareOption(OptionList& ol,                      //!< the list to which this option should be appended 
                            const string& optionname,            //!< the name of this option
                            OptionType ObjectType::* member_ptr, //!< &YourClass::your_field
                            OptionBase::flag_t flags,            //! see the flags in OptionBase
                            const string& description,           //!< a description of the option
                            const string & defaultval="")        //!< the default value for this option, as set by the default constructor
{
  bool found = false;
  for (OptionList::iterator it = ol.begin(); !found && it != ol.end(); it++) {
    if ((*it)->optionname() == optionname) {
      // We found the option to redeclare.
      found = true;
      (*it) = new Option<ObjectType, OptionType>
        (optionname, member_ptr, flags, TypeTraits<OptionType>::name(), defaultval, description);
    }
  }
  if (!found) {
    // We tried to redeclare an option that wasn't declared previously.
    PLERROR("Option::redeclareOption: trying to redeclare option '%s' that has "
            "not been declared before", optionname.c_str());
  }
}

//! Partial specialization for pointers
template<class ObjectType, class OptionType>
inline void redeclareOption(OptionList& ol,                      //!< the list to which this option should be appended 
                            const string& optionname,            //!< the name of this option
                            OptionType* ObjectType::* member_ptr,//!< &YourClass::your_field
                            OptionBase::flag_t flags,            //! see the flags in OptionBase
                            const string& description,           //!< a description of the option
                            const string & defaultval="")        //!< the default value for this option, as set by the default constructor
{
  bool found = false;
  for (OptionList::iterator it = ol.begin(); !found && it != ol.end(); it++) {
    if ((*it)->optionname() == optionname) {
      // We found the option to redeclare.
      found = true;
      (*it) = new Option<ObjectType, OptionType*>
        (optionname, member_ptr, flags, TypeTraits<OptionType*>::name(), defaultval, description);
    }
  }
  if (!found) {
    // We tried to redeclare an option that wasn't declared previously.
    PLERROR("Option::redeclareOption: trying to redeclare option '%s' that has "
            "not been declared before", optionname.c_str());
  }
}

//! Partial specialization for TVec<T>
template<class ObjectType, class VecElementType>
inline void redeclareOption(OptionList& ol,
                            const string& optionname,
                            TVec<VecElementType> ObjectType::* member_ptr,
                            OptionBase::flag_t flags,
                            const string& description,
                            const string & defaultval="")
{
  bool found = false;
  for (OptionList::iterator it = ol.begin(); !found && it != ol.end(); it++) {
    if ((*it)->optionname() == optionname) {
      // We found the option to redeclare.
      found = true;
      (*it) = new TVecOption<ObjectType, VecElementType>
        (optionname, member_ptr, flags,
         TypeTraits< TVec<VecElementType> >::name(),
         defaultval, description);
    }
  }
  if (!found) {
    // We tried to redeclare an option that wasn't declared previously.
    PLERROR("Option::redeclareOption: trying to redeclare option '%s' that has "
            "not been declared before", optionname.c_str());
  }
}


} // end of namespace PLearn

#endif //!<  Option_INC
