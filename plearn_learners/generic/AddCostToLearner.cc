// -*- C++ -*-

// AddCostToLearner.cc
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
   * $Id: AddCostToLearner.cc,v 1.3 2004/03/16 18:37:12 tihocan Exp $ 
   ******************************************************* */

// Authors: Olivier Delalleau

/*! \file AddCostToLearner.cc */


#include "AddCostToLearner.h"
#include "CrossEntropyVariable.h"
#include "VarArray.h"
#include "VecElementVariable.h"

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(AddCostToLearner,
    "A PLearner that just adds additional costs to another PLearner.",
    "Feel free to make this class evolve by adding new costs, or rewriting it\n"
    "in a better fashion, because this one is certainly not perfect.\n"
    "To use the lift cost, do the following:\n"
    " (1) add a cost of type 1 to this object's option 'costs'\n"
    " (2) replace the template_stats_collector of your PTester with one like this:\n"
    "   template_stats_collector =\n"
    "     LiftStatsCollector (\n"
    "      lift_fraction = 0.1 ;\n"
    "      output_column = \"lift_output\" ;\n"
    "      opposite_lift = 0 ; # to set to 1 if we want to optimize it\n"
    "      sign_trick = 1 ;\n"
    " (3) ask for the lift in the stats:\n"
    "   statnames = [\n"
    "     \"E[test1.LIFT]\"\n"
    "     \"E[test1.LIFT_MAX]\"\n"
    "   ];" );

//////////////////////
// AddCostToLearner //
//////////////////////
AddCostToLearner::AddCostToLearner()
: check_output_consistency(1),
  from_max(1),
  from_min(-1),
  rescale_output(0),
  rescale_target(0),
  to_max(1),
  to_min(0)
{}

////////////////////
// declareOptions //
////////////////////
void AddCostToLearner::declareOptions(OptionList& ol)
{
  declareOption(ol, "check_output_consistency", &AddCostToLearner::check_output_consistency, OptionBase::buildoption,
      "If set to 1, additional checks will be performed to make sure the output\n"
      "is compatible with the costs to be computed. This may slow down the costs\n"
      "computation, but is also safer.");

  declareOption(ol, "costs", &AddCostToLearner::costs, OptionBase::buildoption,
      "The costs to be added:\n"
      " - 1 : 'lift_output', used to compute the lift cost\n"
      " - 2 : 'cross_entropy', the cross entropy cost t*log(o) + (1-t)*log(1-o)");

  declareOption(ol, "force_output_to_target_interval", &AddCostToLearner::force_output_to_target_interval, OptionBase::buildoption,
      "If set to 1 and 'rescale_output' is also set to 1, then the scaled output\n"
      "will be forced to belong to [to_min, to_max], which may not be the case otherwise\n"
      "if the output doesn't originate from [from_min, from_max].");
      
  declareOption(ol, "from_max", &AddCostToLearner::from_max, OptionBase::buildoption,
      "Upper bound of the source interval [from_min, from_max] (used in rescaling).");

  declareOption(ol, "from_min", &AddCostToLearner::from_min, OptionBase::buildoption,
      "Lower bound of the source interval [from_min, from_max] (used in rescaling).");

  declareOption(ol, "rescale_output", &AddCostToLearner::rescale_output, OptionBase::buildoption,
      "If set to 1, then the output will be rescaled before computing the costs, according\n"
      "to the values of from_min, from_max, to_min, to_max. This means it will map\n"
      "[from_min, from_max] to [to_min, to_max].");

  declareOption(ol, "rescale_target", &AddCostToLearner::rescale_target, OptionBase::buildoption,
      "Same as 'rescale_output', but for the target.");

  declareOption(ol, "sub_learner", &AddCostToLearner::sub_learner, OptionBase::buildoption,
      "The learner to which we add the costs.");

  declareOption(ol, "to_max", &AddCostToLearner::to_max, OptionBase::buildoption,
      "Upper bound of the destination interval [to_min, to_max] (used in rescaling).");

  declareOption(ol, "to_min", &AddCostToLearner::to_min, OptionBase::buildoption,
      "Lower bound of the destination interval [to_min, to_max] (used in rescaling).");

  // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

///////////
// build //
///////////
void AddCostToLearner::build()
{
  inherited::build();
  build_();
}

////////////
// build_ //
////////////
void AddCostToLearner::build_()
{
  // Make sure all costs are valid.
  int n = costs.length();
  int min_verb = 2;
  bool display = (verbosity >= min_verb);
  if (n > 0) {
    if (display) {
      cout << "Additional costs computed: ";
    }
    int os = sub_learner->outputsize();
    sub_learner_output.resize(os);
    desired_target.resize(os);
    if (rescale_output || rescale_target) {
      real from_fac = from_max - from_min;
      real to_fac = to_max - to_min;
      fac = to_fac / from_fac;
    }
    output_min = -REAL_MAX;
    output_max = REAL_MAX;
    for (int i = 0; i < n; i++) {
      switch(costs[i]) {
        case 1:
          if (display) cout << "lift_output ";
          // Output should be positive.
          output_min = max(output_min, real(0));
          break;
        case 2:
          if (display) cout << "cross_entropy ";
          // Output should be in [0,1].
          output_min = max(output_min, real(0));
          output_max = min(output_max, real(1));
          {
            Var zero = var(0);
            output_var = accessElement(sub_learner_output, zero);
            target_var = accessElement(desired_target, zero);
            cross_entropy_var = cross_entropy(output_var, target_var);
            cross_entropy_prop = propagationPath(cross_entropy_var);
          }
          break;
        default:
          PLERROR("In AddCostToLearner::build_ - Invalid cost requested");
          break;
      }
    }
    cout << endl;
  }
}

/////////////////////////////
// computeCostsFromOutputs //
/////////////////////////////
void AddCostToLearner::computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                           const Vec& target, Vec& costs) const
{
  int n_original_costs = sub_learner->nTestCosts();
  // We give only costs.subVec to the sub_learner because it may want to resize it.
  Vec sub_costs = costs.subVec(0, n_original_costs);
  sub_learner->computeCostsFromOutputs(input, output, target, sub_costs);

  // Optional rescaling.
  if (!rescale_output) {
    sub_learner_output << output;
  } else {
    int n = output.length();
    real scaled_output;
    for (int i = 0; i < n; i++) {
      scaled_output = (output[i] - from_min) * fac + to_min;
      if (force_output_to_target_interval) {
        if (scaled_output > to_max) {
          scaled_output = to_max;
        } else if (scaled_output < to_min) {
          scaled_output = to_min;
        }
      }
      sub_learner_output[i] = scaled_output;
    }
  }
  if (!rescale_target) {
    desired_target << target;
  } else {
    int n = output.length();
    for (int i = 0; i < n; i++) {
      desired_target[i] = (target[i] - from_min) * fac + to_min;
    }
  }

  if (check_output_consistency) {
    for (int i = 0; i < sub_learner_output; i++) {
      real out = sub_learner_output[i];
      if (out < output_min) {
        PLERROR("In AddCostToLearner::computeCostsFromOutputs - Sub-learner output (%f) is lower than %f", out, output_min);
      }
      if (out > output_max) {
        PLERROR("In AddCostToLearner::computeCostsFromOutputs - Sub-learner output (%f) is higher than %f", out, output_max);
      }
    }
  }

  for (int i = 0; i < this->costs.length(); i++) {
    switch(this->costs[i]) {
      case 1: // Lift.
        // TODO Using a LiftOutputVariable would be better.
#ifdef BOUNDCHECK
        if (sub_learner_output.length() != 1 || desired_target.length() != 1) {
          PLERROR("In AddCostToLearner::computeCostsFromOutputs - Lift cost is only meant to be used with one-dimensional output and target");
        }
#endif
        {
          // The 'lift cost', which actually isn't a cost, is the output when
          // the target is 1, and -output when the target is 0
#ifdef BOUNDCHECK
          if (desired_target[0] != 0 && desired_target[0] != 1) {
            // Invalid target.
            PLERROR("In AddCostToLearner::computeCostsFromOutputs - Target isn't compatible with lift");
          }
#endif
          if (desired_target[0] == 1) {
            costs[i + n_original_costs] = sub_learner_output[0];
          } else {
            costs[i + n_original_costs] = - sub_learner_output[0];
          }
        }
        break;
      case 2: // Cross entropy.
#ifdef BOUNDCHECK
          if (desired_target[0] != 0 && desired_target[0] != 1) {
            // Invalid target.
            PLERROR("In AddCostToLearner::computeCostsFromOutputs - Target isn't compatible with cross_entropy");
          }
#endif
        cross_entropy_prop.fprop();
        costs[i + n_original_costs] = cross_entropy_var->valuedata[0];
        break;
      default:
        break;
    }
  }
}                                

