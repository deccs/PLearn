// -*- C++ -*-

// NatGradNNet.h
//
// Copyright (C) 2007 Yoshua Bengio
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

// Authors: Yoshua Bengio

/*! \file NatGradNNet.h */


#ifndef NatGradNNet_INC
#define NatGradNNet_INC

#include <plearn_learners/generic/PLearner.h>
#include <plearn_learners/generic/GradientCorrector.h>
#include <plearn/sys/Profiler.h>
//#include "CorrelationProfiler.h" // *stat*

namespace PLearn {

/**
 * Multi-layer neural network trained with an efficient Natural Gradient optimization.
 */
class NatGradNNet : public PLearner
{
    typedef PLearner inherited;

public:
    //#####  Public Build Options  ############################################

    int noutputs;

    //! sizes of hidden layers, provided by the user.
    TVec<int> hidden_layer_sizes;

    //! layer_params[i] is a matrix of dimension layer_sizes[i+1] x (layer_sizes[i]+1)
    //! containing the neuron biases in its first column.
    TVec<Mat> layer_params;
    //! mean layer_params, averaged over past updates (moving average)
    TVec<Mat> layer_mparams;

    //! mparams <-- (1-params_averaging_coeff)mparams + params_averaging_coeff*params
    real params_averaging_coeff;
    //! how often (in terms of minibatches, i.e. weight updates) do we perform the above?
    int params_averaging_freq;

    //! initial learning rate
    real init_lrate;

    //! learning rate decay factor
    real lrate_decay;

    //! L1 penalty applied to the output layer's parameters
    real output_layer_L1_penalty_factor;

    //! scaling factor of the learning rate for the output layer
    real output_layer_lrate_scale;

    //! update the parameters only so often
    int minibatch_size;

    //! natural gradient estimator for neurons
    //! (if 0 then do not correct the gradient on neurons)
    PP<GradientCorrector> neurons_natgrad_template;
    TVec<PP<GradientCorrector> > neurons_natgrad_per_layer;

    //! natural gradient estimator for the parameters within each neuron
    //! (if 0 then do not correct the gradient on each neuron weight)
    PP<GradientCorrector> params_natgrad_template;
    //! natural gradient estimator solely for the parameters of the first
    //! layer. If present, performs over groups of parameters related to the
    //! same input (this includes the additional bias input).
    //! Has precedence over params_natgrad_template, ie if present, there is
    //! no natural gradient performed on the groups of a neuron's parameters:
    //! params_natgrad_template is not applied for the first hidden layer's
    //! parameters). 
    PP<GradientCorrector> params_natgrad_per_input_template;

    //! the above templates are used by the user to specifiy all the elements of the vector below
    TVec<PP<GradientCorrector> > params_natgrad_per_group;

    //! optionally, if neurons_natgrad==0 and params_natgrad_template==0, one can
    //! have regular stochastic gradient descent, or full-covariance natural gradient
    //! using the natural gradient estimator below
    PP<GradientCorrector> full_natgrad;

    //! type of output cost: "NLL" for classification problems, "MSE" for regression
    string output_type;

    //! 0 does not scale the learning rate
    //! 1 scales it by 1 / the nb of inputs of the neuron
    //! 2 scales it by 1 / sqrt(the nb of inputs of the neuron)
    //! etc.
    real input_size_lrate_normalization_power;

    //! scale the learning rate in different neurons by a factor
    //! taken randomly as follows: choose integer n uniformly between 
    //! lrate_scale_factor_min_power and lrate_scale_factor_max_power
    //! inclusively, and then scale learning rate by lrate_scale_factor^n.
    real lrate_scale_factor;
    int lrate_scale_factor_max_power;
    int lrate_scale_factor_min_power;

    //! Let each neuron self-adjust its bias and scaling factor of its activations
    //! so that the mean and standard deviation of the activations reach 
    //! the target_mean_activation and target_stdev_activation.
    bool self_adjusted_scaling_and_bias;
    real target_mean_activation;
    real target_stdev_activation;
    // the mean and variance of the activations is estimated by a moving
    // average with this coefficient (near 0 for very slow averaging)
    real activation_statistics_moving_average_coefficient;

    // *stat*
    // Temporary stuff for getting a clue as to what's going on
    // Look for the marker '*stat*' in the code

    // -Options-
    //! Stages for profiling the correlation between the gradients' elements
    //int corr_profiling_start, corr_profiling_end;

    // -Not options-
    //PP<CorrelationProfiler> g_corrprof, ng_corrprof;    // for optional gradient correlation profiling
    //real sum_gradient_norms;     // holds sum of the gradient norms - reset at each epoch
    //Vec all_params_cum_gradient; // holds the sum of the gradients - reset at each epoch

    //! To see differences between class gradient covariances
    //TVec<VecStatsCollector> pa_gradstats;   // one VecStatsCollector per output class

    //! Holds the number of samples gathered for each weight
    //TVec<int> pv_all_nsamples;
    //TVec< TMat<int> > pv_layer_nsamples;

    // *stat* end

public:
    //#####  Public Member Functions  #########################################

    NatGradNNet();

    //#####  PLearner Member Functions  #######################################

    //! Returns the size of this learner's output, (which typically
    //! may depend on its inputsize(), targetsize() and set options).
    // (PLEASE IMPLEMENT IN .cc)
    virtual int outputsize() const;

