// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio, Rejean Ducharme and University of Montreal
// Copyright (C) 2001-2002 Nicolas Chapados, Ichiro Takeuchi, Jean-Sebastien Senecal
// Copyright (C) 2002 Xiangdong Wang, Christian Dorion

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
   * $Id: EqualScalarVariable.cc,v 1.5 2004/04/27 15:58:16 morinf Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "EqualScalarVariable.h"

namespace PLearn {
using namespace std;


/** EqualScalarVariable **/

PLEARN_IMPLEMENT_OBJECT(EqualScalarVariable,
                        "A scalar var;  equal 1 if input1==input2, 0 otherwise",
                        "NO HELP");

EqualScalarVariable::EqualScalarVariable(Variable* input1, Variable* input2)
  : inherited(input1, input2, input1->length(), input1->width())
{
    build_();
}

void
EqualScalarVariable::build()
{
    inherited::build();
    build_();
}

void
EqualScalarVariable::build_()
{
    if(input2 && !input2->isScalar())
        PLERROR("IsSmallerVariable EqualScalarVariable(Variable* input1, Variable* input2) input2 must be a scalar");
}

  
void EqualScalarVariable::recomputeSize(int& l, int& w) const
{
    if (input1) {
        l = input1->length();
        w = input1->width();
    } else
        l = w = 0;
}

void EqualScalarVariable::fprop()
{
  real eqv = input2->valuedata[0];
  for(int i=0; i<nelems(); i++)
    valuedata[i] = (input1->valuedata[i] == eqv);
}


// not really differentiable (zero gradient almost everywhere)
void EqualScalarVariable::bprop() {}

void EqualScalarVariable::symbolicBprop() {}


} // end of namespace PLearn


