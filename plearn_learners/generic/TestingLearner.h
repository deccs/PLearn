// -*- C++ -*-

// TestingLearner.h
//
// Copyright (C) 2004 Marius Muja 
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
   * $Id: TestingLearner.h,v 1.4 2004/09/14 16:04:56 chrish42 Exp $ 
   ******************************************************* */

// Authors: Marius Muja

/*! \file TestingLearner.h */


#ifndef TestingLearner_INC
#define TestingLearner_INC

#include "PLearner.h"
#include <plearn_learners/testers/PTester.h>

namespace PLearn {
using namespace std;

class TestingLearner: public PLearner
{

private:

  typedef PLearner inherited;
  
protected:

  // *********************
  // * protected options *
  // *********************

public:

  // ************************
  // * public build options *
  // ************************

  PP<PTester> tester;

  // ****************
  // * Constructors *
  // ****************

  //! Default constructor.
  // (Make sure the implementation in the .cc
  // initializes all fields to reasonable default values)
  TestingLearner();


  // ********************
  // * PLearner methods *
  // ********************

private: 

  //! This does the actual building. 
  // (Please implement in .cc)
  void build_();

protected: 
  
  //! Declares this class' options.
  // (Please implement in .cc)
  static void declareOptions(OptionList& ol);

public:

  // ************************
  // **** Object methods ****
  // ************************

  //! Simply calls inherited::build() then build_().
  virtual void build();

  //! Transforms a shallow copy into a deep copy.
  virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

  // Declares other standard object methods.
  // If your class is not instantiatable (it has pure virtual methods)
  // you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT.
  PLEARN_DECLARE_OBJECT(TestingLearner);


  // **************************
  // **** PLearner methods ****
  // **************************

  //! Returns the size of this learner's output, (which typically
  //! may depend on its inputsize(), targetsize() and set options).
  // (PLEASE IMPLEMENT IN .cc)
  virtual int outputsize() const;

  //! (Re-)initializes the PLearner in its fresh state (that state may depend on the 'seed' option)
  //! And sets 'stage' back to 0 (this is the stage of a fresh learner!).
  // (PLEASE IMPLEMENT IN .cc)
  virtual void forget();

    
  //! The role of the train method is to bring the learner up to stage==nstages,
  //! updating the train_stats collector with training costs measured on-line in the process.
  // (PLEASE IMPLEMENT IN .cc)
  virtual void train();


  //! Computes the output from the input.
  // (PLEASE IMPLEMENT IN .cc)
  virtual void computeOutput(const Vec& input, Vec& output) const;

  //! Computes the costs from already computed output. 
  // (PLEASE IMPLEMENT IN .cc)
  virtual void computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                       const Vec& target, Vec& costs) const;
                                

  //! Returns the names of the costs computed by computeCostsFromOutpus (and thus the test method).
  // (PLEASE IMPLEMENT IN .cc)
  virtual TVec<string> getTestCostNames() const;

  //! Returns the names of the objective costs that the train method computes and 
  //! for which it updates the VecStatsCollector train_stats.
  // (PLEASE IMPLEMENT IN .cc)
  virtual TVec<string> getTrainCostNames() const;

  virtual void setTrainingSet(VMat training_set, bool call_forget=true);

  void setExperimentDirectory(const string& the_expdir);

  // *** SUBCLASS WRITING: ***
  // While in general not necessary, in case of particular needs 
  // (efficiency concerns for ex) you may also want to overload
  // some of the following methods:
  // virtual void computeOutputAndCosts(const Vec& input, const Vec& target, Vec& output, Vec& costs) const;
  // virtual void computeCostsOnly(const Vec& input, const Vec& target, Vec& costs) const;
  // virtual void test(VMat testset, PP<VecStatsCollector> test_stats, VMat testoutputs=0, VMat testcosts=0) const;
  // virtual int nTestCosts() const;
  // virtual int nTrainCosts() const;
  // virtual void resetInternalState();
  // virtual bool isStatefulLearner() const;

};

// Declares a few other classes and functions related to this class.
DECLARE_OBJECT_PTR(TestingLearner);
  
} // end of namespace PLearn

#endif
