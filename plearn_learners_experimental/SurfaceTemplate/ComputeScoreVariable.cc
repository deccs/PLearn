// -*- C++ -*-

// ComputeScoreVariable.cc
//
// Copyright (C) 2006 Pascal Lamblin 
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

// Authors: Pascal Lamblin

/*! \file ComputeScoreVariable.cc */


#include "ComputeScoreVariable.h"

namespace PLearn {
using namespace std;

/** ComputeScoreVariable **/

PLEARN_IMPLEMENT_OBJECT(
    ComputeScoreVariable,
    "ONE LINE USER DESCRIPTION",
    "MULTI LINE\nHELP FOR USERS"
    );

ComputeScoreVariable::ComputeScoreVariable() 
{
}

// constructors from input variables.
// NaryVariable constructor (inherited) takes a VarArray as argument.
// You can either construct from a VarArray (if the number of parent Var is not
// fixed, for instance), or construct a VarArray from Variables by operator &:
// input1 & input2 & input3. You can also do both, uncomment what you prefer.

ComputeScoreVariable::ComputeScoreVariable(Variable* input_index,
                                           Variable* geom_mean,
                                           Variable* geom_dev,
                                           Variable* feat_mean,
                                           Variable* feat_dev,
                                           Variable* weighting_params,
                                           string the_weighting_method,
                                           const PP<ICP>& the_icp_aligner,
                                           const TVec<Molecule>& the_molecules
                                          )
    : inherited(input_index & geom_mean & geom_sigma & feat_mean & feat_dev &
                weighting_params, 1, 1),
      weighting_method(the_weighting_method),
      icp_aligner(the_icp_aligner),
      p_molecules(&the_molecules)
{
    // ### You may (or not) want to call build_() to finish building the
    // ### object
    build_()
}

void ComputeScoreVariable::recomputeSizes(int& l, int& w) const
{
    // ### usual code to put here is:
    /*
        if (varray.size() > 0) {
            l = ... ; // the computed length of this Var
            w = ... ; // the computed width
        } else
            l = w = 0;
    */
    l = w = 1;
}

// ### computes value from varray values
void ComputeScoreVariable::fprop()
{
    // ### remove this line when implemented
    PLERROR("In ComputeScoreVariable - fprop() must be implemented.")
}

// ### computes varray gradients from gradient
void ComputeScoreVariable::bprop()
{
    // ### remove this line when implemented
    PLERROR("In ComputeScoreVariable - bprop() must be implemented.")
}

// ### You can implement these methods:
// void ComputeScoreVariable::bbprop() {}
// void ComputeScoreVariable::symbolicBprop() {}
// void ComputeScoreVariable::rfprop() {}


// ### Nothing to add here, simply calls build_
void ComputeScoreVariable::build()
{
    inherited::build();
    build_();
}

void ComputeScoreVariable::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    // ### Call deepCopyField on all "pointer-like" fields 
    // ### that you wish to be deepCopied rather than 
    // ### shallow-copied.
    // ### ex:
    // deepCopyField(trainvec, copies);

    // ### If you want to deepCopy a Var field:
    // varDeepCopyField(somevariable, copies);

    // ### Remove this line when you have fully implemented this method.
    PLERROR("ComputeScoreVariable::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}

void ComputeScoreVariable::declareOptions(OptionList& ol)
{
    // ### Declare all of this object's options here
    // ### For the "flags" of each option, you should typically specify  
    // ### one of OptionBase::buildoption, OptionBase::learntoption or 
    // ### OptionBase::tuningoption. Another possible flag to be combined with
    // ### is OptionBase::nosave

    // ### ex:
    // declareOption(ol, "myoption", &ComputeScoreVariable::myoption,
    //               OptionBase::buildoption,
    //               "Help text describing this option");
    // ...

    declareOption(ol, "weighting_method",
                  &ComputeScoreVariable::weighting_method,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "icp_aligner", &ComputeScoreVariable::icp_aligner,
                  OptionBase::buildoption,
                  "");

    // p_molecules is not an option, it has to be set in the constructor

    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void ComputeScoreVariable::build_()
{
    // ### This method should do the real building of the object,
    // ### according to set 'options', in *any* situation. 
    // ### Typical situations include:
    // ###  - Initial building of an object from a few user-specified options
    // ###  - Building of a "reloaded" object: i.e. from the complete set of
    // ###    all serialised options.
    // ###  - Updating or "re-building" of an object after a few "tuning"
    // ###    options have been modified.
    // ### You should assume that the parent class' build_() has already been
    // ### called.

    icp_aligner->weighting_method = weighting_method;
    // set icp_aligner's Mats' and Vecs' data to the values of the vars
}


} // end of namespace PLearn


/*
  Local Variables:
  mode:c++
  c-basic-offset:4
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0))
  indent-tabs-mode:nil
  fill-column:79
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=79 :
