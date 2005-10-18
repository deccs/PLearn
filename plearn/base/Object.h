// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2005 Pascal Vincent and Yoshua Bengio
// Copyright (C) 2002 Frederic Morin
// Copyright (C) 1999-2005 University of Montreal

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


/*! \file PLearnLibrary/PLearnCore/Object.h */

#ifndef Object_INC
#define Object_INC

#include <map>
#include <string>
#include "PP.h"
#include "StaticInitializer.h"
#include "TypeFactory.h"
#include "Option.h"
#include <plearn/io/PPath.h>
#include <plearn/io/openString.h>

namespace PLearn {
using namespace std;

/**
 * One of the following macros should be called in every Object subclass.  The
 * DECLARE macro in the class's declaration in the .h, and the corresponding
 * IMPLEMENT macro in the class's definitionin the .cc
 *
 * They automatically declare and define important methods used for the build,
 * help, and serilisation mechanism.
 *
 * The ABSTRACT versions of the macros should be used for "abstract" classes
 * (classes that are only meant to be derived, and are non instantiable because
 * they declare pure virtual methods, with no definition)
 *
 * The IMPLEMENT macros take two extra string arguments (other than the class's
 * type): a short one line description of the class, and a multiline help.
 */

#define PLEARN_DECLARE_OBJECT(CLASSTYPE)                        \
        public:                                                 \
        static string _classname_();                            \
        virtual string classname() const;                       \
        static OptionList& _getOptionList_();                   \
        virtual OptionList& getOptionList() const;              \
        static Object* _new_instance_for_typemap_();            \
        static bool _isa_(const Object* o);                     \
        virtual CLASSTYPE* deepCopy(CopiesMap &copies) const;   \
        static void _static_initialize_();                      \
        static StaticInitializer _static_initializer_

#define PLEARN_IMPLEMENT_OBJECT(CLASSTYPE, ONELINEDESCR, MULTILINEHELP)                         \
        string CLASSTYPE::_classname_()                                                         \
        {                                                                                       \
            return #CLASSTYPE;                                                                  \
        }                                                                                       \
                                                                                                \
        string CLASSTYPE::classname() const                                                     \
        {                                                                                       \
            return _classname_();                                                               \
        }                                                                                       \
                                                                                                \
        OptionList& CLASSTYPE::_getOptionList_()                                                \
        {                                                                                       \
            static OptionList ol;                                                               \
            if(ol.empty())                                                                      \
                declareOptions(ol);                                                             \
            return ol;                                                                          \
        }                                                                                       \
                                                                                                \
        OptionList& CLASSTYPE::getOptionList() const                                            \
        {                                                                                       \
            return _getOptionList_();                                                           \
        }                                                                                       \
                                                                                                \
        Object* CLASSTYPE::_new_instance_for_typemap_()                                         \
        {                                                                                       \
            return new CLASSTYPE();                                                             \
        }                                                                                       \
                                                                                                \
        bool CLASSTYPE::_isa_(const Object* o)                                                  \
        {                                                                                       \
            return dynamic_cast<const CLASSTYPE*>(o) != 0;                                      \
        }                                                                                       \
                                                                                                \
        CLASSTYPE* CLASSTYPE::deepCopy(CopiesMap& copies) const                                 \
        {                                                                                       \
            CopiesMap::iterator it = copies.find(this);                                         \
            if (it != copies.end())                                                             \
                return static_cast<CLASSTYPE*>(it->second);                                     \
            CLASSTYPE* deep_copy =                                                              \
                new CLASSTYPE(dynamic_cast<const CLASSTYPE&>(*this));                           \
            copies[this] = deep_copy;                                                           \
            deep_copy->makeDeepCopyFromShallowCopy(copies);                                     \
            return deep_copy;                                                                   \
        }                                                                                       \
                                                                                                \
        void CLASSTYPE::_static_initialize_()                                                   \
        {                                                                                       \
            TypeFactory::register_type(#CLASSTYPE,                                              \
                                       inherited::_classname_(),                                \
                                       &CLASSTYPE::_new_instance_for_typemap_,                  \
                                       &CLASSTYPE::_getOptionList_,                             \
                                       &CLASSTYPE::_isa_,                                       \
                                       ONELINEDESCR,                                            \
                                       MULTILINEHELP);                                          \
        }                                                                                       \
        StaticInitializer CLASSTYPE::_static_initializer_(&CLASSTYPE::_static_initialize_)


#define PLEARN_DECLARE_ABSTRACT_OBJECT(CLASSTYPE)               \
        public:                                                 \
        static string _classname_();                            \
        static OptionList& _getOptionList_();                   \
        static bool _isa_(const Object* o);                     \
        virtual CLASSTYPE* deepCopy(CopiesMap &copies) const;   \
        static void _static_initialize_();                      \
        static StaticInitializer _static_initializer_

#define PLEARN_IMPLEMENT_ABSTRACT_OBJECT(CLASSTYPE, ONELINEDESCR, MULTILINEHELP)                \
        string CLASSTYPE::_classname_()                                                         \
        {                                                                                       \
            return #CLASSTYPE;                                                                  \
        }                                                                                       \
                                                                                                \
        OptionList& CLASSTYPE::_getOptionList_()                                                \
        {                                                                                       \
            static OptionList ol;                                                               \
            if(ol.empty())                                                                      \
                declareOptions(ol);                                                             \
            return ol;                                                                          \
        }                                                                                       \
                                                                                                \
        bool CLASSTYPE::_isa_(const Object* o)                                                  \
        {                                                                                       \
            return dynamic_cast<const CLASSTYPE*>(o) != 0;                                      \
        }                                                                                       \
                                                                                                \
        CLASSTYPE* CLASSTYPE::deepCopy(CopiesMap& copies) const                                 \
        {                                                                                       \
            PLERROR("Called virtual method deepCopy of an abstract class. "                     \
                    "This should never happen!");                                               \
            return 0;                                                                           \
        }                                                                                       \
                                                                                                \
        void CLASSTYPE::_static_initialize_()                                                   \
        {                                                                                       \
            TypeFactory::register_type(#CLASSTYPE,                                              \
                                       inherited::_classname_(),                                \
                                       0,                                                       \
                                       &CLASSTYPE::_getOptionList_,                             \
                                       &CLASSTYPE::_isa_,                                       \
                                       ONELINEDESCR,                                            \
                                       MULTILINEHELP  );                                        \
        }                                                                                       \
        StaticInitializer CLASSTYPE::_static_initializer_(&CLASSTYPE::_static_initialize_)


// Now for TEMPLATEs...

/* Ex: For a template class Toto

template<class T, int U> 
class Toto: public Titi<T> {
public:

typedef Titi<T> inherited;
#define TEMPLATE_DEF_Toto      class T, int U
#define TEMPLATE_ARGS_Toto     T,U
#define TEMPLATE_NAME_Toto string("Toto< ") + TypeTraits<T>::name() + ", " + tostring(U) + " >"
PLEARN_DECLARE_TEMPLATE_OBJECT(Toto)

...
};

PLEARN_IMPLEMENT_TEMPLATE_OBJECT(Toto,"One line description","Multi line help")

// Puis au besoin, pour chaque version de template instanci�e, il faudra
// peut-�tre d�finir le _static_initializer_ (si le compilo n'est pas assez 
// smart pour le faire tout seul depuis la d�finition du template)
template<> StaticInitializer Toto<int,3>::_static_initializer_(&Toto<int,3>::_static_initialize_);

*/

#define PLEARN_DECLARE_TEMPLATE_OBJECT(CLASSTYPE)                                               \
        public:                                                                                 \
        static string _classname_();                                                            \
        virtual string classname() const;                                                       \
        static OptionList& _getOptionList_();                                                   \
        virtual OptionList& getOptionList() const;                                              \
        static Object* _new_instance_for_typemap_();                                            \
        static bool _isa_(const Object* o);                                                     \
        virtual CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >* deepCopy(CopiesMap &copies) const;    \
        static void _static_initialize_();                                                      \
        static StaticInitializer _static_initializer_;

#define PLEARN_IMPLEMENT_TEMPLATE_OBJECT(CLASSTYPE, ONELINEDESCR, MULTILINEHELP)                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        string CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_classname_()                          \
        {                                                                                       \
            return TEMPLATE_NAME_ ## CLASSTYPE ;                                                \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        string CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::classname() const                      \
        {                                                                                       \
            return _classname_();                                                               \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        OptionList& CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_getOptionList_()                 \
        {                                                                                       \
            static OptionList ol;                                                               \
            if(ol.empty())                                                                      \
              declareOptions(ol);                                                               \
            return ol;                                                                          \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        OptionList& CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::getOptionList() const             \
        {                                                                                       \
            return _getOptionList_();                                                           \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        Object* CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_new_instance_for_typemap_()          \
        {                                                                                       \
            return new CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >();                              \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        bool CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_isa_(const Object* o)                   \
        {                                                                                       \
            return dynamic_cast<const CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >*>(o) != 0;       \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >*                                               \
        CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::deepCopy(CopiesMap& copies) const             \
        {                                                                                       \
            CopiesMap::iterator it = copies.find(this);                                         \
            if (it != copies.end())                                                             \
                return static_cast<CLASSTYPE*>(it->second);                                     \
            CLASSTYPE* deep_copy = new CLASSTYPE(dynamic_cast<const CLASSTYPE&>(*this));        \
            copies[this] = deep_copy;                                                           \
            deep_copy->makeDeepCopyFromShallowCopy(copies);                                     \
            return deep_copy;                                                                   \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        void CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_static_initialize_()                    \
        {                                                                                       \
            TypeFactory::register_type(                                                         \
                CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_classname_(),                        \
                CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::inherited::_classname_(),             \
                &CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_new_instance_for_typemap_,          \
                &CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_getOptionList_,                     \
                &CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_isa_,                               \
                ONELINEDESCR,                                                                   \
                MULTILINEHELP);                                                                 \
        }                                                                                       \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        StaticInitializer CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::                            \
            _static_initializer_(&CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::                    \
                                 _static_initialize_);

/** Declare a partially specialized class specific to a given class. This is to
 * ensure the 'diff' static method that is called in Option.h is the one
 * specific to this class.
*/
#define DECLARE_SPECIALIZED_DIFF_CLASS(CLASSTYPE)                                               \
        template<class ObjectType>                                                              \
        class DiffTemplate<ObjectType, CLASSTYPE> {                                             \
            public:                                                                             \
                static int diff(const string& refer, const string& other,                       \
                                const Option<ObjectType, CLASSTYPE>* opt,                       \
                                PLearnDiff* diffs)                                              \
                {                                                                               \
                    return PLearn::diff(refer, other, opt, diffs);                              \
                }                                                                               \
        };
 
/*! The following macro should be called just *after* the declaration of
  an object subclass. It declares and defines a few inline functions needed for
  the serialization of pointers to the newly declared object type and the
  comparison (diff) with other objects.
*/

#define DECLARE_OBJECT_PTR(CLASSTYPE)                                                   \
        inline Object *toObjectPtr(const CLASSTYPE &o)                                  \
        {                                                                               \
            return const_cast<CLASSTYPE *>(&o);                                         \
        }                                                                               \
                                                                                        \
        inline PStream &operator>>(PStream &in, CLASSTYPE &o)                           \
        {                                                                               \
            o.newread(in);                                                              \
            return in;                                                                  \
        }                                                                               \
                                                                                        \
        inline PStream &operator>>(PStream &in, CLASSTYPE * &o)                         \
        {                                                                               \
            Object *ptr = o;                                                            \
            in >> ptr;                                                                  \
            o = dynamic_cast<CLASSTYPE *>(ptr);                                         \
            if(ptr!=0 && o==0)                                                          \
              PLERROR("Mismatched classes while reading a pointer: %s is not a %s",     \
                   ptr->classname().c_str(),CLASSTYPE::_classname_().c_str());          \
            return in;                                                                  \
        }                                                                               \
                                                                                        \
        inline PStream &operator<<(PStream &out, const CLASSTYPE &o)                    \
        {                                                                               \
            o.newwrite(out);                                                            \
            return out;                                                                 \
        }                                                                               \
                                                                                        \
        inline PStream &operator>>(PStream &in, PP<CLASSTYPE> &o)                       \
        {                                                                               \
            Object *ptr = (CLASSTYPE *)o;                                               \
            in >> ptr;                                                                  \
            o = dynamic_cast<CLASSTYPE *>(ptr);                                         \
            if(ptr!=0 && o.isNull())                                                    \
              PLERROR("Mismatched classes while reading a PP: %s is not a %s",          \
                   ptr->classname().c_str(),CLASSTYPE::_classname_().c_str());          \
            return in;                                                                  \
        }                                                                               \
                                                                                        \
        template<class ObjectType>                                                      \
        int diff(const string& refer, const string& other,                              \
                 const Option<ObjectType, CLASSTYPE>* opt, PLearnDiff* diffs)           \
        {                                                                               \
            PP<OptionBase> new_opt = new Option<ObjectType, PP<CLASSTYPE> >             \
                (opt->optionname(), 0, 0, "", "", "");                                  \
            return new_opt->diff(refer, other, diffs);                                  \
        }                                                                               \
        DECLARE_SPECIALIZED_DIFF_CLASS(CLASSTYPE)                                       \
        DECLARE_TYPE_TRAITS(CLASSTYPE)

#define DECLARE_TEMPLATE_OBJECT_PTR(CLASSTYPE)                                                  \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        inline Object *toObjectPtr(const CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  &o)          \
        {                                                                                       \
            return const_cast<CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  *>(&o);                 \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        inline PStream &operator>>(PStream &in, CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  &o)   \
        {                                                                                       \
            o.newread(in); return in;                                                           \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        inline PStream &operator>>(PStream &in, CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  * &o) \
        {                                                                                       \
            if (o)                                                                              \
                o->newread(in);                                                                 \
            else                                                                                \
                o = static_cast<CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  *>(readObject(in));   \
            return in;                                                                          \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        inline PStream &                                                                        \
        operator<<(PStream &out, const CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  &o)            \
        {                                                                                       \
            o.newwrite(out);                                                                    \
            return out;                                                                         \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        inline PStream&                                                                         \
        operator>>(PStream &in, PP<CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE > > &o)               \
        {                                                                                       \
            Object *ptr = (CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  *)o;                       \
            in >> ptr;                                                                          \
            o = dynamic_cast<CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >  *>(ptr);                 \
            return in;                                                                          \
        }                                                                                       \
                                                                                                \
        template < TEMPLATE_DEF_ ## CLASSTYPE >                                                 \
        class TypeTraits< CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE > >                            \
        {                                                                                       \
        public:                                                                                 \
            static inline string name()                                                         \
            { return CLASSTYPE< TEMPLATE_ARGS_ ## CLASSTYPE >::_classname_(); }                 \
                                                                                                \
            static inline unsigned char little_endian_typecode()                                \
            { return 0xFF; }                                                                    \
                                                                                                \
            static inline unsigned char big_endian_typecode()                                   \
            { return 0xFF; }                                                                    \
        } 



/*! The following macro should be called after the declaration of a 
  new SmartPointer derived class. It will declare a number of inline 
  functions used to serialize the new smart pointer type */

#define DECLARE_OBJECT_PP(PPCLASSTYPE, CLASSTYPE)                       \
        inline PStream &operator>>(PStream &in, PPCLASSTYPE &o)         \
          { Object *ptr = 0;                                            \
            in >> ptr;                                                  \
            o = dynamic_cast<CLASSTYPE *>(ptr);                         \
            return in; }                                                \
        inline PStream &operator<<(PStream &out, const PPCLASSTYPE &o)  \
          { out << static_cast<const PP<CLASSTYPE> &>(o); return out; } \
        DECLARE_TYPE_TRAITS(PPCLASSTYPE)


//#####  PLearn::Object  ######################################################

/**
 * @class Object
 * @brief Object is the base class of all high level PLearn objects.
 *
 * @par It exposes simple mechanisms for:
 *
 * @li automatic memory management (through reference counting and smart pointers)
 * @li serialization (read, write, save, load)
 * @li runtime type information (classname)
 * @li displaying (info, print)
 * @li deep copying (deepCopy)
 * @li a generic way of setting options (setOption) when not knowing the
 *     exact type of the Object and a generic build() method (the combination
 *     of the two allows to change the object structure and rebuild it at
 *     runtime)
 */
class Object: public PPointable
{
    typedef Object inherited;

public:
    //#####  Public Interface  ################################################
    
    PLEARN_DECLARE_OBJECT(Object);

    /**
     *  Default Constructor.
     *
     *  SUBCLASS WRITING: Note: all subclasses should define a default
     *  constructor (one that can be called without arguments), whose main role
     *  is to give a reasonable default value to all build options (see
     *  declareOptions).  Completing the actual building of the object is left
     *  to the build_() and build() methods (see below).
     */
    Object();

    //! Virtual Destructor
    virtual ~Object();

    // We rely on the default compiler-generated copy constructor and
    // assignment operator.

    /**
     *  Post-constructor.  The normal implementation should call simply
     *  inherited::build(), then this class's build_().  This method should be
     *  callable again at later times, after modifying some option fields to
     *  change the "architecture" of the object.
     */
    virtual void build();
    
    /**
     *  Does the necessary operations to transform a shallow copy (this) into a
     *  deep copy by deep-copying all the members that need to be.  This needs
     *  to be overridden by every class that adds "complex" data members to the
     *  class, such as \c Vec, \c Mat, \c PP<Something>, etc.  Typical
     *  implementation:
     *
     *  @code
     *  void CLASS_OF_THIS::makeDeepCopyFromShallowCopy(CopiesMap& copies)
     *  {
     *      inherited::makeDeepCopyFromShallowCopy(copies);
     *      deepCopyField(complex_data_member1, copies);
     *      deepCopyField(complex_data_member2, copies);
     *      ...
     *  }
     *  @endcode
     *
     *  @param copies  A map used by the deep-copy mechanism to keep track of
     *                 already-copied objects.
     */
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

    /**
     *  Returns a bit more informative string about object (default returns
     *  classname())
     *
     *  @return Information about the object
     */
    virtual string info() const; 

    /**
     *  Reads and sets the value for the specified option from the specified
     *  stream.
     *
     *  @param in          PStream from which to read the new option value
     *  @param optionname  Name of the option to read from the stream
     */
    void readOptionVal(PStream &in, const string &optionname);
    
    /**
     *  Writes the value of the specified option to the specified stream.
     *
     *  @param out         PStream into which write the option value
     *  @param optionname  Name of the option to write out to the stream
     */
    void writeOptionVal(PStream &out, const string &optionname) const;

    /**
     *  Rreturns a string of the names of all options to save.
     *  (optionnames are to be separated by a space, and must be supported by
     *  writeOptionVal)
     *
     *  @return List of options that don't have the \c OptionBase::nosave flag,
     *          separated by spaces.
     */
    virtual string getOptionsToSave() const;

    /**
     *  The default implementation serializes the object in the new format:
     *
     *  @code
     *  Classname(optionname=optionval; optionname=optionval; ...)
     *  @endcode
     *
     *  Subclasses may override this method to provide different outputs
     *  depending on \c out's mode (\c plearn_ascii, \c raw_ascii, ...).
     *
     *  @param in  Stream onto which serialize the object
     */
    virtual void newwrite(PStream& out) const;

    /**
     *  The default implementation reads and builds an object in the new format:
     *
     *  @code
     *  Classname(optionname=optionval; optionname=optionval; ...)
     *  @endcode
     *
     *  @param in  Stream from which read the object
     */
    void newread(PStream& in);

    /**
     *  Set an option (a data field) into an object.  This is a generic method
     *  to be able to set an option in an object in the most generic manner
     *  value is a string representation of the value to be set. It should only
     *  be called for initial construction or reloading of an object, prior to
     *  calling build(); To modify the options of an already built object, call
     *  changeOptions or changeOption instead.  If no option with that name
     *  exists, it causes an "Unknown option" runtime error.
     *
     *  The implementation calls \c readOptionVal() on a stringstream built
     *  from \c value.
     *
     *  @param optionname  Name of option to set
     *  @param value       String representation of the value to set the option
     *                     to.  Note that the actual value set into the option
     *                     is converted from the string representation into a
     *                     genuine C++ object acceptable for the option type.
     */
    void setOption(const string& optionname, const string& value);

    /**
     *  Return the string representation of an object's option (data field).
     *  This is a generic method to be able to retrieve the value of an
     *  option supported by the object (and its derivatives). The option
     *  value is returned as a string and MUST be converted to the correct type
     *  before use.
     *
     *  The implementation calls writeOptionVal into a string stream.
     *
     *  @param optionname  Name of option to get
     *  @return            String representation of the option value
     */
    string getOption(const string &optionname) const;

    /**
     *  Set multiple options simultaneously into the object.  This method
     *  should be used, rather than \c setOption(), when modifying some options
     *  of an already built object.  The default version simply calls \c
     *  setOption(), but subclasses should override it to execute any code
     *  required to put the object in a consistent state.  If the set of
     *  options would put the object in an inconsistent state, a runtime error
     *  should be issued.
     *
     *  @param name_value  Map of optionname:optionvalue pairs to set into the
     *                     object
     */
    virtual void changeOptions(const map<string,string>& name_value);

    //! Non-virtual method calls virtual \c changeOptions()
    void changeOption(const string& optionname, const string& value);

    /**
     *  Write the object to a C++ \c ostream.
     *
     *  The write method should write a complete description of the object to
     *  the given stream, that should be enough to later reconstruct it.  (a
     *  somewhat human-readable ascii format is usually preferred).  The new
     *  default version simply calls newwrite(...) which simply writes all the
     *  "options" declared in declareOptions, so there is no need to overload
     *  write in subclasses.  Old classes that still override write should
     *  progressively be moved to the new declareOptions/build mechanism.
     *
     *  @deprecated  Use the declareOption / build mechanism instead, that provides
     *               automatic serialization
     */
    virtual void write(ostream& out) const;

    /**
     *  Read the object from a C++ \c istream.
     *
     *  The read method is the counterpart of the write method. It should be
     *  able to reconstruct an object that has been previously written with the
     *  write method. The current implementation automatically decides whether
     *  to call newread() (which is based on the new declareOptions/build
     *  mechanism) or oldread() for backward compatibility (if the header is of
     *  the form <ClassName>).
     *
     *  @deprecated  Use the declareOption / build mechanism instead, that
     *               provides automatic serialization
     */
    virtual void read(istream& in);

    /**
     *  The call method is the standard way to allow for remote method
     *  invocation on instances of your class.  This should result in reading
     *  \c nargs input parameters from \c io, call the appropriate method, and
     *  send results to \c io.  A "Remote-callable method" is typically
     *  associated with an actual methods of your class, but it will usually
     *  differ in its "calling" conventions: its "name", number or input
     *  arguments, and number and nature of output results may differ.
     * 
     *  Here is what such a method should do:
     *  <ol>
     *  <li> Determine from the methodname what actual method to call.
     *       If the given methodname is none of those supported by your call method,
     *       call the parent's "call".
     *       Ex: <tt>inherited::call(methodname, nargs, io)</tt>
     *  <li> The number of arguments nargs may also influence what version of the
     *       method you want to call
     *  <li> read the narg arguments from io Ex: io >> age >> length >> n; 
     *  <li> call the actual associated method
     *  <li> call <tt>prepareToSendResults(io, nres)</tt> where nres is the
     *       number of result parameters. 
     *  <li> send the nres result parameters to io Ex: io << res1 << res2 <<res3;
     *  <li> call <tt>io.flush()</tt>
     *  </ol>
     * 
     *  If anything goes wrong during the process (bad arguments, etc...)
     *  simply call PLERROR with a meaningful message.
     *
     *  @param methodname  Name of method to call
     *  @param nargs       Number of arguments passed to the method
     *  @param io          Stream expected to contain \c nargs input arguments
     *                     and on which will be written the returned object
     */
    virtual void call(const string& methodname, int nargs, PStream& io);

    //! Must be called by the call method prior to sending results. 
    static void prepareToSendResults(PStream& out, int nres);

    /**
     *  Override this for runnable objects (default method issues a runtime
     *  error).  Runnable objects are objects that can be used as *THE* object
     *  of a .plearn script.  The run() method specifies what they should do
     *  when executed.
     */
    virtual void run();

    //! @deprecated  For backward compatibility with old saved object
    virtual void oldread(istream& in);

    /**
     *  @deprecated It simply calls the generic PLearn save function (that can
     *  save any PLearn object): PLearn::save(filename, *this) So you should
     *  call PLearn::save directly (it's defined in plearn/io/load_and_save.h).
     */
    virtual void save(const PPath& filename) const;

    /**
     *  @deprecated It simply calls the generic PLearn load function (that can
     *  load any PLearn object): PLearn::load(filename, *this) So you should
     *  call PLearn::load directly (it's defined in plearn/io/load_and_save.h).
     */
    virtual void load(const PPath& filename);

protected:
    //#####  Protected Member Functions  ##########################################

    /**
     *  Declare options (data fields) for the class.  Redefine this in
     *  subclasses: call \c declareOption(...) for each option, and then call
     *  \c inherited::declareOptions(options).  Please call the \c inherited
     *  method AT THE END to get the options listed in a consistent order (from
     *  most recently defined to least recently defined).
     *
     *  @code
     *  static void declareOptions(OptionList& ol)
     *  {
     *      declareOption(ol, "inputsize", &MyObject::inputsize_,
     *                    OptionBase::buildoption,
     *                    "The size of the input; it must be provided");
     *      declareOption(ol, "weights", &MyObject::weights,
     *                    OptionBase::learntoption,
     *                    "The learned model weights");
     *      inherited::declareOptions(ol);
     *  }
     *  @endcode
     *
     *  @param ol  List of options that is progressively being constructed for
     *             the current class.
     */
    static void declareOptions(OptionList& ol) { }

private:
    //#####  Private Member Functions  ############################################

    /**
     *  Object-specific post-constructor.  This method should be redefined in
     *  subclasses and do the actual building of the object according to
     *  previously set option fields.  Constructors can just set option fields,
     *  and then call build_.  This method is NOT virtual, and will typically
     *  be called only from three places: a constructor, the public virtual \c
     *  build() method, and possibly the public virtual read method (which
     *  calls its parent's read).  \c build_() can assume that its parent's \c
     *  build_() has already been called.
     */
    void build_();
};


/**
 *  This function builds an object from its representation in the stream.
 *  It understands several representations:
 *
 *  @li The <ObjectClass> ... </ObjectClass> type of representation 
 *      as is typically produced by write() serialization methods and functions.
 *      This will call the object's read() method.
 *  @li The ObjectClass( optionname=optionvalue; ... ; optionname=optionvalue )
 *      type of representation (typical form for human input), will result in 
 *      appropriate calls of the object's setOption() followed by its build().
 *  @li load( filepath ) will call loadObject
 */
Object *readObject(PStream &in, unsigned int id = UINT_MAX);
inline Object *readObject(istream &in_)
{
    PStream in(&in_);
    return readObject(in);
}

//! Loads an object from the given file (no macro-preprocessing is performed)
Object* loadObject(const PPath &filename);

//! Same as loadObject but first performs macro-processing on the file
//! vars may be initialised with the values of some variables
//! and upon return it will also contain newly $DEFINED variables 
Object* macroLoadObject(const PPath &filename, map<string,string>& vars);

//! same as previous, but no need to pass a variables map
Object* macroLoadObject(const PPath &filename);

//! Creates a new object according to the given representation.
//! This actually calls readObject on a PStream obtained with 
//! openString,  so anything
//! understandable by readObject can be used here
inline Object* newObject(const string& representation)
{
    PStream in = openString(representation, PStream::plearn_ascii);
    return readObject(in);
}

inline PStream &operator>>(PStream &in, Object &o)
{
    o.newread(in);
    return in;
}

inline PStream &operator<<(PStream &out, const Object &o)
{
    o.newwrite(out);
    return out;
}


//! This takes precedence over the template definitions for a template type T in PStream.h
PStream &operator>>(PStream &in, Object * &o);

} // end of namespace PLearn

//! Useful function for debugging inside gdb:
extern "C" void printobj(PLearn::Object* p);

#endif //!<  Object_INC


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
