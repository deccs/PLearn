// -*- C++ -*-

// AdaBoost.cc
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
   * $Id: AdaBoost.cc,v 1.1 2005/01/06 18:45:11 larocheh Exp $
   ******************************************************* */

// Authors: Yoshua Bengio

/*! \file AdaBoost.cc */

#include "AdaBoost.h"
#include <plearn/math/pl_math.h>
#include <plearn/vmat/ConcatColumnsVMatrix.h>
#include <plearn/vmat/SelectRowsVMatrix.h>
#include <plearn/vmat/MemoryVMatrix.h>
#include <plearn/math/random.h>

namespace PLearn {
using namespace std;

AdaBoost::AdaBoost()
  : target_error(0.5), output_threshold(0.5), compute_training_error(1), 
    pseudo_loss_adaboost(1), conf_rated_adaboost(0), weight_by_resampling(1), early_stopping(1),
    save_often(0)
  { }

PLEARN_IMPLEMENT_OBJECT(
  AdaBoost,
  "AdaBoost boosting algorithm for TWO-CLASS classification",
  "Given a classification weak-learner, this algorithm \"boosts\" it in\n"
  "order to obtain a much more powerful classification algorithm.\n"
  "The classifier is two-class, returning 0 or 1, or a number between 0 and 1\n"
  "(in that case the user can set the 'pseudo_loss_adaboost' option, which\n"
  "computes a more precise notion of error taking into account the precise\n"
  "value outputted by the soft classifier).\n"
  "The nstages option from PLearner is used to specify the desired\n"
  "number of boosting rounds (but the algorithm can stop earlier if\n"
  "the next weak learner is unable to unable to make significant progress.\n");

void AdaBoost::declareOptions(OptionList& ol)
{
  declareOption(ol, "weak_learners", &AdaBoost::weak_learners,
                OptionBase::learntoption,
                "The vector of learned weak learners");

  declareOption(ol, "voting_weights", &AdaBoost::voting_weights,
                OptionBase::learntoption,
                "Weights given to the weak learners (their output is linearly combined with these weights\n"
                "to form the output of the AdaBoost learner).\n");

  declareOption(ol, "sum_voting_weights", &AdaBoost::sum_voting_weights,
                OptionBase::learntoption,
                "Sum of the weak learners voting weights.\n");
  
  declareOption(ol, "initial_sum_weights", &AdaBoost::initial_sum_weights,
                OptionBase::learntoption,
                "Initial sum of weights on the examples. Do not temper with.\n");

  declareOption(ol, "weak_learner_template", &AdaBoost::weak_learner_template,
                OptionBase::buildoption,
                "Template for the regression weak learner to be boosted into a classifier");

  declareOption(ol, "target_error", &AdaBoost::target_error,
                OptionBase::buildoption,
                "This is the target average weighted error below which each weak learner"
                "must reach after its training (ordinary adaboost: target_error=0.5).");

  declareOption(ol, "pseudo_loss_adaboost", &AdaBoost::pseudo_loss_adaboost,
                OptionBase::buildoption,
                "Whether to use a variant of AdaBoost which is appropriate for soft classifiers\n"
                "whose output is between 0 and 1 rather than being either 0 or 1.\n");

  declareOption(ol, "conf_rated_adaboost", &AdaBoost::conf_rated_adaboost,
                OptionBase::buildoption,
                "Whether to use a version of Adaboost appropriate for weak learners that can\n"
                "give a confidence rate to their predictions.\n");

  declareOption(ol, "weight_by_resampling", &AdaBoost::weight_by_resampling,
                OptionBase::buildoption,
                "Whether to train the weak learner using resampling to represent the weighting\n"
                "given to examples. If false then give these weights explicitly in the training set\n"
                "of the weak learner (note that some learners can accomodate weights well, others not).\n");

  declareOption(ol, "output_threshold", &AdaBoost::output_threshold,
                OptionBase::buildoption,
                "To interpret the output of the learner as a class, it is compared to this\n"
                "threshold: class 1 if greather than output_threshold, class 0 otherwise.\n");

  declareOption(ol, "provide_learner_expdir", &AdaBoost::provide_learner_expdir, OptionBase::buildoption,
                "If true, each weak learner to be trained will have its experiment directory set to WeakLearner#kExpdir/");

  declareOption(ol, "early_stopping", &AdaBoost::early_stopping, OptionBase::buildoption,
                "If true, then boosting stops when the next weak learner is too weak (avg error > target_error - .01)\n");

  declareOption(ol, "save_often", &AdaBoost::save_often, OptionBase::buildoption,
                "If true, then save the model after training each weak learner, under <expdir>/model.psave\n");

  declareOption(ol, "compute_training_error", &AdaBoost::compute_training_error, OptionBase::buildoption,
                "Whether to compute training error at each stage.\n");

  // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

void AdaBoost::build_()
{
  if(conf_rated_adaboost && pseudo_loss_adaboost)
    PLERROR("In Adaboost:build_(): conf_rated_adaboost and pseudo_loss_adaboost cannot both be true, a choice must be made");
}

// ### Nothing to add here, simply calls build_
void AdaBoost::build()
{
  inherited::build();
  build_();
}


void AdaBoost::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);

