#ifndef DERIVEDCLASS_INC
#define DERIVEDCLASS_INC

#include "TraderVariable.h"

namespace PLearn {
using namespace std;

class DERIVEDCLASS: public TraderVariable
{
public:
  typedef TraderVariable inherited;

private:
  // *********************
  // * private members   *
  // *********************

protected:

  // ***************************
  // * protected build options *
  // ***************************
  
  // *********************
  // * protected members *
  // *********************

public:
  
  // ************************
  // * public build options *
  // ************************
  
  // ****************
  // * Constructors *
  // ****************
  
  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  DERIVEDCLASS();
  
  // ****************
  // * Methods      * 
  // ****************
  
  // ***************************************
  // * PortfolioManagementVariable methods *
  // ***************************************  
  virtual Vec scale(const Vec& portfolio);  
  
  virtual void fprop();
  virtual void bprop();

private: 
  //! This does the actual building. 
  void build_();
  
protected: 
  //! Declares this class' options
  static void declareOptions(OptionList& ol);
  
public:
  
  // Declares other standard object methods
  //  If your class is not instantiatable (it has pure virtual methods)
  // you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT_METHODS 
  PLEARN_DECLARE_OBJECT(DERIVEDCLASS);
  
  // simply calls inherited::build() then build_() 
  virtual void build();
  
  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(DERIVEDCLASS);

} // end of namespace PLearn

#endif
