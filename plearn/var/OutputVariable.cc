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
   * $Id: OutputVariable.cc,v 1.1 2004/10/01 19:42:52 mariusmuja Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "OutputVariable.h"
#include "Var_operators.h"
#include <plearn/math/plapack.h>

namespace PLearn {
using namespace std;


/** OutputVariable **/

PLEARN_IMPLEMENT_OBJECT(OutputVariable, "Output var contents to a file.", 
                                        "Just writes the var contents to a file.\n");

OutputVariable::OutputVariable(Variable* input, char* filename)
  : inherited(input, input->length(),input->width()) 
{
    output_file = new ofstream(filename);
}

OutputVariable::~OutputVariable()
{
    output_file->close();
    delete output_file;
}

void OutputVariable::recomputeSize(int& l, int& w) const
{
    if (input) {
        l = input->length();
        w = input->width();
    } else
        l = w = 0;
}


void OutputVariable::fprop()
{
    *output_file << input->value << "\n";

    for(int k=0; k<input->nelems(); k++) {
        valuedata[k] = input->valuedata[k];
    }
}


void OutputVariable::bprop() 
{
    for(int k=0; k<input->nelems(); k++) {
        input->gradientdata[k] += gradientdata[k];
    }
}

void OutputVariable::symbolicBprop()
{
    PLERROR("In OutputVariable::symbolicBprop: feature not implemented");
}


void OutputVariable::rfprop()
{
    PLERROR("In OutputVariable::rfprop: feature not implemented");
}



} // end of namespace PLearn