  deepCopyField(learners_error, copies);
  deepCopyField(example_weights, copies);
  deepCopyField(voting_weights, copies);
  deepCopyField(weak_learners, copies);
  deepCopyField(weak_learner_template, copies);
}


int AdaBoost::outputsize() const
{
  // Outputsize is always 1, since this is a 0-1 classifier
  return 1;
}

void AdaBoost::forget()
{
  stage = 0;
  learners_error.resize(0, nstages);
  weak_learners.resize(0, nstages);
  voting_weights.resize(0, nstages);
  sum_voting_weights = 0;
  if (seed_ >= 0)
    manual_seed(seed_);
  else
    PLearn::seed();
}

void AdaBoost::train()
{
  if(!train_set)
    PLERROR("In AdaBoost::train, you did not setTrainingSet");
    
  if(!train_stats)
    PLERROR("In AdaBoost::train, you did not setTrainStatsCollector");

  if (train_set->targetsize()!=1)
    PLERROR("In AdaBoost::train, targetsize should be 1, found %d", train_set->targetsize());

  if (nstages < stage)        //!< Asking to revert to previous stage
    forget();

  static Vec input;
  static Vec output;
  static Vec target;
  real weight;

  static Vec examples_error;

  const int n = train_set.length();
  static TVec<int> train_indices;
  static Vec pseudo_loss;

  input.resize(inputsize());
  output.resize(1);
  target.resize(targetsize());
  examples_error.resize(n);

  if (stage==0)
  {
    example_weights.resize(n);
    if (train_set->weightsize()>0)
    {
      ProgressBar *pb=0;
      if(report_progress) pb = new ProgressBar("AdaBoost round " + tostring(stage) +
                     ": extracting initial weights", n);
      initial_sum_weights=0;
      for (int i=0; i<n; ++i) {
        if(report_progress) pb->update(i);
        train_set->getExample(i, input, target, weight);
        example_weights[i]=weight;
        initial_sum_weights += weight;
      }
      if(report_progress) delete(pb);
      example_weights *= real(1.0)/initial_sum_weights;
    }
    else 
    {
      example_weights.fill(1.0/n);
      initial_sum_weights = 1;
    }
    sum_voting_weights = 0;
    voting_weights.resize(0,nstages);
  }

  VMat unweighted_data = train_set.subMatColumns(0, inputsize()+1);
  learners_error.resize(nstages);

  for ( ; stage < nstages ; ++stage)
  {
    VMat weak_learner_training_set;
    { 
      ProgressBar *pb=0;
      if(report_progress) pb = new ProgressBar("AdaBoost round " + tostring(stage) +
                     ": making training set for weak learner", n);
      // We shall now construct a training set for the new weak learner:
      if (weight_by_resampling)
      {
        // use a "smart" resampling that approximated sampling with replacement
        // with the probabilities given by example_weights.
        map<real,int> indices;
        for (int i=0; i<n; ++i) {
          if(report_progress) pb->update(i);
          real p_i = example_weights[i];
          int n_samples_of_row_i = int(rint(gaussian_mu_sigma(n*p_i,sqrt(n*p_i*(1-p_i))))); // randomly choose how many repeats of example i
          for (int j=0;j<n_samples_of_row_i;j++)
          {
            if (j==0)
              indices[i]=i;
            else
            {
              real k=n*uniform_sample(); // put the others in random places
              indices[k]=i; // while avoiding collisions
            }
          }
        }
        if(report_progress) delete(pb);
        train_indices.resize(0,n);
        map<real,int>::iterator it = indices.begin();
        map<real,int>::iterator last = indices.end();
        for (;it!=last;++it)
          train_indices.push_back(it->second);
        weak_learner_training_set = new SelectRowsVMatrix(unweighted_data, train_indices);
        weak_learner_training_set->defineSizes(inputsize(), 1, 0);
      }
      else
      {
        Mat data_weights_column = example_weights.toMat(n,1).copy();
        data_weights_column *= initial_sum_weights; // to bring the weights to the same average level as the original ones
        VMat data_weights = VMat(data_weights_column);
        weak_learner_training_set = new ConcatColumnsVMatrix(unweighted_data,data_weights);
        weak_learner_training_set->defineSizes(inputsize(), 1, 1);
      }
    }

    // Create new weak-learner and train it
    PP<PLearner> new_weak_learner = ::PLearn::deepCopy(weak_learner_template);
    new_weak_learner->setTrainingSet(weak_learner_training_set);
    new_weak_learner->setTrainStatsCollector(new VecStatsCollector);
    /*
    string file = "train_" + tostring(stage);
    MemoryVMatrix *temp_train_set = new MemoryVMatrix(weak_learner_training_set);
    PLearn::save(file,temp_train_set->data);
    */
    if(expdir!="" && provide_learner_expdir)
      new_weak_learner->setExperimentDirectory(append_slash(expdir+"WeakLearner" + tostring(stage) + "Expdir"));

    new_weak_learner->train();

    // calculate its weighted training error 
    {
      ProgressBar *pb=0;
      if(report_progress) pb = new ProgressBar("computing weighted training error of weak learner",n);
      learners_error[stage] = 0;
      for (int i=0; i<n; ++i) {
        if(report_progress) pb->update(i);
        train_set->getExample(i, input, target, weight);
        new_weak_learner->computeOutput(input,output);
        real y_i=target[0];
        real f_i=output[0];
        if(conf_rated_adaboost)
        {          
          examples_error[i] = 2*(f_i+y_i-2*f_i*y_i);
          learners_error[stage] += example_weights[i]*examples_error[i];
        }
        else
        {
          if (pseudo_loss_adaboost) // an error between 0 and 1 (before weighting)
          {
            examples_error[i] = 0.5*(f_i+y_i-2*f_i*y_i);  
            learners_error[stage] += example_weights[i]*examples_error[i];
          }
          else
          {
            if (y_i==1)
            {
              if (f_i<output_threshold)
              {
                learners_error[stage] += example_weights[i];
                examples_error[i]=1;
              }
              else examples_error[i] = 0;
            }
            else
            {
              if (f_i>=output_threshold) {
                learners_error[stage] += example_weights[i];
                examples_error[i]=1;
              }
              else examples_error[i]=0;
            }
          }
        }
      }
      if(report_progress) delete(pb);
    }

    if (verbosity>1)
      cout << "weak learner at stage " << stage << " has average loss = " << learners_error[stage] << endl;

    // stopping criterion (in addition to n_stages)
    if (early_stopping && (learners_error[stage] == 0 || learners_error[stage] > target_error - 0.01))
    {
      nstages = stage;
      cout << "AdaBoost::train early stopping because learner's loss at stage " << stage << " is " << learners_error[stage] << endl;
      break;
    }

    weak_learners.push_back(new_weak_learner);

    if (save_often && expdir!="")
      PLearn::save(append_slash(expdir)+"model.psave", *this);
      
    // compute the new learner's weight

    if(conf_rated_adaboost)
    {
      // Find optimal weight with line search, blame Norman if this doesn't work ;) 
      
      real ax = -10;
      real bx = 1;
      real cx = 100;
      real xmin;
      real tolerance = 0.001;
      int itmax = 100000;

      int iter;
      real xtmp;
      real fa, fb, fc, ftmp;

      // compute function for fa, fb and fc

      fa = 0;
      fb = 0;
      fc = 0;

      for (int i=0; i<n; ++i) {
        train_set->getExample(i, input, target, weight);
        new_weak_learner->computeOutput(input,output);
        real y_i=(2*target[0]-1);
        real f_i=(2*output[0]-1);
        fa += example_weights[i]*exp(-1*ax*f_i*y_i);
        fb += example_weights[i]*exp(-1*bx*f_i*y_i);
        fc += example_weights[i]*exp(-1*cx*f_i*y_i);
      }

        
      for(iter=1;iter<=itmax;iter++)
      {
        if(verbosity>4)
          cout << "iteration " << iter << ": fx = " << fb << endl;
        if (abs(cx-ax) <= tolerance)
        {
          xmin=bx;
          if(verbosity>3)
          {
            cout << "nIters for minimum: " << iter << endl;
            cout << "xmin = " << xmin << endl;
            cout << "fx = " << fb << endl;
          }
          break;
        }
        if (abs(bx-ax) > abs(bx-cx)) 
        {
          xtmp = (bx + ax) * 0.5;

          ftmp = 0;
          for (int i=0; i<n; ++i) {
            train_set->getExample(i, input, target, weight);
            new_weak_learner->computeOutput(input,output);
            real y_i=(2*target[0]-1);
            real f_i=(2*output[0]-1);
            ftmp += example_weights[i]*exp(-1*xtmp*f_i*y_i);
          }

          if (ftmp > fb)
          {
            ax = xtmp;
            fa = ftmp;
          }
          else
          {
            cx = bx;
            fc = fb;
            bx = xtmp;
            fb = ftmp;
          }
        }
        else
        {
          xtmp = (bx + cx) * 0.5;
          ftmp = 0;
          for (int i=0; i<n; ++i) {
            train_set->getExample(i, input, target, weight);
            new_weak_learner->computeOutput(input,output);
            real y_i=(2*target[0]-1);
            real f_i=(2*output[0]-1);
            ftmp += example_weights[i]*exp(-1*xtmp*f_i*y_i);
          }

          if (ftmp > fb)
          {
            cx = xtmp;
            fc = ftmp;
          }
          else
          {
            ax = bx;
            fa = fb;
            bx = xtmp;
            fb = ftmp;
          }
        }
      }
      if(verbosity>3)
      {
        cout << "Too many iterations in Brent" << endl;
      }
      xmin=bx;
      voting_weights.push_back(xmin);
      sum_voting_weights += abs(voting_weights[stage]);
    }
    else
    {
      voting_weights.push_back(0.5*safeflog(((1-learners_error[stage])*target_error)/(learners_error[stage]*(1-target_error))));
      sum_voting_weights += abs(voting_weights[stage]);
    }

    real sum_w=0;
    for (int i=0;i<n;i++)
    {
      example_weights[i] *= exp(-voting_weights[stage]*(1-examples_error[i]));
      sum_w += example_weights[i];
    }
    example_weights *= real(1.0)/sum_w;

    if (compute_training_error)
    {
      {
        ProgressBar *pb=0;
        if(report_progress) pb = new ProgressBar("computing weighted training error of whole model",n);
        train_stats->forget();
        static Vec err(1);
        for (int i=0;i<n;i++)
        {
          if(report_progress) pb->update(i);
          train_set->getExample(i, input, target, weight);
          computeCostsOnly(input,target,err);
          train_stats->update(err);
        }
        if(report_progress) delete(pb);
        train_stats->finalize();
      }
      if (verbosity>2)
        cout << "At stage " << stage << " boosted (weighted) classification error on training set = " << train_stats->getMean() << endl;
     
    }
  }
}


