// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio and University of Montreal
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


/* *******************************************************      
   * $Id: CompactVMatrixPolynomialKernel.h,v 1.5 2004/04/07 23:15:58 morinf Exp $
   * This file is part of the PLearn library.
   ******************************************************* */


/*! \file PLearnLibrary/PLearnCore/Kernel.h */

#ifndef CompactVMatrixPolynomialKernel_INC
#define CompactVMatrixPolynomialKernel_INC

#include "Kernel.h"
#include "CompactVMatrix.h"

namespace PLearn {
using namespace std;

/*!   behaves like PolynomialKernel except that the x1 and x2 vectors
  actually only contain INDICES of the rows of a CompactVMatrix,
  and the square difference is performed efficiently, taking
  advantage of the discrete nature of many fields.
*/
class CompactVMatrixPolynomialKernel: public Kernel
{
  typedef Kernel inherited;
		
protected:
    int n; //!<  degree of polynomial
    real beta; //!<  a normalization constant for numerical stability
    PP<CompactVMatrix> m;
public:
    CompactVMatrixPolynomialKernel()
        : n(), beta() {}
    CompactVMatrixPolynomialKernel(int degree, PP<CompactVMatrix>& vm, real the_beta=1.0)
        : n(degree), beta(the_beta), m(vm) {}

    PLEARN_DECLARE_OBJECT(CompactVMatrixPolynomialKernel);

    virtual real evaluate(const Vec& x1, const Vec& x2) const;    

protected:
    //!  recognized options are "n", "beta" and "m"
    static void declareOptions(OptionList &ol);
};

DECLARE_OBJECT_PTR(CompactVMatrixPolynomialKernel);

} // end of namespace PLearn

#endif