///////////////////
// computeOutput //
///////////////////
void AddCostToLearner::computeOutput(const Vec& input, Vec& output) const
{
  sub_learner->computeOutput(input, output);
}    

////////////
// forget //
////////////
void AddCostToLearner::forget()
{
  sub_learner->forget();
}
    
//////////////////////
// getTestCostNames //
//////////////////////
TVec<string> AddCostToLearner::getTestCostNames() const
{
  // Return the names of the costs computed by computeCostsFromOutpus
  // (these may or may not be exactly the same as what's returned by getTrainCostNames)
  // ...
  TVec<string> sub_costs = sub_learner->getTestCostNames();
  for (int i = 0; i < this->costs.length(); i++) {
    switch(costs[i]) {
      case 1: // Lift.
        sub_costs.append("lift_output");
        break;
      case 2: // Cross entropy.
        sub_costs.append("cross_entropy");
        break;
      default:
        break;
    }
  }
  return sub_costs;
}

///////////////////////
// getTrainCostNames //
///////////////////////
TVec<string> AddCostToLearner::getTrainCostNames() const
{
  // The lift is only a test cost.
  return sub_learner->getTrainCostNames();
}

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void AddCostToLearner::makeDeepCopyFromShallowCopy(map<const void*, void*>& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);

  // ### Call deepCopyField on all "pointer-like" fields 
  // ### that you wish to be deepCopied rather than 
  // ### shallow-copied.
  // ### ex:
  // deepCopyField(trainvec, copies);

  // ### Remove this line when you have fully implemented this method.
  PLERROR("AddCostToLearner::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}

////////////////
// outputsize //
////////////////
int AddCostToLearner::outputsize() const
{
  return sub_learner->outputsize();
}

///////////
// train //
///////////
void AddCostToLearner::train()
{
  sub_learner->train();
}

} // end of namespace PLearn
