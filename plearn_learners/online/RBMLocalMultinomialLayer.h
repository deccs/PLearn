// -*- C++ -*-

// RBMLocalMultinomialLayer.h
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

// Author: Pascal Lamblin

/*! \file RBMLocalMultinomialLayer.h */


#ifndef RBMLocalMultinomialLayer_INC
#define RBMLocalMultinomialLayer_INC

#include "RBMLayer.h"

namespace PLearn {
using namespace std;

/**
 * Multiple multinomial units, each of them seeing an area of nearby pixels
 *
 */
class RBMLocalMultinomialLayer: public RBMLayer
{
    typedef RBMLayer inherited;

public:
    //#####  Public Build Options  ############################################

    //! Number of images present at the same time in the input vector
    int n_images;

    //! Length of each of the images
    int images_length;

    //! Width of each of the images
    int images_width;

    //! Length of the areas to consider
    int area_length;

    //! Width of the areas to consider
    int area_width;

    //#####  Learnt Options  ##################################################
    int images_size;
    int area_size;
    int n_areas;

public:
    //#####  Public Member Functions  #########################################

    //! Default constructor
    RBMLocalMultinomialLayer( real the_learning_rate=0. );

    //! Constructor from the number of units in the multinomial
    RBMLocalMultinomialLayer( int the_size, real the_learning_rate=0. );


    //! generate a sample, and update the sample field
    virtual void generateSample();

    //! batch version
    virtual void generateSamples();

    //! compute the expectation
    virtual void computeExpectation();

    //! batch version
    virtual void computeExpectations();

    //! forward propagation
    virtual void fprop( const Vec& input, Vec& output ) const;

    //! forward propagation with provided bias
    virtual void fprop( const Vec& input, const Vec& rbm_bias,
                        Vec& output ) const;

    //! back-propagates the output gradient to the input
    virtual void bpropUpdate(const Vec& input, const Vec& output,
                             Vec& input_gradient, const Vec& output_gradient,
                             bool accumulate=false);

    //! Back-propagate the output gradient to the input, and update parameters.
    virtual void bpropUpdate(const Mat& inputs, const Mat& outputs,
                             Mat& input_gradients,
                             const Mat& output_gradients,
                             bool accumulate = false);

    //! back-propagates the output gradient to the input and the bias
    virtual void bpropUpdate(const Vec& input, const Vec& rbm_bias,
                             const Vec& output,
                             Vec& input_gradient, Vec& rbm_bias_gradient,
                             const Vec& output_gradient) ;

    //! Computes the negative log-likelihood of target given the
    //! internal activations of the layer
    virtual real fpropNLL(const Vec& target);
    virtual void fpropNLL(const Mat& targets, const Mat& costs_column);

    //! Computes the gradient of the negative log-likelihood of target
    //! with respect to the layer's bias, given the internal activations
    virtual void bpropNLL(const Vec& target, real nll, Vec& bias_gradient);
    virtual void bpropNLL(const Mat& targets, const Mat& costs_column,
                          Mat& bias_gradients);

    // Compute -bias' unit_values
    virtual real energy(const Vec& unit_values) const;

    //! Computes \f$ -log(\sum_{possible values of h} exp(h' unit_activations))\f$
    //! This quantity is used for computing the free energy of a sample x in
    //! the OTHER layer of an RBM, from which unit_activations was computed.
    virtual real freeEnergyContribution(const Vec& unit_activations) const;

    virtual int getConfigurationCount();

    virtual void getConfiguration(int conf_index, Vec& output);

    //#####  PLearn::Object Protocol  #########################################

    // Declares other standard object methods.
    PLEARN_DECLARE_OBJECT(RBMLocalMultinomialLayer);

    // Simply calls inherited::build() then build_()
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

protected:
    //#####  Not Options  #####################################################

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

    // The rest of the private stuff goes here
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(RBMLocalMultinomialLayer);

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
