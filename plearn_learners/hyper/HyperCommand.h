// -*- C++ -*-

// HyperCommand.h
// 
// Copyright (C) 2003-2004 ApSTAT Technologies Inc.
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

// Author: Pascal Vincent

/* *******************************************************      
   * $Id: HyperCommand.h,v 1.2 2005/01/12 14:42:53 tihocan Exp $ 
   ******************************************************* */

/*! \file HyperCommand.h */
#ifndef HyperCommand_INC
#define HyperCommand_INC

#include <plearn/base/Object.h>
#include <plearn_learners/generic/PLearner.h>

namespace PLearn {
using namespace std;

class HyperLearner;
class VecStatsCollector;

class HyperCommand: public Object
{

private:

  typedef Object inherited;

protected:

  HyperLearner* hlearner;   //!< A 'real' pointer to avoid cycles (and memory leaks)
  string expdir; //<! where to report results

public:

  PLEARN_DECLARE_ABSTRACT_OBJECT(HyperCommand);

  // ****************
  // * Constructors *
  // ****************

  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  HyperCommand();

  // ******************
  // * Object methods *
  // ******************

private: 

  //! This does the actual building. 
  void build_();

protected: 

  //! Declares this class' options
  static void declareOptions(OptionList& ol);

public:
  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

  void setHyperLearner(HyperLearner* the_hlearner)
  { hlearner = the_hlearner; }

  virtual void setExperimentDirectory(const string& the_expdir);

  string getExperimentDirectory() const
  { return expdir; }

  //! Executes the command, returning the resulting costvec of its optimization 
  //! (or an empty vec if it didn't do any testng).
  virtual Vec optimize() =0;

  //! Returns the names of the results returned by the optimize() method
  virtual TVec<string> getResultNames() const =0;

};

// Declares a few other classes and functions related to this class
  DECLARE_OBJECT_PTR(HyperCommand);
  
} // end of namespace PLearn

#endif
