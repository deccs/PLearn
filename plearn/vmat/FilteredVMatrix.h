// -*- C++ -*-

// FilteredVMatrix.h
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
   * $Id: FilteredVMatrix.h,v 1.9 2004/07/07 17:30:48 tihocan Exp $ 
   ******************************************************* */

// Authors: Pascal Vincent

/*! \file FilteredVMatrix.h */


#ifndef FilteredVMatrix_INC
#define FilteredVMatrix_INC

#include "SourceVMatrix.h"
#include "VMatLanguage.h"

namespace PLearn {
using namespace std;

class FilteredVMatrix: public SourceVMatrix
{
  typedef SourceVMatrix inherited;

private:

  //! A custom hack to make sure openIndex() is not called before
  //! the build is complete.
  bool build_complete;
  
protected:

  VMatLanguage program;
  IntVecFile indexes;  // indexes[i] is the

  //! Generates the index file if it does not already exist.
  //! If it exists and is up to date, simply opens it.
  void openIndex();

public:

  // ************************
  // * public build options *
  // ************************

  string prg;  // program string in VPL language

  // ****************
  // * Constructors *
  // ****************

  //! Default constructor.
  FilteredVMatrix();

  //! Convenience constructor.
  FilteredVMatrix(VMat the_source, const string& program_string, const string& the_metadatadir = "");

  virtual void setMetaDataDir(const string& the_metadatadir);

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
  PLEARN_DECLARE_OBJECT(FilteredVMatrix);

};

DECLARE_OBJECT_PTR(FilteredVMatrix);

} // end of namespace PLearn
#endif