    //! (Re-)initializes the PLearner in its fresh state (that state may depend
    //! on the 'seed' option) and sets 'stage' back to 0 (this is the stage of
    //! a fresh learner!).
    // (PLEASE IMPLEMENT IN .cc)
    virtual void forget();

    //! The role of the train method is to bring the learner up to
    //! stage==nstages, updating the train_stats collector with training costs
    //! measured on-line in the process.
    // (PLEASE IMPLEMENT IN .cc)
    virtual void train();

    //! Computes the output from the input.
    // (PLEASE IMPLEMENT IN .cc)
    virtual void computeOutput(const Vec& input, Vec& output) const;
    virtual void computeOutputs(const Mat& input, Mat& output) const;

    virtual void computeOutputsAndCosts(const Mat& input, const Mat& target, 
                                        Mat& output, Mat& costs) const;


    //! Computes the costs from already computed output.
    // (PLEASE IMPLEMENT IN .cc)
    virtual void computeCostsFromOutputs(const Vec& input, const Vec& output,
                                         const Vec& target, Vec& costs) const;

    //! Returns the names of the costs computed by computeCostsFromOutpus (and
    //! thus the test method).
    // (PLEASE IMPLEMENT IN .cc)
    virtual TVec<std::string> getTestCostNames() const;

    //! Returns the names of the objective costs that the train method computes
    //! and  for which it updates the VecStatsCollector train_stats.
    // (PLEASE IMPLEMENT IN .cc)
    virtual TVec<std::string> getTrainCostNames() const;


    // *** SUBCLASS WRITING: ***
    // While in general not necessary, in case of particular needs
    // (efficiency concerns for ex) you may also want to overload
    // some of the following methods:
    // virtual void computeOutputAndCosts(const Vec& input, const Vec& target,
    //                                    Vec& output, Vec& costs) const;
    // virtual void computeCostsOnly(const Vec& input, const Vec& target,
    //                               Vec& costs) const;
    // virtual void test(VMat testset, PP<VecStatsCollector> test_stats,
    //                   VMat testoutputs=0, VMat testcosts=0) const;
    // virtual int nTestCosts() const;
    // virtual int nTrainCosts() const;
    // virtual void resetInternalState();
    // virtual bool isStatefulLearner() const;


    //#####  PLearn::Object Protocol  #########################################

    // Declares other standard object methods.
    // ### If your class is not instantiatable (it has pure virtual methods)
    // ### you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT_METHODS
    PLEARN_DECLARE_OBJECT(NatGradNNet);

    // Simply calls inherited::build() then build_()
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    // (PLEASE IMPLEMENT IN .cc)
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

protected:
    //#####  Protected Options  ###############################################

    // ### Declare protected option fields (such as learned parameters) here

    //! number of layers of weights (2 for a neural net with one hidden layer)
    int n_layers;

    //! layer sizes (derived from hidden_layer_sizes, inputsize_ and outputsize_)
    TVec<int> layer_sizes;

    //! pointers into the layer_params
    TVec<Mat> biases;
    TVec<Mat> weights,mweights;
    TVec<Vec> activations_scaling; // output = tanh(activations_scaling[layer][neuron] * (biases[layer][neuron] + weights[layer]*input[layer-1])
    TVec<Vec> mean_activations;
    TVec<Vec> var_activations;
    real cumulative_training_time;

protected:
    //#####  Protected Member Functions  ######################################

    //! Declares the class options.
    // (PLEASE IMPLEMENT IN .cc)
    static void declareOptions(OptionList& ol);

    //! one minibatch training step
    void onlineStep(int t, const Mat& targets, Mat& train_costs, Vec example_weights);

    //! compute a minibatch of size n_examples network top-layer output given layer 0 output (= network input)
    //! (note that log-probabilities are computed for classification tasks, output_type=NLL)
    void fpropNet(int n_examples, bool during_training) const;

    //! compute train costs given the network top-layer output
    //! and write into neuron_gradients_per_layer[n_layers-2], gradient on pre-non-linearity activation
    void fbpropLoss(const Mat& output, const Mat& target, const Vec& example_weights, Mat& train_costs) const;


private:
    //#####  Private Member Functions  ########################################

    //! This does the actual building.
    // (PLEASE IMPLEMENT IN .cc)
    void build_();

private:
    //#####  Private Data Members  ############################################

    // The rest of the private stuff goes here

    Vec all_params; // all the parameters in one vector
    Vec all_params_delta; // update direction
    Vec all_params_gradient; // all the parameter gradients in one vector
    Vec all_mparams; // mean parameters (moving-averaged over past values)
    TVec<Mat> layer_params_gradient;
    TVec<Vec> layer_params_delta;
    TVec<Vec> group_params; // params of each group (pointing in all_params)
    TVec<Vec> group_params_delta; // params_delta of each group (pointing in all_params_delta)
    TVec<Vec> group_params_gradient; // params_delta of each group (pointing in all_params_gradient)
    Mat neuron_gradients; // one row per example of a minibatch, has concatenation of layer 0, layer 1, ... gradients.
    TVec<Mat> neuron_gradients_per_layer; // pointing into neuron_gradients (one row per example of a minibatch)
    mutable TVec<Mat> neuron_outputs_per_layer;  // same structure
    mutable TVec<Mat> neuron_extended_outputs_per_layer;  // with 1's in the first pseudo-neuron, for doing biases
    Mat targets; // one target row per example in a minibatch
    Vec example_weights; // one element per example in a minibatch
    Mat train_costs; // one row per example in a minibatch
    
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(NatGradNNet);

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
