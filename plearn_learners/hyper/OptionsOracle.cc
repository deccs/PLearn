
// -*- C++ -*-

// OptionsOracle.cc
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
   * $Id: OptionsOracle.cc,v 1.1 2005/01/11 23:22:44 plearner Exp $ 
   ******************************************************* */

/*! \file OptionsOracle.cc */
#include "OptionsOracle.h"

namespace PLearn {
using namespace std;

OptionsOracle::OptionsOracle() 
  {}

PLEARN_IMPLEMENT_ABSTRACT_OBJECT(OptionsOracle, "Generates various option combinations to try, to perform hyper-parameter optimisation.", 
      "OptionsOracle is the base class for implementing hyper-optimization techniques. \n"
      "An OptionGenerator functions as an oracle, responsible for giving the optimization process \n"
      "the next combination of hyper-parameters to try out, taking into account the results of all \n"
      "previous attempts. The HyperOptimize class refers to two OptionsOracle, the first one is responsible for \n"
      "generating potentially parallelizable hyper-parameter combinations to try out, the second \n"
      "helps to carry out an optimization that is sequential in nature (such as an early stopping for \n"
      "some form of incremental learning.) \n");

  void OptionsOracle::declareOptions(OptionList& ol)
  {
    inherited::declareOptions(ol);
  }

  void OptionsOracle::build_()
  {}

  // ### Nothing to add here, simply calls build_
  void OptionsOracle::build()
  {
    inherited::build();
    build_();
  }


  void OptionsOracle::makeDeepCopyFromShallowCopy(CopiesMap& copies)
  {
    inherited::makeDeepCopyFromShallowCopy(copies);
  }

} // end of namespace PLearn
