// -*- C++ -*-

// RBMModule.h
//
// Copyright (C) 2007 Olivier Delalleau
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

// Authors: Olivier Delalleau

/*! \file RBMModule.h */


#ifndef RBMModule_INC
#define RBMModule_INC

#include <map>
#include <plearn_learners/online/OnlineLearningModule.h>
#include <plearn_learners/online/RBMConnection.h>
#include <plearn_learners/online/RBMLayer.h>

namespace PLearn {

/**
 * The first sentence should be a BRIEF DESCRIPTION of what the class does.
 * Place the rest of the class programmer documentation here.  Doxygen supports
 * Javadoc-style comments.  See http://www.doxygen.org/manual.html
 *
 * @todo Write class to-do's here if there are any.
 *
 * @deprecated Write deprecated stuff here if there is any.  Indicate what else
 * should be used instead.
 */
class RBMModule : public OnlineLearningModule
{
    typedef OnlineLearningModule inherited;

public:
    //#####  Public Build Options  ############################################

    PP<RBMLayer> hidden_layer;
    PP<RBMLayer> visible_layer;
    PP<RBMConnection> connection;
    PP<RBMConnection> reconstruction_connection;

    real cd_learning_rate;
    real grad_learning_rate;

    bool compute_contrastive_divergence;

    //! Number of Gibbs sampling steps in negative phase 
    //! of contrastive divergence.
    int n_Gibbs_steps_CD;

    //! used to generate samples from the RBM
    int min_n_Gibbs_steps; 
    int n_Gibbs_steps_per_generated_sample;

    //#####  Public Learnt Options  ############################################
    //! used to generate samples from the RBM
    int Gibbs_step;

public:
    //#####  Public Member Functions  #########################################

    //! Default constructor
    RBMModule();

    // Your other public member functions go here

    //! given the input, compute the output (possibly resize it appropriately)
    virtual void fprop(const Vec& input, Vec& output) const;

    /* Optional
       THE DEFAULT IMPLEMENTATION IN SUPER-CLASS JUST RAISES A PLERROR.
    //! Adapt based on the output gradient, and obtain the input gradient.
    //! The flag indicates wether the input_gradient is accumulated or set.
    //! This method should only be called just after a corresponding
    //! fprop; it should be called with the same arguments as fprop
    //! for the first two arguments (and output should not have been
    //! modified since then).
    //! Since sub-classes are supposed to learn ONLINE, the object
    //! is 'ready-to-be-used' just after any bpropUpdate.
    virtual void bpropUpdate(const Vec& input, const Vec& output,
                             Vec& input_gradient,
                             const Vec& output_gradient,
                             bool accumulate=false);
    */

    /* Optional
       A DEFAULT IMPLEMENTATION IS PROVIDED IN THE SUPER-CLASS, WHICH
       JUST CALLS
            bpropUpdate(input, output, input_gradient, output_gradient)
       AND IGNORES INPUT GRADIENT.
    //! This version does not obtain the input gradient.
    virtual void bpropUpdate(const Vec& input, const Vec& output,
                             const Vec& output_gradient);
    */

    /* Optional
       N.B. A DEFAULT IMPLEMENTATION IS PROVIDED IN THE SUPER-CLASS, WHICH
       RAISES A PLERROR.
    //! Similar to bpropUpdate, but adapt based also on the estimation
    //! of the diagonal of the Hessian matrix, and propagates this
    //! back. If these methods are defined, you can use them INSTEAD of
    //! bpropUpdate(...)
    virtual void bbpropUpdate(const Vec& input, const Vec& output,
                              Vec& input_gradient,
                              const Vec& output_gradient,
                              Vec& input_diag_hessian,
                              const Vec& output_diag_hessian,
                              bool accumulate=false);
    */

    /* Optional
       N.B. A DEFAULT IMPLEMENTATION IS PROVIDED IN THE SUPER-CLASS,
       WHICH JUST CALLS
            bbpropUpdate(input, output, input_gradient, output_gradient,
                         out_hess, in_hess)
       AND IGNORES INPUT HESSIAN AND INPUT GRADIENT.
    //! This version does not obtain the input gradient and diag_hessian.
    virtual void bbpropUpdate(const Vec& input, const Vec& output,
                              const Vec& output_gradient,
                              const Vec& output_diag_hessian);
    */


