// -*- C++ -*-

// KernelProjection.cc
//
// Copyright (C) 2004 Olivier Delalleau 
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
   * $Id: KernelProjection.cc,v 1.6 2004/05/07 19:10:01 tihocan Exp $ 
   ******************************************************* */

// Authors: Olivier Delalleau

/*! \file KernelProjection.cc */

#include "GaussianKernel.h"     //!< For the default kernel.
#include "KernelProjection.h"
#include "plapack.h"            //!< For eigenVecOfSymmMat.

namespace PLearn {
using namespace std;

//////////////////////
// KernelProjection //
//////////////////////
KernelProjection::KernelProjection() 
: n_comp_kept(-1),
  first_output(true),
  free_extra_components(true),
  min_eigenvalue(-REAL_MAX),
  n_comp(1),
  normalize(false)
  
{
}

PLEARN_IMPLEMENT_OBJECT(KernelProjection,
    "Performs dimensionality reduction by learning eigenfunctions of a kernel.", 
    ""
);

////////////////////
// declareOptions //
////////////////////
void KernelProjection::declareOptions(OptionList& ol)
{
  declareOption(ol, "kernel", &KernelProjection::kernel, OptionBase::buildoption,
      "The kernel used to compute the Gram matrix.");

  declareOption(ol, "n_comp", &KernelProjection::n_comp, OptionBase::buildoption,
      "Number of components computed.");

  declareOption(ol, "normalize", &KernelProjection::normalize, OptionBase::buildoption,
      "If set to 1, the resulting embedding will have variance 1 on each coordinate,\n"
      "otherwise the variance will be the corresponding eigenvalue.");

  declareOption(ol, "min_eigenvalue", &KernelProjection::min_eigenvalue, OptionBase::buildoption,
      "Any component associated with an eigenvalue <= min_eigenvalue will be discarded.");

  declareOption(ol, "free_extra_components", &KernelProjection::free_extra_components, OptionBase::buildoption,
      "If set to 1, components computed but not kept won't be available after training.");

  declareOption(ol, "eigenvalues", &KernelProjection::eigenvalues, OptionBase::learntoption,
      "The eigenvalues of the Gram matrix.");

  declareOption(ol, "eigenvectors", &KernelProjection::eigenvectors, OptionBase::learntoption,
      "The eigenvectors of the Gram matrix.");

  declareOption(ol, "n_comp_kept", &KernelProjection::n_comp_kept, OptionBase::learntoption,
      "The actual number of components actually kept in the output (we may discard\n"
      "some because of low eigenvalues).");

  declareOption(ol, "n_examples", &KernelProjection::n_examples, OptionBase::learntoption,
      "The number of points in the training set.");

  // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

///////////
// build //
///////////
void KernelProjection::build()
{
  inherited::build();
  build_();
}

////////////
// build_ //
////////////
void KernelProjection::build_()
{
  if (n_comp_kept == -1) {
    n_comp_kept = n_comp;
  }
  first_output = true;  // Safer.
}

/////////////////////////////
// computeCostsFromOutputs //
/////////////////////////////
void KernelProjection::computeCostsFromOutputs(const Vec& input, const Vec& output, 
                                           const Vec& target, Vec& costs) const
{
  // No cost to compute.
}                                

///////////////////
// computeOutput //
///////////////////
void KernelProjection::computeOutput(const Vec& input, Vec& output) const
{
  static Vec k_x_xi;
  static Mat result;
  static Mat used_eigenvectors;
  if (first_output) {
    // Initialize k_x_xi, used_eigenvectors and result correctly.
    k_x_xi.resize(n_examples);
    used_eigenvectors = eigenvectors.subMatRows(0, n_comp_kept);
    result.resize(n_comp_kept,1);
    first_output = false;
  }
  // Compute the K(x,x_i).
  kernel->evaluate_all_i_x(input, k_x_xi);
  // Compute the output.
  rowSum(used_eigenvectors * k_x_xi, result);
  output.resize(n_comp_kept);
  real* result_ptr = result[0];
  if (normalize) {
    for (int i = 0; i < n_comp_kept; i++) {
      output[i] = *(result_ptr++) / eigenvalues[i];
    }
  } else {
    for (int i = 0; i < n_comp_kept; i++) {
      output[i] = *(result_ptr++) / sqrt(eigenvalues[i]);
    }
  }
}    

////////////
// forget //
////////////
void KernelProjection::forget()
{
  stage = 0;
  n_comp_kept = n_comp;
  n_examples = 0;
  first_output = true;
  // Free memory.
  eigenvectors = Mat();
  eigenvalues = Vec();
}
    
//////////////////////
// getTestCostNames //
//////////////////////
TVec<string> KernelProjection::getTestCostNames() const
{
  // No cost to compute.
  TVec<string> t;
  return t;
}

///////////////////////
// getTrainCostNames //
///////////////////////
TVec<string> KernelProjection::getTrainCostNames() const
{
  return getTestCostNames();
}

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void KernelProjection::makeDeepCopyFromShallowCopy(map<const void*, void*>& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);

  // ### Call deepCopyField on all "pointer-like" fields 
  // ### that you wish to be deepCopied rather than 
  // ### shallow-copied.
  // ### ex:
  // deepCopyField(trainvec, copies);

  // ### Remove this line when you have fully implemented this method.
  PLERROR("KernelProjection::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}


////////////////
// outputsize //
////////////////
int KernelProjection::outputsize() const
{
  return n_comp_kept;
}

////////////////////
// setTrainingSet //
////////////////////
void KernelProjection::setTrainingSet(VMat training_set, bool call_forget) {
  inherited::setTrainingSet(training_set, call_forget);
  n_examples = training_set->length();
  // Save the dataset in the kernel, because it may be needed after we reload
  // the learner.
  kernel->specify_dataset = training_set;
  kernel->build();
}

///////////
// train //
///////////
void KernelProjection::train()
{
  if (stage == 1) {
    PLWARNING("In KernelProjection::train - Learner has already been trained");
    return;
  }
  Mat gram(n_examples,n_examples);
  // (1) Compute the Gram matrix.
  if (report_progress) {
    kernel->report_progress = true;
  }
  kernel->computeGramMatrix(gram);
  // (2) Compute its eigenvectors and eigenvalues.
  eigenVecOfSymmMat(gram, n_comp, eigenvalues, eigenvectors);
  // (3) Discard low eigenvalues.
  int p = 0;
  while (p < n_comp && eigenvalues[p] > min_eigenvalue)
    p++;
  n_comp_kept = p;
  // (4) Optionally remove the discarded components.
  if (free_extra_components) {
    eigenvalues.resize(n_comp_kept);
    eigenvectors.resize(n_comp_kept, eigenvectors.width());
  }
  // All done!
  first_output = true;
  stage = 1;
}

} // end of namespace PLearn
