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
   * $Id: RowBufferedVMatrix.h,v 1.6 2004/04/05 23:14:13 morinf Exp $
   * This file is part of the PLearn library.
   ******************************************************* */


/*! \file PLearnLibrary/PLearnCore/VMat.h */

#ifndef RowBufferedRowBufferedVMatrix_INC
#define RowBufferedRowBufferedVMatrix_INC

#include "VMatrix.h"

namespace PLearn {
using namespace std;

class RowBufferedVMatrix: public VMatrix
{
  typedef VMatrix inherited;

protected:
  mutable int current_row_index;
  mutable Vec current_row;
  mutable int other_row_index; //!<  used by dot
  mutable Vec other_row;

public:
  RowBufferedVMatrix();
  RowBufferedVMatrix(int the_length, int the_width);

  //!  This is the only method requiring implementation
  virtual void getRow(int i, Vec v) const =0;

  //!  These methods are implemented by buffering calls to getRow
  virtual real get(int i, int j) const; //!<  returns element (i,j)
  virtual void getSubRow(int i, int j, Vec v) const; //!<  fills v with the subrow i laying between columns j (inclusive) and j+v.length() (exclusive)

  virtual real dot(int i1, int i2, int inputsize) const;
  virtual real dot(int i, const Vec& v) const;

  PLEARN_DECLARE_ABSTRACT_OBJECT(RowBufferedVMatrix);
};

DECLARE_OBJECT_PTR(RowBufferedVMatrix);

} // end of namespace PLearn
#endif
