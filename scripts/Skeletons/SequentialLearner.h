#ifndef DERIVEDCLASS_INC
#define DERIVEDCLASS_INC

#include <plearn_learners/sequential/SequentialLearner.h>

namespace PLearn {

class DERIVEDCLASS: public SequentialLearner
{

private:

  typedef SequentialLearner inherited;
  
private:
  //! This does the actual building
  void build_();

protected:
  //! Declare this class' options
  static void declareOptions(OptionList& ol);

public:

  //! Constructor
  DERIVEDCLASS();

  //! simply calls inherited::build() then build_()
  virtual void build();

  //! *** SUBCLASS WRITING: ***
  virtual void train();
 
  //! *** SUBCLASS WRITING: ***
  virtual void test(VMat testset, PP<VecStatsCollector> test_stats,
                    VMat testoutputs=0, VMat testcosts=0) const;

  virtual void computeOutputAndCosts(const Vec& input, const Vec& target,
                                     Vec& output, Vec& costs) const;

  virtual void computeCostsOnly(const Vec& input, const Vec& target,
                                Vec& costs) const;

  virtual void computeOutput(const Vec& input, Vec& output) const;

  virtual void computeCostsFromOutputs(const Vec& input, const Vec& output,
                                       const Vec& target, Vec& costs) const;

  virtual void forget();

  virtual TVec<std::string> getTrainCostNames() const;
  virtual TVec<std::string> getTestCostNames() const;

  //!  Does the necessary operations to transform a shallow copy (this)
  //!  into a deep copy by deep-copying all the members that need to be.
  PLEARN_DECLARE_OBJECT(DERIVEDCLASS);
  virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);
};

//! Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(DERIVEDCLASS);

} // end of namespace PLearn

#endif