void AdaBoost::computeOutput(const Vec& input, Vec& output) const
{
  output.resize(1);
  real sum_out=0;
  static Vec weak_learner_output(1);
  for (int i=0;i<voting_weights.length();i++)
  {
    weak_learners[i]->computeOutput(input,weak_learner_output);
    if(!pseudo_loss_adaboost && !conf_rated_adaboost)
      sum_out += (weak_learner_output[0] < output_threshold ? 0 : 1) *voting_weights[i];
    else
      sum_out += weak_learner_output[0]*voting_weights[i];
  }
  output[0] = sum_out/sum_voting_weights;
}

void AdaBoost::computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                       const Vec& target, Vec& costs) const
{
  costs.resize(2);

  // First cost is negative log-likelihood...  output[0] is the likelihood
  // of the first class
  if (target.size() > 1)
    PLERROR("AdaBoost::computeCostsFromOutputs: target must contain "
            "one element only: the 0/1 class");
  if (target[0] == 0) {
    costs[0] = output[0] >= output_threshold; 
  }
  else if (target[0] == 1) {
    costs[0] = output[0] < output_threshold; 
  }
  else PLERROR("AdaBoost::computeCostsFromOutputs: target must be "
               "either 0 or 1; current target=%f", target[0]);
  costs[1] = exp(-1.0*sum_voting_weights*(2*output[0]-1)*(2*target[0]-1));
}

TVec<string> AdaBoost::getTestCostNames() const
{
  return getTrainCostNames();
}

TVec<string> AdaBoost::getTrainCostNames() const
{
  TVec<string> costs(2);
  costs[0] = "binary_class_error";
  costs[1] = "exp_neg_margin";
  return costs;
}

} // end of namespace PLearn
