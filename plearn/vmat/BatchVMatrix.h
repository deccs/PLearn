// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 2003 Olivier Delalleau
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
   * $Id: BatchVMatrix.h,v 1.1 2003/05/21 19:31:52 tihocan Exp $
   ******************************************************* */


#ifndef BatchVMatrix_INC
#define BatchVMatrix_INC

#include "VMat.h"

namespace PLearn <%
using namespace std;

//! VMat class that replicates small parts of a matrix (mini-batches), so that
//! each mini-batch appears twice (consecutively).
class BatchVMatrix: public VMatrix
{

  typedef VMatrix inherited;

public:

  //! Public build options
  VMat m;         //< the source matrix
  int batch_size; //< the size of each mini_batch

private:

  int last_batch; // contains the index of the last mini-batch (=n_batches-1)
  int last_batch_size; // contains the size of the last batch

public:

  BatchVMatrix() {}

  PLEARN_DECLARE_OBJECT_METHODS(BatchVMatrix, "BatchVMatrix", VMatrix);

  static void declareOptions(OptionList& ol);
    
  static string help();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  virtual void build();
  virtual real get(int i, int j) const;
  virtual void put(int i, int j, real value);

private:

  void build_();

};

%> // end of namespcae PLearn
#endif
