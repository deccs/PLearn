#include "DERIVEDCLASS.h"

namespace PLearn {
using namespace std;

//////////////////
// DERIVEDCLASS //
//////////////////
DERIVEDCLASS::DERIVEDCLASS() 
/* ### Initialize all fields to their default value here */
{
  // ...

  // ### You may or may not want to call build_() to finish building the object
  // build_();
}

PLEARN_IMPLEMENT_OBJECT(DERIVEDCLASS,
    "ONE LINE DESCRIPTION",
    "MULTI-LINE \nHELP"
);

////////////////////
// declareOptions //
////////////////////
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

///////////
// build //
///////////
void DERIVEDCLASS::build()
{
  // ### Nothing to add here, simply calls build_
  inherited::build();
  build_();
}

////////////
// build_ //
////////////
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

//////////////
// evaluate //
//////////////
real DERIVEDCLASS::evaluate(const Vec& x1, const Vec& x2) const {
  // ### Evaluate the kernel on a pair of points.
}

/* ### This method will very often be overridden.
//////////////////
// evaluate_i_j //
//////////////////
real DERIVEDCLASS::evaluate_i_j(int i, int j) const {
  // ### Evaluate the kernel on a pair of training points.
}
*/

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void DERIVEDCLASS::makeDeepCopyFromShallowCopy(map<const void*, void*>& copies)
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

/* ### This method will be overridden if computations need to be done,
   ### or to forward the call to another object.
   ### In this case, be careful that it may be called BEFORE the build_()
   ### method has been called, if the 'specify_dataset' option is used.
////////////////////////////
// setDataForKernelMatrix //
////////////////////////////
void DERIVEDCLASS::setDataForKernelMatrix(VMat the_data) {
}
*/

} // end of namespace PLearn
