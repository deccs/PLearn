// -*- C++ -*-

// PLS.h
//
// Copyright (C) 2004 Olivier Delalleau 
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
   * $Id: PLS.h,v 1.3 2004/04/27 13:05:14 tihocan Exp $ 
   ******************************************************* */

// Authors: Olivier Delalleau

/*! \file PLS.h */

#ifndef PLS_INC
#define PLS_INC

#include "PLearner.h"

namespace PLearn {
using namespace std;

class PLS: public PLearner
{

public:

  typedef PLearner inherited;
  
protected:

  // *********************
  // * protected options *
  // *********************

  Mat B;
  int m;
  Vec mean_input;
  Vec mean_target;
  int p;
  Vec stddev_input;
  Vec stddev_target;
  Mat W;

public:

  // ************************
  // * public build options *
  // ************************

  int k;
  string method;
  real precision;
  bool output_the_score;
  bool output_the_target;

  // ****************
  // * Constructors *
  // ****************

  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  PLS();


  // ********************
  // * PLearner methods *
  // ********************

private: 

  //! This does the actual building. 
  void build_();

protected: 
  
  //! Declares this class' options.
  static void declareOptions(OptionList& ol);

public:

  // ************************
  // **** Object methods ****
  // ************************

  //! Simply calls inherited::build() then build_().
  virtual void build();

  //! Transforms a shallow copy into a deep copy.
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  // Declares other standard object methods.
  // If your class is not instantiatable (it has pure virtual methods)
  // you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT_METHODS.
  PLEARN_DECLARE_OBJECT(PLS);


  // **************************
  // **** PLearner methods ****
  // **************************

  //! Returns the size of this learner's output, (which typically
  //! may depend on its inputsize(), targetsize() and set options).
  virtual int outputsize() const;

  //! (Re-)initializes the PLearner in its fresh state (that state may depend on the 'seed' option)
  //! And sets 'stage' back to 0 (this is the stage of a fresh learner!).
  virtual void forget();
    
  //! The role of the train method is to bring the learner up to stage==nstages,
  //! updating the train_stats collector with training costs measured on-line in the process.
  virtual void train();

  //! Computes the output from the input.
  virtual void computeOutput(const Vec& input, Vec& output) const;

  //! Computes the costs from already computed output. 
  virtual void computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                       const Vec& target, Vec& costs) const;
                                
  //! Returns the names of the costs computed by computeCostsFromOutpus (and thus the test method).
  virtual TVec<string> getTestCostNames() const;

  //! Returns the names of the objective costs that the train method computes and 
  //! for which it updates the VecStatsCollector train_stats.
  virtual TVec<string> getTrainCostNames() const;

  // *** SUBCLASS WRITING: ***
  // While in general not necessary, in case of particular needs 
  // (efficiency concerns for ex) you may also want to overload
  // some of the following methods:
  // virtual void computeOutputAndCosts(const Vec& input, const Vec& target, Vec& output, Vec& costs) const;
  // virtual void computeCostsOnly(const Vec& input, const Vec& target, Vec& costs) const;
  // virtual void test(VMat testset, PP<VecStatsCollector> test_stats, VMat testoutputs=0, VMat testcosts=0) const;
  // virtual int nTestCosts() const;
  // virtual int nTrainCosts() const;


  //***** STATIC METHODS *****

  //! Compute the largest eigenvector of m with the NIPALS algorithm:
  //!   (1) v <- random initialization (but normalized)
  //!   (2) v = m.v, normalize v
  //!   (3) if there is a v[i] that has changed by more than 'preicision',
  //!       go to (2), otherwise return v.
  static void NIPALSEigenvector(const Mat& m, Vec& v, real precision);

};

// Declares a few other classes and functions related to this class.
DECLARE_OBJECT_PTR(PLS);
  
} // end of namespace PLearn

#endif
