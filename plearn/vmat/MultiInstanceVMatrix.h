// -*- C++ -*-

// MultiInstanceVMatrix.h
//
// Copyright (C) 2004 Norman Casagrande 
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
   * $Id: MultiInstanceVMatrix.h,v 1.2 2004/02/25 05:20:34 nova77 Exp $ 
   ******************************************************* */

// Authors: Norman Casagrande

/*! \file MultiInstanceVMatrix.h */


#ifndef MultiInstanceVMatrix_INC
#define MultiInstanceVMatrix_INC

#include <vector>
#include <utility>
#include "RowBufferedVMatrix.h"

namespace PLearn {
using namespace std;

class MultiInstanceVMatrix: public RowBufferedVMatrix
{
public:
  typedef RowBufferedVMatrix inherited;

protected:

  Mat         data_;
  string      specification_;  // The file name
  vector<int> configs_;
  vector< pair<string, int> > names_;

public:

  // ************************
  // * public build options *
  // ************************

  // ### declare public option fields (such as build options) here
  // ...

  // ****************
  // * Constructors *
  // ****************

  // Default constructor.
  MultiInstanceVMatrix();
  //MultiInstanceVMatrix(const string& filename); //!<  opens an existing file

  // ******************
  // * Object methods *
  // ******************

private: 
  //! This does the actual building. 
  // (Please implement in .cc)
  void build_();

protected: 

  static void declareOptions(OptionList& ol);

public:
  virtual void getRow(int i, Vec v) const;

  virtual void build();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  //! Declares name and deepCopy methods
  PLEARN_DECLARE_OBJECT(MultiInstanceVMatrix);

};
DECLARE_OBJECT_PTR(MultiInstanceVMatrix);

} // end of namespace PLearn
#endif
