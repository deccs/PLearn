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
   * $Id: SumVariable.cc,v 1.2 2003/01/08 21:33:04 ducharme Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "SumVariable.h"

namespace PLearn <%
using namespace std;



/** SumVariable **/

SumVariable::SumVariable(Variable* input)
  :UnaryVariable(input, 1, 1) {}


IMPLEMENT_NAME_AND_DEEPCOPY(SumVariable);

void SumVariable::recomputeSize(int& l, int& w) const
{ l=1; w=1; }


void SumVariable::deepRead(istream& in, DeepReadMap& old2new)
{
  readHeader(in, "SumVariable");
  inherited::deepRead(in, old2new);
  readFooter(in, "SumVariable");
}


void SumVariable::deepWrite(ostream& out, DeepWriteSet& already_saved) const
{
  writeHeader(out, "SumVariable");
  inherited::deepWrite(out, already_saved);
  writeFooter(out, "SumVariable");
}


void SumVariable::fprop()
{
  real sum = 0.0;
  for(int k=0; k<input->nelems(); k++)
    sum += input->valuedata[k];
  valuedata[0] = sum;
}


void SumVariable::bprop()
{
  real grd = gradientdata[0];
  for(int k=0; k<input->nelems(); k++)
    input->gradientdata[k] += grd;
}


void SumVariable::symbolicBprop()
{
  input->accg(g);
}


// R(sum_i x_i) = sum_i R(x_i)
void SumVariable::rfprop()
{
  if (rValue.length()==0) resizeRValue();
  real sum = 0.0;
  for(int k=0; k<input->nelems(); k++)
    sum += input->rvaluedata[k];
  rvaluedata[0] = sum;
}



%> // end of namespace PLearn


