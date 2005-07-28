// -*- C++ -*-

// RegressionTreeRegisters.h
// Copyright (c) 1998-2002 Pascal Vincent
// Copyright (C) 1999-2002 Yoshua Bengio and University of Montreal
// Copyright (c) 2002 Jean-Sebastien Senecal, Xavier Saint-Mleux, Rejean Ducharme
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


/* *********************************************************************************    
   * $Id: RegressionTreeRegisters.h, v 1.0 2004/07/19 10:00:00 Bengio/Kegl/Godbout *
   * This file is part of the PLearn library.                                      *
   ********************************************************************************* */

#ifndef RegressionTreeRegisters_INC
#define RegressionTreeRegisters_INC

#include <plearn/base/Object.h>
#include <plearn/base/stringutils.h>
#include <plearn/math/TMat.h>
#include <plearn/vmat/VMat.h>

namespace PLearn {
using namespace std;

class RegressionTreeRegisters: public Object
{
  typedef Object inherited;
  
private:

/*
  Build options: they have to be set before training
*/

  int  report_progress;
  int  verbosity;
  VMat train_set;
  
/*
  Learnt options: they are sized and initialized if need be, at initRegisters(...) or reinitRegisters()
*/

  int       next_id;
  int       length;
  int       width;
  int       inputsize;
  int       targetsize;
  int       weightsize;  
  TMat<int> sorted_row;
  TMat<int> inverted_sorted_row;
  TVec<int> leave_register;
  TVec<int> leave_candidate;
 
/*
  Work fields: they are sized and initialized if need be, at buid time
*/  
 
  int  each_train_sample_index;
  int  next_train_sample_index;
  int  saved_index;
  int  sample_dim;
  real sample_feature;
  
public:
                       RegressionTreeRegisters();
  virtual              ~RegressionTreeRegisters();
    
    PLEARN_DECLARE_OBJECT(RegressionTreeRegisters);

  static  void         declareOptions(OptionList& ol);
  virtual void         makeDeepCopyFromShallowCopy(CopiesMap &copies);
  virtual void         build();
          void         initRegisters(VMat train_set);
          void         reinitRegisters();
          void         applyForRow(int leave_id, int row);
          void         registerLeave(int leave_id, int row);
          real         getFeature(int row, int col);
          real         getTarget(int row);
          real         getWeight(int row);
          int          getLength();
          int          getNextId();
          int          getInputsize();
          int          getNextRegisteredRow(int leave_id, int col, int previous_row);
          int          getNextCandidateRow(int leave_id, int col, int previous_row);
          void         sortRows();
          void         printRegisters();
private:
          void         build_();
          void         sortEachDim(int dim);
          void         sortSmallSubArray(int start_index, int end_index, int dim);
          void         swapIndex(int index_i, int index_j, int dim);
          real         compare(real field1, real field2);
          void         verbose(string msg, int level);
};

  DECLARE_OBJECT_PTR(RegressionTreeRegisters);

} // end of namespace PLearn

#endif

