// -*- C++ -*-

// ClassErrorCostModule.h
//
// Copyright (C) 2007 Pascal Lamblin
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

/*! \file ClassErrorCostModule.h */


#ifndef ClassErrorCostModule_INC
#define ClassErrorCostModule_INC

#include <plearn_learners/online/CostModule.h>

namespace PLearn {

/**
 * Multiclass classification error.
 * If input_size > 1, outputs 0 if target == argmax(input), 1 else
 * If input_size == 1, outputs 0 if target is the closest integer to
 * input[0], 1 else.
 * There is no gradient to compute (it returns an error if you try), so if you
 * use this module inside a CombiningCostsModule, put its weight to 0.
 */
class ClassErrorCostModule : public CostModule
{
    typedef CostModule inherited;

public:
    //#####  Public Build Options  ############################################
    
    Mat error_costs;

public:
    //#####  Public Member Functions  #########################################

    //! Default constructor
    ClassErrorCostModule();

    //! Given the input and the target, compute a vector of costs
    //! (possibly resize it appropriately)
    virtual void fprop(const Vec& input, const Vec& target, Vec& cost) const;

    //! Overridden from parent class.
    virtual void fprop(const Mat& inputs, const Mat& targets, Mat& costs)
        const;

    //! Given the input and the target, compute only the first cost
    //! (of which we will compute the gradient)
    virtual void fprop(const Vec& input, const Vec& target, real& cost) const;

    //! Nothing to do
    virtual void bpropUpdate(const Vec& input, const Vec& target, real cost);

    /*
    //! No differentiable, so no gradient to backprop!
    virtual void bpropUpdate(const Vec& input, const Vec& target, real cost,
                             Vec& input_gradient);
    */

    //! Nothing to do
    virtual void bbpropUpdate(const Vec& input, const Vec& target, real cost);

    virtual void bpropUpdate(const Mat& inputs, const Mat& targets,
            const Vec& costs)
    {}

    /* Optional
       N.B. A DEFAULT IMPLEMENTATION IS PROVIDED IN THE SUPER-CLASS, WHICH
       RAISES A PLERROR.
    //! No differentiable, so no gradient to backprop!
    virtual void bbpropUpdate(const Vec& input, const Vec& target, real cost,
                              Vec& input_gradient, Vec& input_diag_hessian);
    */

    //! Reset the parameters to the state they would be BEFORE starting
    //! training.  Note that this method is necessarily called from
    //! build().
    virtual void forget();

    //! Does nothing (there isn't a learning rate in this class)
    virtual void setLearningRate(real dynamic_learning_rate) {}

    //! In case bpropUpdate does not do anything, make it known
    virtual bool bpropDoesNothing();

    //! Indicates the name of the computed costs
    virtual TVec<string> costNames();


    //#####  PLearn::Object Protocol  #########################################

    // Declares other standard object methods.
    PLEARN_DECLARE_OBJECT(ClassErrorCostModule);

    // Simply calls inherited::build() then build_()
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);


protected:
    //#####  Protected Member Functions  ######################################

    //! Declares the class options.
    static void declareOptions(OptionList& ol);

private:
    //#####  Private Member Functions  ########################################

    //! This does the actual building.
    void build_();

private:
    //#####  Private Data Members  ############################################
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(ClassErrorCostModule);

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
