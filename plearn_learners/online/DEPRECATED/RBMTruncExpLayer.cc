// -*- C++ -*-

// RBMTruncExpLayer.cc
//
// Copyright (C) 2006 Pascal Lamblin & Dan Popovici
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

// Authors: Pascal Lamblin & Dan Popovici

/*! \file PLearn/plearn_learners/online/DEPRECATED/RBMTruncExpLayer.cc */

#include "RBMTruncExpLayer.h"
#include <plearn/math/TMat_maths.h>
#include "RBMParameters.h"

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(
    RBMTruncExpLayer,
    "RBM Layer where unit distribution is a truncated exponential in [0,1]",
    "");

RBMTruncExpLayer::RBMTruncExpLayer()
{
}

RBMTruncExpLayer::RBMTruncExpLayer( int the_size )
{
    size = the_size;
    units_types = string( the_size, 'l' );
    activations.resize( the_size );
    sample.resize( the_size );
    expectation.resize( the_size );
    expectation_is_up_to_date = false;
}

//! Uses "rbmp" to obtain the activations of unit "i" of this layer.
//! This activation vector is computed by the "i+offset"-th unit of "rbmp"
void RBMTruncExpLayer::getUnitActivations( int i, PP<RBMParameters> rbmp,
                                           int offset )
{
    Vec activation = activations.subVec( i, 1 );
    rbmp->computeUnitActivations( i+offset, 1, activation );
    expectation_is_up_to_date = false;
}

//! Uses "rbmp" to obtain the activations of all units in this layer.
//! Unit 0 of this layer corresponds to unit "offset" of "rbmp".
void RBMTruncExpLayer::getAllActivations( PP<RBMParameters> rbmp, int offset )
{
    rbmp->computeUnitActivations( offset, size, activations );
    expectation_is_up_to_date = false;
}

void RBMTruncExpLayer::generateSample()
{
    /* The cumulative is :
     * C(U) = P(u<U | x) = (1 - exp(-U a)) / (1 - exp(-a)) if 0 < U < 1,
     *        0 if U <= 0 and
     *        1 if 1 <= U
     *
     * And the inverse, if 0 <= s <=1:
     * C^{-1}(s) = - log(1 - s*(1 - exp(-a)) / a
     */

    for( int i=0 ; i<size ; i++ )
    {
        real s = random_gen->uniform_sample();
        real a_i = activations[i];
        sample[i] = - pl_log( 1. - s*( 1 - exp(-a_i) ) ) / a_i;
    }
}

void RBMTruncExpLayer::computeExpectation()
{
    if( expectation_is_up_to_date )
        return;

    /* Conditional expectation:
     * E[u|x] = 1/(1-exp(a)) + 1/a
     */

    for( int i=0 ; i<size ; i++ )
    {
        real a_i = activations[i];
        expectation[i] = 1/(1-exp(a_i)) + 1/a_i;
    }

    expectation_is_up_to_date = true;
}

void RBMTruncExpLayer::bpropUpdate(const Vec& input, const Vec& output,
                                   Vec& input_gradient,
                                   const Vec& output_gradient)
{
    PLASSERT( input.size() == size );
    PLASSERT( output.size() == size );
    PLASSERT( output_gradient.size() == size );
    input_gradient.resize( size );

    // df/da = exp(a)/(1-exp(a))^2 - 1/a^2

    for( int i=0 ; i<size ; i++ )
    {
        real a_i = input[i];
        real ea_i = exp( a_i );
        input_gradient[i] = ea_i/( (1 - ea_i) * (1 - ea_i) ) + 1/(a_i * a_i);
    }
}



void RBMTruncExpLayer::declareOptions(OptionList& ol)
{
/*
    declareOption(ol, "size", &RBMTruncExpLayer::size,
                  OptionBase::buildoption,
                  "Number of units.");
*/
    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void RBMTruncExpLayer::build_()
{
    if( size < 0 )
        size = int(units_types.size());
    if( size != (int) units_types.size() )
        units_types = string( size, 'l' );

    activations.resize( size );
    sample.resize( size );
    expectation.resize( size );
    expectation_is_up_to_date = false;
}

void RBMTruncExpLayer::build()
{
    inherited::build();
    build_();
}


void RBMTruncExpLayer::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);
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
