// -*- C++ -*-

// AddCostToLearner.h
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
   * $Id: AddCostToLearner.h,v 1.4 2004/05/07 20:24:21 tihocan Exp $ 
   ******************************************************* */

// Authors: Olivier Delalleau

/*! \file AddCostToLearner.h */

#ifndef AddCostToLearner_INC
#define AddCostToLearner_INC

#include "PLearner.h"
#include "Var.h"

namespace PLearn {
using namespace std;

class AddCostToLearner: public PLearner
{

public:

  typedef PLearner inherited;
  
protected:

  // *********************
  // * protected options *
  // *********************

  // Fields below are not options.

  //! Used to store the outputs of the sub_learner for each sample in a bag.
  mutable Mat bag_outputs;

  //! Used to count the number of instances in a bag.
  mutable int bag_size;
    
  //! Propagation path for the cross entropy cost.
  mutable VarArray cross_entropy_prop;
  
  //! Variable giving the cross entropy cost.
  Var cross_entropy_var;
  
  //! Used to store the target when computing a cost.
  Vec desired_target;

  //! A precomputed factor for faster mapping.
  real fac;

  //! Constraints on the output given the costs being computed.
  real output_max;
  real output_min;

  //! Its value is sub_learner_output[0].
  Var output_var;
  
  //! Used to store the sub_learner_output.
  Vec sub_learner_output;

  //! Used to store the input given to the sub_learner, when it needs to be
  //! copied in a separate place.
  mutable Vec sub_input;

  //! Its value is desired_target[0].
  Var target_var;

public:

  // ************************
  // * public build options *
  // ************************

  bool check_output_consistency;
  int combine_bag_outputs_method;
  bool compute_costs_on_bags;
  TVec<string> costs;
  bool force_output_to_target_interval;
  real from_max;
  real from_min;
  bool rescale_output;
  bool rescale_target;
  PP<PLearner> sub_learner;
  real to_max;
  real to_min;

  // ****************
  // * Constructors *
  // ****************

  AddCostToLearner();

  // ******************
  // * PLearner methods *
  // ******************

private: 

  //! This does the actual building. 
  void build_();

protected: 

  //! Declares this class' options
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
  PLEARN_DECLARE_OBJECT(AddCostToLearner);


  // **************************
  // **** PLearner methods ****
  // **************************

  //! Returns the size of this learner's output, (which typically
  //! may depend on its inputsize(), targetsize() and set options)
  virtual int outputsize() const;

  //! (Re-)initializes the PLearner in its fresh state (that state may depend on the 'seed' option)
  //! And sets 'stage' back to 0   (this is the stage of a fresh learner!).
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

  //! Overrridden to forward to the sub_learner.
  virtual void setExperimentDirectory(const string& the_expdir) {
    sub_learner->setExperimentDirectory(the_expdir);
  }
  virtual void setTrainingSet(VMat training_set, bool call_forget=true);
  virtual void setTrainStatsCollector(PP<VecStatsCollector> statscol) {
    sub_learner->setTrainStatsCollector(statscol);
  }

};

// Declares a few other classes and functions related to this class.
DECLARE_OBJECT_PTR(AddCostToLearner);
  
} // end of namespace PLearn

#endif
