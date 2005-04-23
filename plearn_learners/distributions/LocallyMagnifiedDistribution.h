// -*- C++ -*-

// LocallyMagnifiedDistribution.h
//
// Copyright (C) 2005 Pascal Vincent 
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
   * $Id: LocallyMagnifiedDistribution.h,v 1.1 2005/04/23 13:19:23 plearner Exp $ 
   ******************************************************* */

// Authors: Pascal Vincent

/*! \file LocallyMagnifiedDistribution.h */


#ifndef LocallyMagnifiedDistribution_INC
#define LocallyMagnifiedDistribution_INC

#include <plearn_learners/distributions/PDistribution.h>
#include <plearn/ker/Kernel.h>
#include <plearn_learners/nearest_neighbors/GenericNearestNeighbors.h>

namespace PLearn {

class LocallyMagnifiedDistribution: public PDistribution
{

private:

  typedef PDistribution inherited;  

  //! Global storage to save memory allocations.
  mutable Vec trainsample, weights;

protected:

  // *********************
  // * protected options *
  // *********************

  PP<GenericNearestNeighbors> NN;
  Vec emptyvec;
  mutable Vec NN_outputs;
  mutable Vec NN_costs;

    
public:

  // ************************
  // * public build options *
  // ************************

  //! The kernel that will be used to locally weigh the samples
  Ker weighting_kernel;

  //! The distribution that will be trained with local weights
  PP<PDistribution> localdistr; 

  int mode;

  int width_neighbor;
  real width_factor;
  string width_optionname;

  // ****************
  // * Constructors *
  // ****************

  //! Default constructor.
  LocallyMagnifiedDistribution();

  // *************************
  // * PDistribution methods *
  // *************************

private: 

  //! This does the actual building. 
  // ### Please implement in .cc.
  void build_();

protected: 

  //! Declare this class' options.
  // ### Please implement in .cc.
  static void declareOptions(OptionList& ol);

public:

  // ************************
  // **** Object methods ****
  // ************************

  //! Simply call inherited::build() then build_().
  virtual void build();

  //! Transform a shallow copy into a deep copy.
  virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

  // Declare other standard object methods.
  // ### If your class is not instantiatable (it has pure virtual methods)
  // ### you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT_METHODS.
  PLEARN_DECLARE_OBJECT(LocallyMagnifiedDistribution);

  // *******************************
  // **** PDistribution methods ****
  // *******************************

  //! Return log of probability density log(p(y | x)).
  virtual real log_density(const Vec& x) const;

  // **************************
  // **** PLearner methods ****
  // **************************

  //! The role of the train method is to bring the learner up to stage == nstages,
  //! updating the train_stats collector with training costs measured on-line in the process.
  // ### You may remove this method if your distribution does not implement it.
  virtual void train();

  virtual void forget();

};

// Declare a few other classes and functions related to this class.
DECLARE_OBJECT_PTR(LocallyMagnifiedDistribution);
  
} // end of namespace PLearn

#endif
