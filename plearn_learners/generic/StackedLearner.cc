
// -*- C++ -*-

// StackedLearner.cc
//
// Copyright (C) 2003  Pascal Vincent 
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
   * $Id: StackedLearner.cc,v 1.1 2003/07/04 18:48:59 plearner Exp $ 
   ******************************************************* */

/*! \file StackedLearner.cc */
#include "StackedLearner.h"

namespace PLearn <%
using namespace std;

StackedLearner::StackedLearner() 
/* ### Initialise all fields to their default value here */
  {
    // ...

    // ### You may or may not want to call build_() to finish building the object
    // build_();
  }

  PLEARN_IMPLEMENT_OBJECT_METHODS(StackedLearner, "StackedLearner", PLearner);

  void StackedLearner::declareOptions(OptionList& ol)
  {
    // ### Declare all of this object's options here
    // ### For the "flags" of each option, you should typically specify  
    // ### one of OptionBase::buildoption, OptionBase::learntoption or 
    // ### OptionBase::tuningoption. Another possible flag to be combined with
    // ### is OptionBase::nosave

    // ### ex:
    // declareOption(ol, "myoption", &StackedLearner::myoption, OptionBase::buildoption,
    //               "Help text describing this option");
    // ...

    // Now call the parent class' declareOptions
    parentclass::declareOptions(ol);
  }

  string StackedLearner::help()
  {
    // ### Provide some useful description of what the class is ...
    return 
      "StackedLearner implements a ...\n";
  }

  void StackedLearner::build_()
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
  void StackedLearner::build()
  {
    parentclass::build();
    build_();
  }


  void StackedLearner::makeDeepCopyFromShallowCopy(map<const void*, void*>& copies)
  {
    parentclass::makeDeepCopyFromShallowCopy(copies);

    // ### Call deepCopyField on all "pointer-like" fields 
    // ### that you wish to be deepCopied rather than 
    // ### shallow-copied.
    // ### ex:
    // deepCopyField(trainvec, copies);

    // ### Remove this line when you have fully implemented this method.
    PLERROR("StackedLearner::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
  }


int StackedLearner::outputsize() const
{
  // compute and return the size of this learner's output, (which typically
  // may depend on its inputsize(), targetsize() and set options)
}

void StackedLearner::forget()
{
  //! (Re-)initialize the PLearner in its fresh state (that state may depend on the 'seed' option)
  //! And sets 'stage' back to 0   (this is the stage of a fresh learner!)
    /*!
      A typical forget() method should do the following:
         - initialize a random number generator with the seed option
         - initialize the learner's parameters, using this random generator
         - stage = 0
    */
}
    
void StackedLearner::train()
{
    // The role of the train method is to bring the learner up to stage==nstages,
    // updating train_stats with training costs measured on-line in the process.

    /* TYPICAL CODE:

      static Vec input  // static so we don't reallocate/deallocate memory each time...
      static Vec target
      input.resize(inputsize())    // the train_set's inputsize()
      target.resize(targetsize())  // the train_set's targetsize()
      real weight

      if(!train_stats)  // make a default stats collector, in case there's none
         train_stats = new VecStatsCollector()

      if(nstages<stage) // asking to revert to a previous stage!
         forget()  // reset the learner to stage=0

      while(stage<nstages)
        {
          // clear statistics of previous epoch
          train_stats->forget() 
          
          //... train for 1 stage, and update train_stats,
          // using train_set->getSample(input, target, weight)
          // and train_stats->update(train_costs)
          
          ++stage
          train_stats->finalize() // finalize statistics for this epoch
        }
    */
}


void StackedLearner::computeOutput(const Vec& input, Vec& output) const
{
  // Compute the output from the input
  // int nout = outputsize();
  // output.resize(nout);
  // ...
}    

void StackedLearner::computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                           const Vec& target, Vec& costs) const
{
// Compute the costs from *already* computed output. 
// ...
}                                

TVec<string> StackedLearner::getTestCostNames() const
{
  // Return the names of the costs computed by computeCostsFromOutpus
  // (these may or may not be exactly the same as what's returned by getTrainCostNames)
  // ...
}

TVec<string> StackedLearner::getTrainCostNames() const
{
  // Return the names of the objective costs that the train method computes and 
  // for which it updates the VecStatsCollector train_stats
  // (these may or may not be exactly the same as what's returned by getTestCostNames)
  // ...
}



%> // end of namespace PLearn
