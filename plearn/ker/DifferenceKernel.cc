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
   * $Id: DifferenceKernel.cc,v 1.3 2004/04/02 19:56:54 tihocan Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "DifferenceKernel.h"
#include "SelectedOutputCostFunction.h"

// From Old Kernel.cc: all includes are putted in every file.
// To be revised manually 
/*#include <cmath>
#include "stringutils.h"
#include "Kernel.h"
#include "TMat_maths.h"
#include "PLMPI.h"*/
//////////////////////////
namespace PLearn {
using namespace std;


PLEARN_IMPLEMENT_OBJECT(DifferenceKernel, "ONE LINE DESCR", "NO HELP");
real DifferenceKernel::evaluate(const Vec& x1, const Vec& x2) const
{ 
  real result = 0.0;
  for(int i=0; i<x1.length(); i++)
    result += x1[i]-x2[i];
  return result;
}

void DifferenceKernel::write(ostream& out) const
{
  writeHeader(out,"DifferenceKernel"); 
  inherited::oldwrite(out);
  writeFooter(out,"DifferenceKernel");
}

void DifferenceKernel::oldread(istream& in)
{
  readHeader(in,"DifferenceKernel");
  inherited::oldread(in);
  readFooter(in,"DifferenceKernel");
}

CostFunc output_minus_target(int singleoutputindex)
{
  if(singleoutputindex>=0)
    return new SelectedOutputCostFunction(new DifferenceKernel(),singleoutputindex); 
  else
    return new DifferenceKernel(); 
}

} // end of namespace PLearn

