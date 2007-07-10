// -*- C++ -*-

// StackedAutoassociatorsNet.h
//
// Copyright (C) 2007 Hugo Larochelle
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

// Authors: Pascal Lamblin

/*! \file StackedAutoassociatorsNet.h */


#ifndef StackedAutoassociatorsNet_INC
#define StackedAutoassociatorsNet_INC

#include <plearn_learners/generic/PLearner.h>
#include <plearn_learners/online/OnlineLearningModule.h>
#include <plearn_learners/online/RBMLayer.h>
#include <plearn_learners/online/RBMConnection.h>
#include <plearn_learners/online/RBMMatrixConnection.h>
#include <plearn/misc/PTimer.h>

namespace PLearn {

/**
 * Neural net, initialized with SVDs of logistic auto-regressions.
 */
class StackedAutoassociatorsNet : public PLearner
{
    typedef PLearner inherited;

public:
    //#####  Public Build Options  ############################################

    //! The learning rate used during the logistic auto-regression 
    //! gradient descent training
    real greedy_learning_rate;

    //! The decrease constant of the learning rate used during the 
    //! logistic auto-regression gradient descent training. 
    real greedy_decrease_ct;

    //! The learning rate used during the fine tuning gradient descent
    real fine_tuning_learning_rate;

    //! The decrease constant of the learning rate used during fine tuning
    //! gradient descent
    real fine_tuning_decrease_ct;

    //! Size of mini-batch for gradient descent
    int batch_size;
    
    //! Minimum relative improvement convergence criteria
    //! for the logistic auto-regression.
    real minimum_relative_improvement;
    
    //! The layers of units in the network
    TVec< PP<RBMLayer> > layers;

    //! Module that takes as input the output of the last layer
    //! (layers[n_layers-1), and feeds its output to final_cost
    //! which defines the fine-tuning criteria.
    PP<OnlineLearningModule> final_module;

    //! The cost function to be applied on top of the neural network
    //! (i.e. at the output of final_module). Its gradients will be 
    //! backpropagated to final_module and then backpropagated to
    //! the layers.
    PP<CostModule> final_cost;

    //#####  Public Learnt Options  ###########################################

    //! The weights of the connections between the layers
    TVec< PP<RBMMatrixConnection> > connections;

    //! Number of layers
    int n_layers;

public:
    //#####  Public Member Functions  #########################################

    //! Default constructor
    StackedAutoassociatorsNet();


    //#####  PLearner Member Functions  #######################################

    //! Returns the size of this learner's output, (which typically
    //! may depend on its inputsize(), targetsize() and set options).
    virtual int outputsize() const;

    //! (Re-)initializes the PLearner in its fresh state (that state may depend
    //! on the 'seed' option) and sets 'stage' back to 0 (this is the stage of
    //! a fresh learner!).
    virtual void forget();

    //! The role of the train method is to bring the learner up to
    //! stage==nstages, updating the train_stats collector with training costs
    //! measured on-line in the process.
    virtual void train();

    //! Computes the output from the input.
    virtual void computeOutput(const Vec& input, Vec& output) const;

    //! Computes the costs from already computed output.
    virtual void computeCostsFromOutputs(const Vec& input, const Vec& output,
                                         const Vec& target, Vec& costs) const;

    //! Returns the names of the costs computed by computeCostsFromOutpus (and
    //! thus the test method).
    virtual TVec<std::string> getTestCostNames() const;

    //! Returns the names of the objective costs that the train method computes
    //! and  for which it updates the VecStatsCollector train_stats.
    virtual TVec<std::string> getTrainCostNames() const;


    void greedyStep( const Vec& input, const Vec& target, int index, 
                     Vec train_costs );

    void fineTuningStep( const Vec& input, const Vec& target,
                         Vec& train_costs );

    //#####  PLearn::Object Protocol  #########################################

    // Declares other standard object methods.
    // ### If your class is not instantiatable (it has pure virtual methods)
    // ### you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT_METHODS
    PLEARN_DECLARE_OBJECT(StackedAutoassociatorsNet);

    // Simply calls inherited::build() then build_()
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    // (PLEASE IMPLEMENT IN .cc)
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

protected:
    //#####  Not Options  #####################################################

    //! Stores the activations of the input and hidden layers
    //! (at the input of the layers)
    mutable TVec<Mat> activations;

    //! Stores the expectations of the input and hidden layers
    //! (at the output of the layers)
    mutable TVec<Mat> expectations;

    //! Stores the gradient of the cost wrt the activations of 
    //! the input and hidden layers
    //! (at the input of the layers)
    mutable TVec<Mat> activation_gradients;

    //! Stores the gradient of the cost wrt the expectations of 
    //! the input and hidden layers
    //! (at the output of the layers)
    mutable TVec<Mat> expectation_gradients;

    //! Reconstruction activations
    mutable Mat reconstruction_activations;
    
    //! Reconstruction expectations
    mutable Mat reconstruction_expectations;
    
    //! Reconstruction activations
    mutable Mat reconstruction_activation_gradients;
    
    //! Reconstruction expectations
    mutable Mat reconstruction_expectation_gradients;

    //! Input of the final_cost
    mutable Vec final_cost_input;

    //! Cost value of final_cost
    mutable Vec final_cost_value;

    //! Stores the gradient of the cost at the input of final_cost
    mutable Vec final_cost_gradient;

    //! Currently trained layer (1 means the first hidden layer,
    //! n_layers means the output layer)
    int currently_trained_layer;

    //! Indication whether final_module has learning rate
    bool final_module_has_learning_rate;
    
    //! Indication whether final_cost has learning rate
    bool final_cost_has_learning_rate;
    
protected:
    //#####  Protected Member Functions  ######################################

    //! Declares the class options.
    static void declareOptions(OptionList& ol);

private:
    //#####  Private Member Functions  ########################################

    //! This does the actual building.
    void build_();

    void build_layers_and_connections();

    void build_classification_cost();

    void build_costs();

    void setLearningRate( real the_learning_rate );

private:
    //#####  Private Data Members  ############################################

    // The rest of the private stuff goes here    
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(StackedAutoassociatorsNet);

} // end of namespace PLearn

#endif


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