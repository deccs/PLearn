#include "DERIVEDCLASS.h"

namespace PLearn {
using namespace std;

DERIVEDCLASS::DERIVEDCLASS() 
  /* ### Initialize all fields to their default value */
{
  // ...

  // ### You may (or not) want to call build_() to finish building the object
  // ### (doing so assumes the parent classes' build_() have been called too
  // ### in the parent classes' constructors, something that you must ensure)
}

PLEARN_IMPLEMENT_OBJECT(DERIVEDCLASS,
    "ONE LINE DESCRIPTION",
    "MULTI LINE\nHELP"
);

void DERIVEDCLASS::declareOptions(OptionList& ol)
{
  // ### Declare all of this object's options here
  // ### For the "flags" of each option, you should typically specify  
  // ### one of OptionBase::buildoption, OptionBase::learntoption or 
  // ### OptionBase::tuningoption. Another possible flag to be combined with
  // ### is OptionBase::nosave

  // ### ex:
  // declareOption(ol, "myoption", &DERIVEDCLASS::myoption, OptionBase::buildoption,
  //               "Help text describing this option");
  // ...

  // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

void DERIVEDCLASS::build_()
{
  // ### This method should do the real building of the object,
  // ### according to set 'options', in *any* situation. 
  // ### Typical situations include:
  // ###  - Initial building of an object from a few user-specified options
  // ###  - Building of a "reloaded" object: i.e. from the complete set of all serialised options.
  // ###  - Updating or "re-building" of an object after a few "tuning" options have been modified.
  // ### You should assume that the parent class' build_() has already been called.
}

// ### Nothing to add here, simply calls build_
void DERIVEDCLASS::build()
{
  inherited::build();
  build_();
}

void DERIVEDCLASS::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);

  // ### Call deepCopyField on all "pointer-like" fields 
  // ### that you wish to be deepCopied rather than 
  // ### shallow-copied.
  // ### ex:
  // deepCopyField(trainvec, copies);

  // ### Remove this line when you have fully implemented this method.
  PLERROR("DERIVEDCLASS::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}

} // end of namespace PLearn
