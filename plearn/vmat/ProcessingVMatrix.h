// -*- C++ -*-

// ProcessingVMatrix.h
//
// Copyright (C) 2003 Pascal Vincent 
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
   * $Id: ProcessingVMatrix.h,v 1.1 2003/10/08 23:01:56 plearner Exp $ 
   ******************************************************* */

// Authors: Pascal Vincent

/*! \file ProcessingVMatrix.h */


#ifndef ProcessingVMatrix_INC
#define ProcessingVMatrix_INC

#include "SourceVMatrix.h"
#include "VMatLanguage.h"

namespace PLearn <%
using namespace std;

class ProcessingVMatrix: public SourceVMatrix
{
protected:
  Vec sourcevec;
  VMatLanguage program;

public:

  typedef SourceVMatrix inherited;

  // ************************
  // * public build options *
  // ************************

  string prg;  // program string in VPL language

  // ****************
  // * Constructors *
  // ****************

  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  ProcessingVMatrix();

  ProcessingVMatrix(VMat the_source, const string& program_string)
    :SourceVMatrix(the_source),prg(program_string)
  { build_(); }

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

public:
  //!  This is the only method requiring implementation
  virtual void getRow(int i, Vec v) const;

  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  //! Declares name and deepCopy methods
  PLEARN_DECLARE_OBJECT(ProcessingVMatrix);

};

%> // end of namespace PLearn
#endif
