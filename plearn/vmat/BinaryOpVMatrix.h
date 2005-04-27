// -*- C++ -*-

// BinaryOpVMatrix.h
//
// Copyright (C) 2005 Nicolas Chapados 
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
   * $Id: BinaryOpVMatrix.h,v 1.1 2005/04/27 14:21:14 chapados Exp $ 
   ******************************************************* */

// Authors: Nicolas Chapados

/*! \file BinaryOpVMatrix.h */


#ifndef BinaryOpVMatrix_INC
#define BinaryOpVMatrix_INC

#include <plearn/vmat/RowBufferedVMatrix.h>
#include <plearn/vmat/VMat.h>

namespace PLearn {

class BinaryOpVMatrix: public RowBufferedVMatrix
{

private:

  typedef RowBufferedVMatrix inherited;

  static real op_add(double x, double y) { return x+y; }
  static real op_sub(double x, double y) { return x-y; }
  static real op_mul(double x, double y) { return x*y; }
  static real op_div(double x, double y) { return x/y; }

  //! Operation to perform
  real (*selected_op)(double,double);

  //! Implementation buffers
  mutable Vec row1, row2;
  
public:

  //! First VMatrix to operate on
  VMat vm1;

  //! Second VMatrix to operate on
  VMat vm2;

  //! Operation to perform; may be "add", "sub", "mult", "div"
  string op;
  

  //! Default constructor.
  BinaryOpVMatrix();

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

  //! Fill the vector 'v' with the content of the i-th row.
  //! v is assumed to be the right size.
  virtual void getNewRow(int i, const Vec& v) const;

public:

  // Simply call inherited::build() then build_().
  virtual void build();

  //! Transform a shallow copy into a deep copy.
  virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

  //! Declare name and deepCopy methods.
  PLEARN_DECLARE_OBJECT(BinaryOpVMatrix);

};

DECLARE_OBJECT_PTR(BinaryOpVMatrix);

} // end of namespace PLearn
#endif
