
// -*- C++ -*-

// LearnerProcessedVMatrix.h
//
// Copyright (C) 2003  Pascal Vincent 
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
   * $Id: LearnerProcessedVMatrix.h,v 1.9 2004/07/21 16:30:55 chrish42 Exp $ 
   ******************************************************* */

/*! \file LearnerProcessedVMatrix.h */
#ifndef LearnerProcessedVMatrix_INC
#define LearnerProcessedVMatrix_INC

#include "RowBufferedVMatrix.h"
#include <plearn_learners/generic/PLearner.h>

namespace PLearn {
using namespace std;

class LearnerProcessedVMatrix: public RowBufferedVMatrix
{
  typedef RowBufferedVMatrix inherited;

protected:
  // *********************
  // * protected options *
  // *********************

public:

  // ************************
  // * public build options *
  // ************************

  //! The source vmatrix whose inputs wil be porcessed by the learner
  //! If present, the target and weight columns will be appended to the processed input in the resulting matrix.
  VMat source; 

  //! The learner used to process the VMat's input.
  PP<PLearner> learner;

  //! Indicates if the learner should be trained on the source, and on what part
  //! '0': don't train
  //! 'S': supervised training using input and target (possibly weighted if weight is  present)
  //! 'U': unsupervised training using only input part (possibly weighted if weight is present). 
  char train_learner; 

  // ****************
  // * Constructors *
  // ****************

  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  LearnerProcessedVMatrix();

  // ******************
  // * Object methods *
  // ******************

private: 
  //! This does the actual building. 
  // (Please implement in .cc)
  void build_();

protected: 
  //! Declares this class' options
  // (Please implement in .cc)
  static void declareOptions(OptionList& ol);

  //!  This is the only method requiring implementation
  virtual void getNewRow(int i, const Vec& v) const;

public:
  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  //! Declares name and deepCopy methods
  PLEARN_DECLARE_OBJECT(LearnerProcessedVMatrix);
};

DECLARE_OBJECT_PTR(LearnerProcessedVMatrix);

} // end of namespace PLearn
#endif