    //! Reset the parameters to the state they would be BEFORE starting
    //! training.  Note that this method is necessarily called from
    //! build().
    virtual void forget();


    /* Optional
       THE DEFAULT IMPLEMENTATION PROVIDED IN THE SUPER-CLASS DOES NOT
       DO ANYTHING.
    //! Perform some processing after training, or after a series of
    //! fprop/bpropUpdate calls to prepare the model for truly out-of-sample
    //! operation.
    virtual void finalize();
    */

    /* Optional
       THE DEFAULT IMPLEMENTATION PROVIDED IN THE SUPER-CLASS RETURNS false
    //! In case bpropUpdate does not do anything, make it known
    virtual bool bpropDoesNothing();
    */

    //! Throws an error (please use explicitely the two different kinds of
    //! learning rates available here).
    virtual void setLearningRate(real dynamic_learning_rate);

    //! Overridden.
    virtual void fprop(const TVec<Mat*>& ports_value);

    //! Overridden.
    virtual void bpropAccUpdate(const TVec<Mat*>& ports_value,
                                const TVec<Mat*>& ports_gradient);

    //! Returns all ports in a RBMModule.
    virtual const TVec<string>& getPorts();

    //! The ports' sizes are given by the corresponding RBM layers.
    virtual const TMat<int>& getPortSizes();

    //#####  PLearn::Object Protocol  #########################################

    // Declares other standard object methods.
    // ### If your class is not instantiatable (it has pure virtual methods)
    // ### you should replace this by PLEARN_DECLARE_ABSTRACT_OBJECT
    PLEARN_DECLARE_OBJECT(RBMModule);

    // Simply calls inherited::build() then build_()
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);


protected:

    //! Used to store gradient w.r.t. expectations of the hidden layer.
    Mat hidden_exp_grad;

    //! Used to store gradient w.r.t. activations of the hidden layer.
    Mat hidden_act_grad;

    //! Used to store gradient w.r.t. expectations of the visible layer.
    Mat visible_exp_grad;

    //! Used to store gradient w.r.t. activations of the visible layer.
    Mat visible_act_grad;

    //! Used to store gradient w.r.t. bias of visible layer
    Vec visible_bias_grad;

    //! Used to cache the hidden layer expectations and activations
    Mat hidden_exp_store;
    Mat hidden_act_store;

    //! names of the ports
    TVec<string> ports;
    map<string,int> portname_to_index;
    int& portname2index(string name) 
    { 
        map<string,int>::iterator it=portname_to_index.find(name);
        if (it==portname_to_index.end()) 
            PLERROR("RBMModule: asking for unknown port name %s",name.c_str());
        return it->second;
    }
    void addportname(string name) { ports.append(name); portname_to_index[name]=ports.length()-1; }
    //#####  Protected Member Functions  ######################################

    //! Forward the given learning rate to all elements of this module.
    void setAllLearningRates(real lr);

    //! Declares the class options.
    static void declareOptions(OptionList& ol);

    Mat* bias;

    void computeHiddenActivations(Mat& visible) {
        connection->setAsDownInputs(visible);
        hidden_layer->getAllActivations(connection, 0, true);
        if (bias && !bias->isEmpty())
            hidden_layer->activations += *bias;
    }
    void sampleHiddenGivenVisible(Mat& visible) {
        computeHiddenActivations(visible);
        hidden_layer->generateSamples();
    }
    void computeVisibleActivations(Mat& hidden, bool using_reconstruction_connection=false) {
        if (using_reconstruction_connection)
        {
            reconstruction_connection->setAsUpInputs(hidden);
            visible_layer->getAllActivations(reconstruction_connection, 0, true);
        }
        else
        {
            connection->setAsUpInputs(hidden);
            visible_layer->getAllActivations(connection, 0, true);
        }
    }
    void sampleVisibleGivenHidden(Mat& hidden) {
        computeVisibleActivations(hidden);
        visible_layer->generateSamples();
    }

private:
    //#####  Private Member Functions  ########################################

    //! This does the actual building.
    void build_();

private:
    //#####  Private Data Members  ############################################

    // The rest of the private stuff goes here
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(RBMModule);

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