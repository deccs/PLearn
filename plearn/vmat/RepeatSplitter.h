// -*- C++ -*-

// RepeatSplitter.h
//
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
   * $Id: RepeatSplitter.h,v 1.6 2004/03/23 23:08:08 morinf Exp $ 
   ******************************************************* */

/*! \file RepeatSplitter.h */

#ifndef RepeatSplitter_INC
#define RepeatSplitter_INC

#include "Splitter.h"

namespace PLearn {
using namespace std;

class RepeatSplitter: public Splitter
{
  typedef Splitter inherited;

protected:

  // *********************
  // * protected options *
  // *********************

  //! A matrix where the i-th row is the vector of the indices of the i-th
  //! shuffled matrix (used only when shuffle = 1).
  TMat<int> indices;

  //! Used to know if we need to reshuffle the matrix.
  int last_n;
    
public:

  // ************************
  // * public build options *
  // ************************

  bool do_not_shuffle_first;
  int n;
  long seed;
  int shuffle;
  PP<Splitter> to_repeat;

  // ****************
  // * Constructors *
  // ****************

  RepeatSplitter();


  // ******************
  // * Object methods *
  // ******************

  //! Set the dataset on which the splits are to be based.
  virtual void setDataSet(VMat the_dataset);

private: 
  //! This does the actual building. 
  // (Please implement in .cc)
  void build_();

protected: 
  //! Declares this class' options
  // (Please implement in .cc)
  static void declareOptions(OptionList& ol);

public:
  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  //! Declares name and deepCopy methods
  PLEARN_DECLARE_OBJECT(RepeatSplitter);


  // ********************************
  // *        Splitter methods      *
  // * (must be implemented in .cc) *
  // ********************************

  //! Returns the number of available different "splits"
  virtual int nsplits() const;

  //! Returns the number of sets per split
  virtual int nSetsPerSplit() const;

  //! Returns split number i
  virtual TVec<VMat> getSplit(int i=0);

};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(RepeatSplitter);
  
} // end of namespace PLearn

#endif
