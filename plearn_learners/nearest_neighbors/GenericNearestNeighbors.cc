// -*- C++ -*-

// GenericNearestNeighbors.cc
//
// Copyright (C) 2004 Nicolas Chapados
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
   * $Id: GenericNearestNeighbors.cc,v 1.2 2004/12/21 07:13:15 chapados Exp $ 
   ******************************************************* */

// Authors: Nicolas Chapados

/*! \file StatefulLearner.cc */

// From C++ stdlib
#include <algorithm>

// From PLearn
#include "GenericNearestNeighbors.h"

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_ABSTRACT_OBJECT(
  GenericNearestNeighbors,
  "Base class for algorithms that find nearest-neighbors",
  "This class provides an abstract base class for nearest-neighbors-type\n"
  "algorithms.  The basic abstraction is that from a test point, one can\n"
  "ask to find the \"K\" nearest points from the training set.  (Specified\n"
  "through the \"num_neighbors\" option).  Although per se, this class (and\n"
  "its descendants) only FIND the nearest neighbors, the design is such\n"
  "that it can be embedded in concrete algorithms that perform\n"
  "classification or regression.  The separation between \"neighborhood\n"
  "finding\" and \"how to use the neighbors\" allows multiple instantiations\n"
  "of, say, K-Nearest-Neighbors classification, using several (exact and\n"
  "approximate) neighbors-finding algorithms.\n"
  "\n"
  "There are a number of options that control how the output vectors are\n"
  "generated.  (See below).  For a given neighbor found, the output vector\n"
  "is always the concatenation of one or more of (in that order):\n"
  "\n"
  "- The input vector from the training set (option \"copy_input\")\n"
  "- The target vector from the training set (option \"copy_target\")\n"
  "- The weight from the training set (option \"copy_weight\")\n"
  "- The index (row number) of the example from the training set (option\n"
  "  \"copy_weight\")\n"
  "\n"
  "If more than one neighbor is requested, the complete output vector of\n"
  "this learner is simply the concatenation of the above template for\n"
  "creating one output vector.\n"
  "\n"
  "The learner's costs are dependent on the derived classes.  It is\n"
  "suggested that, at least, the similarity measure (Kernel value) between\n"
  "the test and train points be output."
  );

GenericNearestNeighbors::GenericNearestNeighbors()
  : num_neighbors(1),
    copy_input(false),
    copy_target(true),
    copy_weight(true),
    copy_index(false)
{ }

void GenericNearestNeighbors::declareOptions(OptionList& ol)
{
  declareOption(
    ol, "num_neighbors", &GenericNearestNeighbors::num_neighbors,
    OptionBase::buildoption,
    "Number of nearest-neighbors to compute.  This is usually called \"K\".\n"
    "The output vector is simply the concatenation of all found neighbors.\n"
    "(Default = 1)");

  declareOption(
    ol, "copy_input", &GenericNearestNeighbors::copy_input,
    OptionBase::buildoption,
    "If true, the output contains a copy of the found input vector(s).\n"
    "(Default = false)");

  declareOption(
    ol, "copy_target", &GenericNearestNeighbors::copy_target,
    OptionBase::buildoption,
    "If true, the output contains a copy of the found target vector(s).\n"
    "(Default = true)");

  declareOption(
    ol, "copy_weight", &GenericNearestNeighbors::copy_weight,
    OptionBase::buildoption,
    "If true, the output contains a copy of the found weight, if any.\n"
    "(Default = true)");

  declareOption(
    ol, "copy_index", &GenericNearestNeighbors::copy_index,
    OptionBase::buildoption,
    "If true, the output contains the index of the found neighbor\n"
    "(as the row number, zero-based, in the training set.)\n"
    "(Default = false)");
  
  // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

void GenericNearestNeighbors::build_()
{
  //! Just some sanity checking on the options
  if (num_neighbors <= 0)
    PLERROR("GenericNearestNeighbors::build_: the option \"num_neighbors\" "
            "must be strictly positive");
  if (! (copy_input || copy_target || copy_weight || copy_index))
    PLERROR("GenericNearestNeighbors::build_: at least one of the options "
            "\"copy_input\", \"copy_target\", \"copy_weight\", \"copy_index\" "
            "must be specified (i.e. true)");
}

// ### Nothing to add here, simply calls build_
void GenericNearestNeighbors::build()
{
  inherited::build();
  build_();
}


void GenericNearestNeighbors::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  deepCopyField(currow, copies);
  
  inherited::makeDeepCopyFromShallowCopy(copies);
}


int GenericNearestNeighbors::outputsize() const
{
  int base_outputsize = 0;
  assert( train_set );
  if (copy_input)
    base_outputsize += train_set->inputsize();
  if (copy_target)
    base_outputsize += train_set->targetsize();
  if (copy_weight)
    base_outputsize += train_set->weightsize();
  if (copy_index)
    base_outputsize += 1;

  assert( num_neighbors > 0 );
  assert( base_outputsize > 0 );
  
  return num_neighbors * base_outputsize;
}

void GenericNearestNeighbors::constructOutputVector(const TVec<int>& indexes,
                                                    Vec& output) const
{
  assert( output.size() == outputsize() );
  
  int i, n=min(num_neighbors, indexes.size());
  int inputsize = train_set->inputsize();
  int targetsize = train_set->targetsize();
  int weightsize = train_set->weightsize();
  real* output_data = output.data();

  currow.resize(train_set.width());
  for (i=0 ; i<n ; ++i) {
    train_set->getRow(indexes[i], currow);
    real* currow_data = currow.data();

    if(copy_input) {
      copy(currow_data, currow_data+inputsize, output_data);
      output_data += inputsize;
    }
    currow_data += inputsize;
    
    if(copy_target) {
      copy(currow_data, currow_data+targetsize, output_data);
      output_data += targetsize;
    }
    currow_data += targetsize;
    
    if(copy_weight) {
      copy(currow_data, currow_data+weightsize, output_data);
      output_data += weightsize;
    }
    currow_data += weightsize;

    if (copy_index)
      *output_data++ = real(indexes[i]);
  }

  if (n < num_neighbors)
    fill(output_data, output.end(), MISSING_VALUE);
}

} // end of namespace PLearn
