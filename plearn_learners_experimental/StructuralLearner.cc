// -*- C++ -*-

// StructuralLearner.cc
//
// Copyright (C) 2006 Pierre-Antoine Manzagol 
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
   * $Id: .pyskeleton_header 544 2003-09-01 00:05:31Z plearner $ 
   ******************************************************* */

// Authors: Pierre-Antoine Manzagol

/*! \file StructuralLearner.cc */


#include "StructuralLearner.h"
#include <plearn/math/plapack.h>
#include <plearn/math/random.h>

#include <map>
#include <vector>
#include <algorithm>

// *** Used to determine most frequent words in auxiliary set ***
class freqCount {

public:
  freqCount(int wt, unsigned long int c) : wordtag(wt), count(c){};
  int wordtag;
  unsigned long int count;
};

bool freqCountGT(const freqCount &a, const freqCount &b) 
{
    return a.count > b.count;
}


namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(
    StructuralLearner,
    "ONE LINE DESCRIPTION",
    "MULTI-LINE \nHELP");

StructuralLearner::StructuralLearner() 
/* ### Initialize all fields to their default value here */
{

  std::cerr << "StructuralLearner::StructuralLearner()" << std::endl;

  // With these values, will not learn
  start_learning_rate=0.0;
  decrease_constant=0.0;
  lambda=0.0001;
  index_O = 1;
  nhidden = 0;
  separate_features = 1;
  n_auxiliary_wordproblems = 400;
  epsilon = 1e-4;
  //m_tvec_auxiliaryLearners.resize(0);
  
  // ### You may (or not) want to call build_() to finish building the object
  // ### (doing so assumes the parent classes' build_() have been called too
  // ### in the parent classes' constructors, something that you must ensure)
}

