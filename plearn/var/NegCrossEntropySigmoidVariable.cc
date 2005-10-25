// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio, Rejean Ducharme and University of Montreal
// Copyright (C) 2001-2002 Nicolas Chapados, Ichiro Takeuchi, Jean-Sebastien Senecal
// Copyright (C) 2002 Xiangdong Wang, Christian Dorion
// Copyright (C) 2003 Olivier Delalleau

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
 * $Id$
 * This file is part of the PLearn library.
 ******************************************************* */

#include "NegCrossEntropySigmoidVariable.h"

namespace PLearn {
using namespace std;

/** NegCrossEntropySigmoidVariable **/

PLEARN_IMPLEMENT_OBJECT(NegCrossEntropySigmoidVariable,
                        "Compute sigmoid of its first input, and then computes the negative "
                        "cross-entropy cost",
                        "NO HELP");

////////////////////////////////////
// NegCrossEntropySigmoidVariable //
////////////////////////////////////
NegCrossEntropySigmoidVariable::NegCrossEntropySigmoidVariable(Variable* netout, Variable* target, real regularizer_)
    : inherited(netout,target,1,1),regularizer(regularizer_)
{
    build_();
}

void
NegCrossEntropySigmoidVariable::build()
{
    inherited::build();
    build_();
}

void
NegCrossEntropySigmoidVariable::build_()
{
    if (input1 && input2) {
        // input1 and input2 are (respectively) netout and target from constructor
        if(input1->size() != input2->size())
            PLERROR("In NegCrossEntropySigmoidVariable: netout and target must have the same size");
    }
}

///////////////////
// recomputeSize //
///////////////////
void NegCrossEntropySigmoidVariable::recomputeSize(int& l, int& w) const
{ l=1, w=1; }

///////////
// fprop //
///////////
void NegCrossEntropySigmoidVariable::fprop()
{
    real cost = 0.0;
    for (int i=0; i<input1->size(); i++)
    {
        real output = sigmoid(input1->valuedata[i]);
        real target = input2->valuedata[i];
        if (fast_exact_is_equal(output,0.0)) {
            if (fast_exact_is_equal(target, 1.0)) {
                PLWARNING("NegCrossEntropySigmoidVariable::fprop: model output is 0 and target is 1, cost should be infinite !");
                cost += -1e9;
            } // If target == 0.0 do nothing, cost is 0.
        } else if (fast_exact_is_equal(output, 1.0)) {
            if (fast_exact_is_equal(target, 0.0)) {
                PLWARNING("NegCrossEntropySigmoidVariable::fprop: model output is 1 and target is 0, cost should be infinite !");
                cost += -1e9;
            } // If target == 1.0 do nothing, cost is 0.
        } else {
            if (fast_exact_is_equal(regularizer, 0)) {
                // Standard cross entropy.
                cost += target*log(output) + (1.0-target)*log(1.0-output);
            } else {
                // Regularized cross entropy.
                cost += target*((1 - regularizer) * log(output) + regularizer * log(1.0 - output)) +
                    (1.0-target)*((1 - regularizer) * log(1.0-output) + regularizer * log(output));
            }
        }
    }
    valuedata[0] = -cost;
}

///////////
// bprop //
///////////
void NegCrossEntropySigmoidVariable::bprop()
{
    real gr = *gradientdata;
    for (int i=0; i<input1->size(); i++)
    {
        real output = sigmoid(input1->valuedata[i]);
        real target = input2->valuedata[i];
        if (fast_exact_is_equal(regularizer, 0)) {
            // Standard cross entropy.
            input1->gradientdata[i] += gr*(output - target);
        } else {
            // Regularized cross entropy.
            if (fast_exact_is_equal(target, 0.0)) {
                input1->gradientdata[i] += gr*((1-regularizer) * output - regularizer * (1-output));
            } else if (fast_exact_is_equal(target, 1.0)) {
                input1->gradientdata[i] += gr*(regularizer * output - (1-regularizer) * (1-output));
            } else {
                PLERROR("NegCrossEntropySigmoidVariable::bprop: target is neither 0 nor 1");
            }
        }
    }
}

////////////////////
// setRegularizer //
////////////////////
void NegCrossEntropySigmoidVariable::setRegularizer(real r)
{
    PLWARNING("NegCrossEntropySigmoidVariable::setRegularizer() has been deprecated, use the setOption() method instead");
    this->regularizer = r;
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
