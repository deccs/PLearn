// -*- C++ -*-

// ManifoldParzen2.cc
// 
// Copyright (C) 2003 Pascal Vincent, Julien Keable
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


/*! \file ParzenWindow.cc */
#include "ParzenWindow.h"

#include <plearn/math/plapack.h>
#include <plearn/base/general.h>
#include <plearn/math/TMat.h>
#include <plearn/math/TMat_maths.h>
#include <plearn/math/BottomNI.h>

namespace PLearn {

PLEARN_IMPLEMENT_OBJECT(ParzenWindow, 
                        "Parzen Window density estimate ", 
                        "Standard Parzen Window algorithm. The user only needs\n"
                        "to set the sigma_square parameter"
);

/////////////////////
// ParzenWindow //
/////////////////////
ParzenWindow::ParzenWindow()
: sigma_square(1)
{
  nstages = 1;
}

ParzenWindow::ParzenWindow(real the_sigma_square)
  : sigma_square(the_sigma_square)
{
}

// ### Nothing to add here, simply calls build_
void ParzenWindow::build()
{
  inherited::build();
  build_();
}

// TODO Hide the options from GaussMix that are overwritten.

////////////////////
// declareOptions //
////////////////////
void ParzenWindow::declareOptions(OptionList& ol)
{
  declareOption(ol,"sigma_square", &ParzenWindow::sigma_square, OptionBase::buildoption,
                "Spherical variance parameter");

   // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

void ParzenWindow::build_()
{}

void ParzenWindow::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  PLearner::makeDeepCopyFromShallowCopy(copies);

  // ### Call deepCopyField on all "pointer-like" fields 
  // ### that you wish to be deepCopied rather than 
  // ### shallow-copied.
  // ### ex:
  // deepCopyField(trainvec, copies);

  // ### Remove this line when you have fully implemented this method.
  //PLERROR("ParzenWindow::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}

void ParzenWindow::train()
{
  Mat trainset(train_set);
  int l = train_set.length();
  
  type = "spherical";
  L = l;
  D = -1;
  GaussMix::build();
  resizeStuffBeforeTraining();
//  setMixtureTypeGeneral(l, ncomponents, w); // TODO Remove this line when it works.
  

  for(int i=0; i<l; i++)
  {
    if(i%100==0)
      cerr << "[SEQUENTIAL TRAIN: processing pattern #" << i << "/" << l << "]\n";
      
    // center is sample
    mu(i) << trainset(i);
    sigma[i] = sigma_square;

    alpha[i] = 1.0 / l;
    GaussMix::build();
    resizeStuffBeforeTraining();
  }
  stage = 1;
  precomputeStuff();
  build();
}

} // end of namespace PLearn