void StructuralLearner::declareOptions(OptionList& ol)
{
    declareOption(ol, "ws", &StructuralLearner::ws, OptionBase::learntoption,
                   "Weights of the linear classifier: f(x) = wt x + vt theta x");
    declareOption(ol, "vs", &StructuralLearner::vs, OptionBase::learntoption,
                   "Weights of the linear classifier: f(x) = wt x + vt theta x");
    declareOption(ol, "whids", &StructuralLearner::whids, OptionBase::learntoption,
                   "Weights from input to hidden layers (one for each feature group)");
    declareOption(ol, "thetas", &StructuralLearner::thetas, OptionBase::learntoption,
                   "structure parameter of the linear classifier: f(x) = wt x + vt theta x");
    declareOption(ol, "start_learning_rate", &StructuralLearner::start_learning_rate, OptionBase::buildoption,
                   "Starting learning rate of the stochastic gradient descent");
    declareOption(ol, "decrease_constant", &StructuralLearner::decrease_constant, OptionBase::buildoption,
                   "Decrease constant of the stochastic learning rate");
    declareOption(ol, "auxiliary_task_train_set", &StructuralLearner::auxiliary_task_train_set, OptionBase::buildoption,
                   "Training set for auxiliary task");
    declareOption(ol, "epsilon", &StructuralLearner::epsilon, OptionBase::buildoption,
                   "Threshold to determine convergence of stochastic descent");
    declareOption(ol, "lambda", &StructuralLearner::lambda, OptionBase::buildoption,
                   "Weight decay for output weights");
    declareOption(ol, "nhidden", &StructuralLearner::nhidden, OptionBase::buildoption,
                   "Number of hidden neurons in the hidden layers");
    declareOption(ol, "index_O", &StructuralLearner::index_O, OptionBase::buildoption,
                   "Index of the \"O\" (abstention) symbol");
    declareOption(ol, "separate_features", &StructuralLearner::separate_features, OptionBase::buildoption,
                   "Indication that the features should be separated into groups");
    declareOption(ol, "n_auxiliary_wordproblems", &StructuralLearner::n_auxiliary_wordproblems, OptionBase::buildoption,
                   "Number of most frequent words that are to be predicted.");




    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void StructuralLearner::buildTasksParameters(int nout, TVec<unsigned int> feat_lengths)
{

    // Do your thing! (Gotta love those comments) (Remember to search for nasty
    // words in plearn code - has to be some nice things)

    before_softmax.resize(nout);
    output.resize(nout);

    if(!separate_features)
    {
        if(nhidden > 0) 
        {
            ws.resize(1);
            whids.resize( feat_lengths.length() );
        }
        else
            ws.resize(feat_lengths.length());
        vs.resize(1);        
    }
    else
    {
        ws.resize( feat_lengths.length() );
        vs.resize( feat_lengths.length() );
        if(nhidden > 0) 
        {
            whids.resize( feat_lengths.length() );
        }
    }

    for(int i=0; i<ws.length(); i++)  {
        if(nhidden>0)
            ws[i].resize( nout, nhidden +1);  // +1 for the bias
        else
            ws[i].resize( nout, feat_lengths[i] );
    }

    for(int i=0; i<vs.length(); i++)  {
        if(nhidden>0)
            vs[i].resize( nout, 50 );
        else
            vs[i].resize( nout, 50 );
    }

    for(int i=0; i<whids.length(); i++)  {
            whids[i].resize(nhidden,feat_lengths[i]);
    }

    if(nhidden > 0)
    {
        if(separate_features)
            activations.resize(nhidden+1,feat_lengths.length()); // +1 for the bias
        else
            activations.resize(nhidden+1,1); // idem
    }

}

void StructuralLearner::buildThetaParameters(TVec<unsigned int> feat_lengths)
{
    if(separate_features)
        thetas.resize( feat_lengths.length()-2 );  // Do not consider features for previous tags!
    else       
        thetas.resize(1);

    if(separate_features)
        thetas_times_x.resize( 50, feat_lengths.length() );
    else
        thetas_times_x.resize(50,1);
    
    for(int i=0; i<thetas.length(); i++)  {
        if(nhidden>0)
        {
            thetas[i].resize( 50, nhidden +1); // +1 for the bias
        }
        else
        {
            thetas[i].resize( 50, feat_lengths[i] );
        }
    }
}

// For now everything is done in the train. For sure, that's not a good, for example if we want to
// reload ...
void StructuralLearner::build_()
{
    std::cerr << "StructuralLearner::build_()" << std::endl;

  if(train_set)
  {
    // ***** Sanity checks
    if(weightsize_ < 0)
      PLWARNING("In StructuralLearner::build_(): negative weightsize_");
    if(weightsize_ > 0)
      PLWARNING("In StructuralLearner::build_(): does not support weighting of the training set");
    if(targetsize_ < 0)
      PLWARNING("In StructuralLearner::build_(): negative targetsize_");
    if(targetsize_ > 1)
      PLWARNING("In StructuralLearner::build_(): multi-target learning is not supported, only one (hardcoded) target will be considered");

    // ***** Resize vectors
    input.resize(inputsize());
    target.resize(targetsize());
    costs.resize(getTrainCostNames().length());
    
    // - resize and initialize ws, vs and thetas and thetas_times_x
    
    // dummy call to computeFeatures in order to set fls
    computeFeatures(input, target, 0, 0, feats);

    if(auxiliary_task_train_set)
        buildTasksParameters(2*n_auxiliary_wordproblems,fls);
    else
        buildTasksParameters(outputsize(),fls);

    if(auxiliary_task_train_set)
        buildThetaParameters(fls);

    initializeParams();

    // TODO:
    // - create auxiliary task (if auxiliary_task_train_set != 0)
    if( auxiliary_task_train_set && auxiliary_indices_left.size()==0) {
      initWordProblemsStructures();
    }

  }// if we have a train_set
}

// ### Nothing to add here, simply calls build_
void StructuralLearner::build()
{
    inherited::build();
    build_();
}


void StructuralLearner::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    deepCopyField(thetas, copies);
    deepCopyField(thetas_times_x, copies);
    deepCopyField(auxiliary_task_train_set,copies);
    deepCopyField(ws,copies);
    deepCopyField(vs,copies);
    deepCopyField(whids,copies);
    deepCopyField(feats, copies);
    deepCopyField(input, copies);
    deepCopyField(target, copies);
    deepCopyField(activations, copies);
    deepCopyField(before_softmax, copies);
    deepCopyField(output, copies);
    deepCopyField(costs, copies);
    deepCopyField(auxiliary_indices_current, copies);
    deepCopyField(auxiliary_indices_left, copies);

    deepCopyField(currentFeatureGroup, copies);
    deepCopyField(fls, copies);

    // ### Remove this line when you have fully implemented this method.
    //PLERROR("StructuralLearner::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}


int StructuralLearner::outputsize() const
{
    return(train_set->getDictionary(inputsize_)->size() + (train_set->getDictionary(inputsize_)->oov_not_in_possible_values ? 0 : 1));
}

void StructuralLearner::forget()
{
    //! (Re-)initialize the PLearner in its fresh state (that state may depend on the 'seed' option)
    //! And sets 'stage' back to 0   (this is the stage of a fresh learner!)
    /*!
      A typical forget() method should do the following:
      - initialize a random number generator with the seed option
      - initialize the learner's parameters, using this random generator
      - stage = 0
    */
  if(ws.size()!=0)
    initializeParams();
    
  stage = 0;

}
    
void StructuralLearner::train()
{
  if (!initTrain())
      return;

  int nout = outputsize();

  // Compute thetas over auxiliary task,
  // if an auxiliary problem is given
  if( auxiliary_task_train_set && stage == 0)  {
      
      // Preprocessing of auxiliary task should be done by now!

      // Train initial weights ws
      std::cerr << "StructuralLearner::train() - Training learner" << std::endl;    

      // TODO: should I save features once and for all here
      // think of access to dictionaries...
      nout = ws[0].length();
      real best_error=REAL_MAX;
      real current_error=REAL_MAX/2;
      int it = 0;
    
      while(current_error < best_error - epsilon)  {
          // TODO: is this a good clear?
          //token_prediction.clear();
          best_error = current_error;
          train_stats->forget();
          learning_rate = start_learning_rate / (1+decrease_constant*it);          
          for(int i=0; i<auxiliary_indices_current.length()+auxiliary_indices_left.length(); i++)  {
              if(i<auxiliary_indices_current.length())
              {
                  auxiliary_task_train_set->getExample(auxiliary_indices_current(i,0), input, target, weight);
                  target.resize(5);
                  target.fill(MISSING_VALUE);
                  target[2] = auxiliary_indices_current(i,1);
                  computeFeatures(input,target,1,i,feats,27);
                  computeOutputWithFeatures(feats,output,false,0,n_auxiliary_wordproblems); 
              }
              else
              {
                  auxiliary_task_train_set->getExample(auxiliary_indices_left(i-auxiliary_indices_current.length(),0), input, target, weight);
                  target.resize(5);
                  target.fill(MISSING_VALUE);
                  target[2] = n_auxiliary_wordproblems+auxiliary_indices_left(i-auxiliary_indices_current.length(),1);

                  computeFeatures(input,target,1,i,feats,23);
                  computeOutputWithFeatures(feats,output,false,n_auxiliary_wordproblems,2*n_auxiliary_wordproblems); 
              }

              
              computeCostsFromOutputs(input, output, target, costs);
              train_stats->update(costs);
              
              // TODO: update dynamic features

              // Update weights
                            
              // TODO: update for neural network

              if(nhidden>0)
              {
                  // Output weights update
                  for(int f=0; f<(separate_features ? feats.length() : 1); f++)
                  {
                      for(int i=0; i<nout; i++) 
                      {                                        
                          // Update w
                          for(int j=0; j<nhidden+1; j++)  {
                              if(i!=target[2])  {
                                  ws[f](i, j) -= learning_rate*output[i]*activations(j,f) + (lambda != 0 ? 2*lambda*ws[f](i,j) : 0);
                              }
                              else  {
                                  ws[f](i, j) -= learning_rate*(output[i]-1)*activations(j,f) + (lambda != 0 ? 2*lambda*ws[f](i, j) : 0);
                              }
                          }                                                             
                      }
                  }

                  // Hidden weights update
                  for(int f=0; f<feats.length(); f++)
                  {
                      current_features = feats[f].data();
                      for(int j=0; j<nhidden; j++)  {
                          for(int k=0; k<feats[f].length(); k++)
                          {                          
                              for(int i=0; i<nout; i++) 
                              {                              
                                  if(i!=target[2])  {
                                      if(separate_features) whids[f](j, current_features[k]) -= learning_rate*output[i]*ws[f](i,j)*(1-mypow(activations(j,f),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                                      else whids[f](j, current_features[k]) -= learning_rate*output[i]*ws[0](i,j)*(1-mypow(activations(j,0),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                                  }
                                  else  {
                                      if(separate_features) whids[f](j, current_features[k]) -= learning_rate*(output[i]-1)*ws[f](i,j)*(1-mypow(activations(j,f),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                                      else whids[f](j, current_features[k]) -= learning_rate*(output[i]-1)*ws[0](i,j)*(1-mypow(activations(j,0),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                                  }
                              }
                          }
                      }
                  }
                  
              }
              else
              {
                  for(int f=0; f<feats.length(); f++)
                  {
                      current_features = feats[f].data();
                      for(int i=0; i<nout; i++) 
                      {                                        
                          // Update w
                          for(int j=0; j<feats[f].length(); j++)  {
                              if(i!=target[2])  {
                                  ws[f](i, current_features[j]) -= learning_rate*output[i] + (lambda != 0 ? 2*lambda*ws[f](i, current_features[j]) : 0);
                              }
                              else  {
                                  ws[f](i, current_features[j]) -= learning_rate*(output[i]-1) + (lambda != 0 ? 2*lambda*ws[f](i, current_features[j]) : 0);
                              }
                          }                                                             
                      }
                  }
              }
          }
          it++;
          train_stats->finalize();
          current_error = train_stats->getMean()[0];
          cout << "Current error = " << current_error << endl;
      }
      
      // Now, using computed theta to bias training
      // TODO: consider separate_features variable...
      // TODO: do this for neural network!
      Mat V;
      Vec D;          
      for(int f=0; f<thetas.length(); f++)
      {
          // Perform SVD
          std::cerr << "StructuralLearner::train() - Performing " << f << "th SVD" << std::endl;
          Mat U_t;
          if(separate_features)
              U_t= sqrt(lambda) * ws[f];
          else
          {
              Array<Mat> to_concat(ws.length());
              for(int m=0; m<to_concat.length(); m++)
                  to_concat[m] = ws[m];
              U_t = hconcat(to_concat);
          }

          std::cout << "U_t.length() " << U_t.length() << " U_t.width() " << U_t.width() << std::endl;
          
          // --- Faire la SVD
          lapackSVD(U_t, thetas[f], D, V, 'A');
          
          std::cout << "thetas[f].length() " << thetas[f].length() << " thetas[f].width() " << thetas[f].width() << std::endl;
          
          thetas[f] = thetas[f].subMatRows(0, 50);
      }

      // Resize and initialize ws, vs, whids, etc.      
      nout = outputsize();
      buildTasksParameters(nout,fls);
      initializeParams();
  }
  else
  {
      // Train target classifier
      std::cerr << "StructuralLearner::train() - Training target classifier" << std::endl;                
      std::cerr << "StructuralLearner::train() - stage is " << stage << std::endl;
      
      train_stats->forget();
      learning_rate = start_learning_rate / (1+decrease_constant*stage);

      // TODO: is this a good clear?
      //token_prediction.clear();
      for(int t=0; t<train_set->length(); t++)  {
              
          train_set->getExample(t, input, target, weight);
          computeFeatures(input,target,0,t,feats);
          // 1) compute the output
          computeOutputWithFeatures(feats,output,auxiliary_task_train_set) ; 
          // 2) compute the cost      
          computeCostsFromOutputs(input, output, target, costs);
          train_stats->update(costs);
          // TODO: verify if OK
          //updateDynamicFeatures(token_prediction_train,input[3*2],target[2]);
          // 3) Update weights                           

          if(nhidden>0)
          {
              // Output weights update
              for(int f=0; f<(separate_features ? feats.length() : 1); f++)
              {
                  for(int i=0; i<nout; i++) 
                  {                                        
                      // Update w
                      for(int j=0; j<nhidden+1; j++)  {
                          if(i!=target[2])  {
                              ws[f](i, j) -= learning_rate*output[i]*activations(j,f) + (lambda != 0 ? 2*lambda*ws[f](i,j) : 0);
                          }
                          else  {
                              ws[f](i, j) -= learning_rate*(output[i]-1)*activations(j,f) + (lambda != 0 ? 2*lambda*ws[f](i, j) : 0);
                          }
                      } 
                      if(auxiliary_task_train_set)
                      {
                          // Update v
                          for(int j=0; j<50; j++)  {
                              if(i!=target[2])  {
                                  vs[f](i, j) -= learning_rate*output[i]*thetas_times_x(j,f);
                              }
                              else  {
                                  vs[f](i, j) -= learning_rate*(output[i]-1)*thetas_times_x(j,f);
                              }
                          }
                      }
                  }
              }

              // Hidden weights update
              for(int f=0; f<feats.length(); f++)
              {
                  current_features = feats[f].data();
                  for(int j=0; j<nhidden; j++)  {
                      for(int k=0; k<feats[f].length(); k++)
                      {                          
                          for(int i=0; i<nout; i++) 
                          {                              
                              if(i!=target[2])  {
                                  if(separate_features) whids[f](j, current_features[k]) -= learning_rate*output[i]*ws[f](i,j)*(1-mypow(activations(j,f),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                                  else whids[f](j, current_features[k]) -= learning_rate*output[i]*ws[0](i,j)*(1-mypow(activations(j,0),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                              }
                              else  {
                                  if(separate_features) whids[f](j, current_features[k]) -= learning_rate*(output[i]-1)*ws[f](i,j)*(1-mypow(activations(j,f),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                                  else whids[f](j, current_features[k]) -= learning_rate*(output[i]-1)*ws[0](i,j)*(1-mypow(activations(j,0),2)) + (lambda != 0 ? 2*lambda*whids[f](j,current_features[k]) : 0);
                              }
                          }
                      }
                  }
              }
                  
          }
          else
          {

              for(int f=0; f<feats.length(); f++)
              {
                  current_features = feats[f].data();
                  for(int i=0; i<nout; i++) 
                  {                                        
                      // Update w
                      for(int j=0; j<feats[f].length(); j++)  {
                          if(i!=target[2])  {
                              ws[f](i, current_features[j]) -= learning_rate*output[i] + (lambda != 0 ? 2*lambda*ws[f](i, current_features[j]) : 0);
                          }
                          else  {
                              ws[f](i, current_features[j]) -= learning_rate*(output[i]-1) + (lambda != 0 ? 2*lambda*ws[f](i, current_features[j]) : 0);
                          }
                      }                                                             

                      if(auxiliary_task_train_set && (separate_features || f==0))
                      {
                          // Update v
                          for(int j=0; j<50; j++)  {
                              if(i!=target[2])  {
                                  vs[f](i, j) -= learning_rate*output[i]*thetas_times_x(j,f);
                              }
                              else  {
                                  vs[f](i, j) -= learning_rate*(output[i]-1)*thetas_times_x(j,f);
                              }
                          }
                      }
                  }
              }
          }
      }
      ++stage;
      train_stats->finalize(); // finalize statistics for this epoch
      
  }
  
}

void StructuralLearner::test(VMat testset, PP<VecStatsCollector> test_stats, 
                    VMat testoutputs, VMat testcosts) const
{
    int l = testset.length();

    ProgressBar* pb = NULL;
    if(report_progress) 
        pb = new ProgressBar("Testing learner",l);

    if (l == 0) {
        // Empty test set: we give -1 cost arbitrarily.
        costs.fill(-1);
        test_stats->update(costs);
    }

    // TODO: VITERBI!!!! This is cheating!!!
    for(int i=0; i<l; i++)
    {
        testset.getExample(i, input, target, weight);
      
        computeFeatures(input,target,-1,i,feats);
        computeOutputWithFeatures(feats,output);
        computeCostsFromOutputs(input,output,target,costs);
        //computeOutputAndCosts(input,target,output,costs);

        // TODO: update dynamic feature
        //updateDynamicFeatures(token_prediction_train,input[3*2],target[2]);

        if(testoutputs)
            testoutputs->putOrAppendRow(i,output);

        if(testcosts)
            testcosts->putOrAppendRow(i, costs);

        if(test_stats)
            test_stats->update(costs,weight);

        if(report_progress)
            pb->update(i);
    }

    /*
      for(int i=0; i<l; i++)
      {
       testset.getExample(i, input, target, weight);
      
        computeFeatures(input,target,-1,i,feats);
        computeOutputWithFeatures(feats,output);
        computeCostsFromOutputs(input,output,target,costs);
        //computeOutputAndCosts(input,target,output,costs);

        // TODO: update dynamic feature
        //updateDynamicFeatures(token_prediction_train,input[3*2],target[2]);

        if(testoutputs)
            testoutputs->putOrAppendRow(i,output);

        if(testcosts)
            testcosts->putOrAppendRow(i, costs);

        if(test_stats)
            test_stats->update(costs,weight);

        if(report_progress)
            pb->update(i);
    }
    */

    if(pb)
        delete pb;
}

void StructuralLearner::computeOutputWithFeatures(TVec<TVec<unsigned int> >& feats, Vec& output, bool use_theta, int begin_class, int end_class) const
{
    if(begin_class < 0) begin_class = 0;
    if(end_class < 0) end_class = output.length();
    /*
    if(only_this_class < 0) 
    {
        output.resize(ws[0].lenght());
        before_softmax.resize(ws[0].lenght());
    }
    else 
    {
        output.resize(1);
        before_softmax.resize(1);
    }
    */
    for(int i=0; i<before_softmax.length(); i++) {
        before_softmax[i] = 0;
    }

    // TODO: computations with Neural Network

    if(nhidden > 0)
    {
        activations.clear();
        activations.lastRow().fill(1.0);
        for(int f=0; f<feats.length(); f++)
        {
            current_features = feats[f].data();
            for(int i=0; i<nhidden; i++) {
                for(int j=0; j<feats[f].length(); j++)  {
                    if(separate_features)
                        activations(i,f) += whids[f](i, current_features[j]);
                    else
                        activations(i,0) += whids[f](i, current_features[j]);
                }
                if(separate_features)
                    activations(i,f) = tanh(activations(i,f));
            }
        }
        if(!separate_features)
            for(int i=0; i<nhidden; i++)
                activations(i,0) = tanh(activations(i,0));

        if(use_theta)
        {
            // compute theta * x
            thetas_times_x.clear();
            for(int f=0; f<(separate_features ? feats.length() : 1); f++)
            {
                for(int j=0; j<50; j++)
                {
                    for(int k=0; k<nhidden+1; k++)
                            thetas_times_x(j,f) += thetas[f](j,k)*activations(k,f);
                }
            }
        }
        
        for(int f=0; f<(separate_features ? feats.length() : 1); f++)
        {
            //if(only_this_class < 0)
            //{
                for(int i=begin_class; i<end_class; i++) {
                    for(int j=0; j<nhidden+1; j++)  {
                        before_softmax[i] += ws[f](i, j) * activations(j,f); 
                    }
                    if(use_theta)
                        for(int ii=0; ii<50; ii++) {
                            before_softmax[i] += vs[f](i, ii)*thetas_times_x(ii,f);
                        }        
                }
                /*
            }
            else
            {
                for(int j=0; j<nhidden; j++)  {
                    before_softmax[0] += ws[f](only_this_class, j) * activations(j,f); 
                }
                if(use_theta)
                    for(int ii=0; ii<50; ii++) {
                        before_softmax[0] += vs[f](only_this_class, ii)*thetas_times_x(ii,f);
                    }       
            }
                */
        }
    }
    else
    {
        if(use_theta)
        {
            fl = 0;
            // compute theta * x
            thetas_times_x.clear();
            for(int f=0; f<feats.length(); f++)
            {
                current_features = feats[f].data();
                for(int j=0; j<50; j++)
                {
                    for(int k=0; k<feats[f].length(); k++)
                        if(separate_features)
                            thetas_times_x(j,f) += thetas[f](j,current_features[k]);
                        else
                            thetas_times_x(j,0) += thetas[0](j,current_features[k]+fl);
                }
                fl += ws[f].width();
            }
        }
        
        for(int f=0; f<feats.length(); f++)
        {
            current_features = feats[f].data();
            //if(only_this_class < 0)
            //{
                for(int i=begin_class; i<end_class; i++) {
                    for(int j=0; j<feats[f].length(); j++)  {
                        before_softmax[i] += ws[f](i, current_features[j]);
                    }
                    if(use_theta && (separate_features || f==0))
                        for(int ii=0; ii<50; ii++) {
                            before_softmax[i] += vs[f](i, ii) * thetas_times_x(ii,f) ;
                        }        
                }
                /*
            }
            else
            {
                for(int j=0; j<feats[f].length(); j++)  {
                    before_softmax[0] += ws[f](only_this_class, current_features[j]);
                }
                if(use_theta && (separate_features || f==0))
                    for(int ii=0; ii<50; ii++) {
                        before_softmax[0] += vs[f](only_this_class, ii) * thetas_times_x(ii,f) ;
                    }
            }
                */
        }

    }

    //if(only_this_class < 0)
    if(begin_class != 0 || end_class != output.length())
        softmax(before_softmax.subVec(begin_class,end_class),output.subVec(begin_class,end_class));
    //else
    //    output[0] = sigmoid(before_softmax[0]);
}


void StructuralLearner::computeOutput(const Vec& input, Vec& output) const
{
    PLERROR("In StructuralLearner::computeOutput(): not implemented");
}    

void StructuralLearner::computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                           const Vec& target, Vec& costs) const
{   
    // Compute the costs from *already* computed output.    
    int argout;
    real output_index_O = output[index_O];
    if(index_O < 0 || output_index_O > abstention_threshold)
        argout = argmax(output);
    else
    {
        output[index_O] = -1;
        argout = argmax(output);
        output[index_O] = output_index_O;
    }
    costs[0] = -safeflog( output[(int)target[2]] );
    costs[1] = argout == target[2] ? 0 : 1; //class_error(output,target);
    if(argout != index_O) costs[2] = costs[1];
    else costs[2] = MISSING_VALUE;
    if(target[2] != index_O) costs[3] = costs[1];
    else costs[3] = MISSING_VALUE;     
}

TVec<string> StructuralLearner::getTestCostNames() const
{
    // Return the names of the costs computed by computeCostsFromOutpus
    // (these may or may not be exactly the same as what's returned by getTrainCostNames).
    TVec<string> ret;
    ret.resize(4);
    ret[0] = "NLL";
    ret[1] = "class_error";
    ret[2] = "precision";
    ret[3] = "recall";
    return ret;
}

TVec<string> StructuralLearner::getTrainCostNames() const
{
    // Return the names of the objective costs that the train method computes and 
    // for which it updates the VecStatsCollector train_stats
    // (these may or may not be exactly the same as what's returned by getTestCostNames).
    TVec<string> ret;
    ret.resize(4);
    ret[0] = "NLL";
    ret[1] = "class_error";
    ret[2] = "precision";
    ret[3] = "recall";
    return ret;
}

/** 
* @brief Takes an example as input and returns the corresponding features. These are onehot encoded
* and so it is the active indices that are returned. The function returns the features' onehot encoded
* length.
* 
* @param input the example's input
* @param target the example's target
* @param data_set the index for the data set (-1 is for test set, 0 for training set and 1 for auxiliary task train set)
* @param index the index of the example for which features are extracted
* @param theFeatureGroups the features (the indices that are active) organized by groups - output
* @param featureMask specifies whether the features are masked - lower 5 bits
* are used to represent the 5-token window. Inactive bit means do not output
* features for that position.
*       '00011111' means return all features
*       '00000100' means return only features for the position we're making the
*       prediction at
*
* @returns the features' onehot encoded length
*
* @note check with Hugo: fl+=(train_set->getDictionary(0))->size()+2; HUGO: I don't think we need a feature for missing values...
* @todo take option into account
**/
void StructuralLearner::computeFeatures(const Vec& input, const Vec& target, int data_set, int index, TVec< TVec<unsigned int> >& theFeatureGroups, char
featureMask) const 
{
    

    fl=0;		// length of the onehot encoded features (stands for "features' length")

    // We have 5 feature groups
    theFeatureGroups.resize(6);
    fls.resize(6);

    // *** Wordtag features ***
    // Wordtags in a 5 word window with a onehot encoding
    // Derived from the wordtags input[0], input[7], input[14], input[21],
    // input[28]
    currentFeatureGroup = theFeatureGroups[0];
    currentFeatureGroup.resize(6);
    size = 0;
    for(int i=0, ii=0; i<5; i++)  {
        ii=7*i;
	
        if( featureMask & (1<<i) ) {        // we are doing this test often, but it should be quick enough. If need be we'll optimize the function
            if( !is_missing(input[ii]) ) {
                currentFeatureGroup[size] = (unsigned int)(fl + input[ii]);
                size++;
            }   
            // I don't think having a feature for missing value will help...
            /*
              else	{
              currentFeatureGroup.push_back( fl + (train_set->getDictionary(ii))->size() + 1 );  // explicitly say it's missing
              }
              fl += (train_set->getDictionary(ii))->size()+2; // +1 for OOV and +1 for missing<
            */
        }

        fl += (train_set->getDictionary(ii))->size()+1;
    }//for wordtags

    // For the bias!!!
    currentFeatureGroup[size] = fl;
    size++; 
    fl++; 
    fls[0] = fl;
    theFeatureGroups[0].resize(size);


    // *** Prefix features ***
    // Prefix features - prefix tag 
    // Derived from input[1], input[8], input[15], input[23], input[31])
    currentFeatureGroup = theFeatureGroups[1];
    currentFeatureGroup.resize(5);
    size = 0;   
    fl=0;
    for(int i=0, ii=0; i<5; i++)  {        
        ii=7*i+1;
        
        if( featureMask & (1<<i) ) {        // we are doing this test often, but it should be quick enough. If need be we'll optimize the function
            // Prefix tag is not missing, look at it
            if( !is_missing(input[ii]) ) {
                currentFeatureGroup[size] = (unsigned int)(fl + input[ii]);
                size++;
            }
        }    
        fl += (train_set->getDictionary(ii))->size()+1;
    }//for 5 word window
    theFeatureGroups[1].resize(size);
    fls[1] = fl;


    // *** Suffix features ***
    // Suffix features - suffix tags
    // Derived from input[2], input[9], ... 
    currentFeatureGroup = theFeatureGroups[2];
    currentFeatureGroup.resize(5);
    size = 0;   
    fl=0;
    for(int i=0, ii=0; i<5; i++)  
    {
        ii=7*i+2;
	     	
        if( featureMask & (1<<i) ) {        // we are doing this test often, but it should be quick enough. If need be we'll optimize the function
            // Suffix tag is not missing, look at it
            if( !is_missing(input[ii]) ) {
                currentFeatureGroup[size] = (unsigned int)(fl + input[ii]);
                size++;
            }
        }    
        fl += (train_set->getDictionary(ii))->size()+1;
    }//for 5 word window
    theFeatureGroups[2].resize(size);
    fls[2] = fl;


    // *** Char type features ***
    // Char type features in a 5 word window - 4 features (1 if true, 0 if not):
    //		-1st letter capitalized
    //		-All letters capitalized
    //		-All digits
    //		-All digits and '.'  ','
    // Explicit from input[3], input[4], input[5], input[6], input[10], ...

    currentFeatureGroup = theFeatureGroups[3];
    currentFeatureGroup.resize(20);
    size = 0;
    fl = 0;
    for(int i=0, ii=0; i<5; i++)  {
        ii=7*i+3;
	
        if( featureMask & (1<<i) ) {        // we are doing this test often, but it should be quick enough. If need be we'll optimize the function
            // for 4 features
            for(int j=0; j<4; j++)  {
                // feature not missing
                if( !is_missing(input[ii]) ) {
                    // feature active
                    if(input[ii]==1)    {
                        currentFeatureGroup[size] = (unsigned int)(fl);
                        size++;
                    }
                }      
                fl++;
                ii++;
            }
        }   else    {
            fl = fl+4;
        }
    }//for 5 word window
    theFeatureGroups[3].resize(size);
    fls[3] = fl;

    // *** "Bag of words in a 3 syntactic chunk window" features ***
    // we have this from preprocessing
/*
    currentFeatureGroup = theFeatureGroups[5];
    currentFeatureGroup.resize(0);
    size = 0;   
    fl=0;
    // TODO: fetch correct wordsIn3SyntacticContext Vec, depending
    //       on the values of data_set and index
    //for(int i=0; i<wordsIn3SyntacticContext.length(); i++)	{
    //currentFeatureGroup.push_back(wordsIn3SyntacticContext[i]);
    //}
    theFeatureGroups[5].resize(size);
    fls[5] = fl;
*/

    // *** Label features ***
    // Labels of the 2 words on the left - should always be in the target (if we are decoding, then the target
    // should hold what we have predicted
    currentFeatureGroup = theFeatureGroups[4];
    currentFeatureGroup.resize(2);
    size = 0;   
    fl = 0;
    // Hugo: we don't use the tag features for auxiliary task???
    if( featureMask & 1 ) {       
        if( !is_missing(target[0]) ) {
            currentFeatureGroup.push_back( fl+(int)target[0] );
            size++;
        }
    }
    fl += (train_set->getDictionary(inputsize_))->size()+1;
        
    // Hugo: idem
    if( featureMask & 2) {       
        if( !is_missing(target[1]) ) {
            currentFeatureGroup.push_back( fl + (int)target[1] );
            size++;
        }
    }
    fl += (train_set->getDictionary(inputsize_))->size()+1;
    theFeatureGroups[4].resize(size);
    fls[4] = fl;

    // *** Bigrams of current token and label on the left
    currentFeatureGroup = theFeatureGroups[5];
    currentFeatureGroup.resize(1);
    fl = 0;
    size=0;
  
    // Hugo: idem!!!
    // if none of the 2 are masked than we'll compute the feature
    if( (featureMask & 2) && (featureMask & 4) ) {      
        if( !is_missing(target[1]) && !is_missing(input[14]) ) {
          int bigram = (int)target[1] * ((train_set->getDictionary(0))->size()+1) + (int)input[14];
          std::map<int, int>::iterator itr_plcw_bigram_mapping;

          // is it in our mapping of bigrams seen in train_set?
          itr_plcw_bigram_mapping = plcw_bigram_mapping.find( bigram );

          if( itr_plcw_bigram_mapping != plcw_bigram_mapping.end() )  {
            currentFeatureGroup.push_back( itr_plcw_bigram_mapping->second );
            size++;
          }
        }
    } 
    fl += plcw_bigram_mapping.size();
    theFeatureGroups[5].resize(size);
    fls[5] = fl;


    // *** Previous occurences features ***
  /*  // ...
        
    fl = 0;
    size=0;
    // Add things here...
    theFeatureGroups[8].resize(size);
    fls[8] = fl;
  */  
}

/** 
* @brief Updates features computed in computeFeatures(). 
* Actually just recomputes the feature groups based upon the target vector, ie the previous labels and "previous label - current word" bigram.
*
* @param input the example's input
* @param theFeatureGroups the features (the indices that are active) organized by groups - output
* @param featureMask specifies whether the features are masked - lower 5 bits
* are used to represent the 5-token window. Inactive bit means do not output
* features for that position.
*       '00011111' means return all features
*       '00000100' means return only features for the position we're making the
*       prediction at
*
* @returns 
*
* @note 
* @todo 
**/
void StructuralLearner::updateFeatures(const Vec& input, const Vec& target,  TVec< TVec<unsigned int> >& theFeatureGroups, char
featureMask)  
{

    // *** Label features ***
    // Labels of the 2 words on the left - should always be in the target (if we are decoding, then the target
    // should hold what we have predicted
    currentFeatureGroup = theFeatureGroups[4];
    currentFeatureGroup.resize(2);
    size = 0;
    fl = 0;

    // Hugo: we don't use the tag features for auxiliary task???
    if( featureMask & 1 ) {       
        if( !is_missing(target[0]) ) {
            currentFeatureGroup.push_back( fl+(int)target[0] );
            size++;
        }
    }
    fl += (train_set->getDictionary(inputsize_))->size()+1;
        
    // Hugo: idem
    if( featureMask & 2) {       
        if( !is_missing(target[1]) ) {
            currentFeatureGroup.push_back( fl + (int)target[1] );
            size++;
        }
    }
    fl += (train_set->getDictionary(inputsize_))->size()+1;
    theFeatureGroups[4].resize(size);
    fls[4] = fl;

    // *** Bigrams of current token and label on the left
    currentFeatureGroup = theFeatureGroups[5];
    currentFeatureGroup.resize(1);
    fl = 0;
    size=0;
  
    // Hugo: idem!!!
    // if none of the 2 are masked than we'll compute the feature
    if( (featureMask & 2) && (featureMask & 4) ) {      
        if( !is_missing(target[1]) && !is_missing(input[14]) ) {

          int bigram = (int)target[1] * ((train_set->getDictionary(0))->size()+1) + (int)input[14];
          std::map<int, int>::iterator itr_plcw_bigram_mapping;

          // is it in our mapping of bigrams seen in train_set?
          itr_plcw_bigram_mapping = plcw_bigram_mapping.find( bigram );

          if( itr_plcw_bigram_mapping != plcw_bigram_mapping.end() )  {
            currentFeatureGroup.push_back( itr_plcw_bigram_mapping->second );
            size++;
          }
        }
    } 
    fl += plcw_bigram_mapping.size();
    theFeatureGroups[5].resize(size);
    fls[5] = fl;


}

/** 
* @brief Determines 1000 most frequent words and builds 2 TVecs of indices of examples that have respetively
* a frequent word at current and left positions.
* 
* @param 
* @returns 
*
* @note 
* @todo 
**/
void StructuralLearner::initWordProblemsStructures()
{

  // *** Determine most frequent words
  // Just a big fequency array.

  // 1) Create and init the freq table - has for size the size of the vocabulary +1 for OOV
  unsigned long int* frequency;
  frequency = new unsigned long int[ (auxiliary_task_train_set->getDictionary(0))->size() + 1];
  //memset(frequency, 0, ((train_set->getDictionary(6))->size()+1) * sizeof(unsigned long int) ); 
  for(int i=0; i<((auxiliary_task_train_set->getDictionary(0))->size()+1); i++)  {  
    frequency[i]=0;
  }

  // 2) Compute frequencies
  for(int e=0; e<auxiliary_task_train_set->length(); e++)  {
    auxiliary_task_train_set->getExample(e, input, target, weight);
    frequency[(int)input[14]]++;
  }

  // 3) extract most frequent entries -> build a map
  // build a stl vector (skip OOV output) and sort it
  std::vector<freqCount> tmp;
  for(int i=1; i<((auxiliary_task_train_set->getDictionary(0))->size()+1); i++)  {  
    tmp.push_back( freqCount(i, frequency[i]) );
  }

  delete []frequency;

  // Sort the items in descending order
  std::sort(tmp.begin(), tmp.end(), freqCountGT);

  // Build a map of the most frequent words' wordtags with their "most frequent word's"-tag
  std::map<int, int> map_mostFrequentWords;  // word tag is key, value is the net's output for it
  std::vector<freqCount>::iterator itr;
  int i;
  for(i=0, itr=tmp.begin(); itr!=tmp.end() && i<=n_auxiliary_wordproblems; itr++, i++) {
    map_mostFrequentWords[itr->wordtag] = i; 
    //MostFrequentWordsCount+=itr->count;
  }

  tmp.clear();


  // *** Build the TMats for the auxiliary problems
  std::map<int, int>::iterator itr_map_mostFrequentWords;
  int leftWord_Wordtag, currentWord_Wordtag;
  int left_size=0;
  int current_size=0;

  auxiliary_indices_left.resize(auxiliary_task_train_set->length(), 2);
  auxiliary_indices_current.resize(auxiliary_task_train_set->length(), 2);

  for(int e=0; e<auxiliary_task_train_set->length(); e++)  {
    auxiliary_task_train_set->getExample(e, input, target, weight);

    // * if this example has a most frequent word at left
    leftWord_Wordtag = (int)input[7];

    itr_map_mostFrequentWords = map_mostFrequentWords.find( leftWord_Wordtag );

    if( itr_map_mostFrequentWords != map_mostFrequentWords.end() )  {
        auxiliary_indices_left[left_size][0] = e;
        auxiliary_indices_left[left_size][1] = itr_map_mostFrequentWords->second;
        left_size++;
    }

    // * if this example has a most frequent word at current
    currentWord_Wordtag = (int)input[14];

    itr_map_mostFrequentWords = map_mostFrequentWords.find( currentWord_Wordtag );

    if( itr_map_mostFrequentWords != map_mostFrequentWords.end() )  {
        auxiliary_indices_current[current_size][0] = e;
        auxiliary_indices_current[current_size][1] = itr_map_mostFrequentWords->second;
        current_size++;
    }

  }// end for auxiliary example

  map_mostFrequentWords.clear();

  auxiliary_indices_left.resize(left_size, 2);
  auxiliary_indices_current.resize(current_size, 2);

}

/** 
* @brief Determines which "previous label - current word" bigrams are in the
* training set and indexes them is a stl map.
* OOV's are ignored.
* @param 
* @returns 
*
* @note 
* @todo 
**/
void StructuralLearner::initPreviousLabelCurrentWordBigramMapping()
{
    int bigram;
    int currentBigramIndex=0;

    std::map<int, int>::iterator itr_plcw_bigram_mapping;

    // Attribute an index to "previous label - current word" bigrams seen in train_set
    for(int e=0; e<train_set->length(); e++)  {
        train_set->getExample(e, input, target, weight);

        if( !is_missing(target[1]) && !is_missing(input[14]) ) {
          // if no OOV
          if( (!target[1]) && (!input[14]) )  {
            // The bigram 
            bigram = (int)target[1] * ((train_set->getDictionary(0))->size()+1) + (int)input[14];

            // if not already there, add it
            itr_plcw_bigram_mapping = plcw_bigram_mapping.find( bigram );

            if( itr_plcw_bigram_mapping == plcw_bigram_mapping.end() )  {
                plcw_bigram_mapping[bigram] = currentBigramIndex;
                currentBigramIndex++;
            }
          }
        }
    }// end for auxiliary example

}



/*
//PA - need to integrate this
int StructuralLearner::determineWordsIn3SyntacticContext(VMat example_set, TVec< TVec<unsigned int> >& wordsIn3SyntacticContext_set)	{
	
	TVec< unsigned int > leftSyntacticChunkBagOfWords;
	TVec< unsigned int > CurrentSyntacticChunkBagOfWords;
	TVec< unsigned int > RightSyntacticChunkBagOfWords;
	
	TVec< unsigned int > wordsIn3SyntacticContext;

	input[8] is current chunk
	
	// set currentSyntacticChunk
	//compute CurrentSyntacticChunkBagOfWords and RightSyntacticChunkBagOfWords
	// then cat into wordsIn3SyntacticContext
	
	for(int e=0; e<train_set->length(); e++)  {
		train_set->getExample(e, input, target, weight);
	
		// We encounter a new chunk
		if( input[8] != currentSyntacticChunk )	{		// input[8] is the current syntactic chunk - never a missing value
			leftSyntacticChunkBagOfWords = CurrentSyntacticChunkBagOfWords;
			CurrentSyntacticChunkBagOfWords = RightSyntacticChunkBagOfWords;
			// set currentSyntacticChunk
			// compute new RightSyntacticChunkBagOfWords
			// readjust wordsIn3SyntacticContext by cating all 3 (insure unicity? YES!)
		}
		
		wordsIn3SyntacticContext_set.push_back(wordsIn3SyntacticContext);
		
	}//for the examples
		
		
	return 0;
}

*/



//////////////////////
// initializeParams //
//////////////////////
void StructuralLearner::initializeParams(bool set_seed)
{
    if (set_seed) {
        if (seed_>=0)
            manual_seed(seed_);
        else
            PLearn::seed();
    }

  // initialize weights
  if (train_set) {
    real delta;
    int is;

    for(int i=0; i<ws.length(); i++) {
        is = ws[i].size();
        delta = 1.0 / sqrt(real(is));
        fill_random_uniform(ws[i], -delta, delta);
    }

    for(int i=0; i<vs.length(); i++) {
        is = vs[i].size();
        delta = 1.0 / sqrt(real(is));
        fill_random_uniform(vs[i], -delta, delta);
    }
    
    
    if(nhidden > 0)
        for(int i=0; i<whids.length(); i++) 
        {
            is = whids[i].size();
            delta = 1.0 / sqrt(real(is));
            fill_random_uniform(whids[i], -delta, delta);
        }
    

  }
}

} // end of namespace PLearn


/*
  Local Variables:
  mode:c++
  c-basic-offset:4
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0))
  indent-tabs-mode:nil
  fill-column:79
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=79 :
