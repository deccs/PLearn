

// -*- C++ -*-

// Distribution.h
// 
// Copyright (C) *YEAR* *AUTHOR(S)* 
// ...
// Copyright (C) *YEAR* *AUTHOR(S)* 
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
   * $Id: Distribution.h,v 1.8 2003/08/13 08:13:46 plearner Exp $ 
   ******************************************************* */

/*! \file Distribution.h */
#ifndef Distribution_INC
#define Distribution_INC

#include "Learner.h"

namespace PLearn <%
using namespace std;

class Distribution: public Learner
{
protected:
  // *********************
  // * protected options *
  // *********************

  // ### declare protected option fields (such as learnt parameters) here
  // ...
    
public:

  typedef Learner inherited;

  // ************************
  // * public build options *
  // ************************

  //! A string where the characters have the following meaning:
  //! 'l'->log_density, 'd' -> density, 'c' -> cdf, 's' -> survival_fn, 'e' -> expectation, 'v' -> variance
  string use_returns_what;

  // ****************
  // * Constructors *
  // ****************

  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  Distribution();


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
  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Provides a help message describing this class
  static string help();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  //! Declares name and deepCopy methods
  PLEARN_DECLARE_OBJECT(Distribution);

  // *******************
  // * Learner methods *
  // *******************

  //! trains the model
  virtual void train(VMat training_set); 

  //! computes the ouptu of a trained model
  virtual void use(const Vec& input, Vec& output);


  //! return log of probability density log(p(x))
  virtual double log_density(const Vec& x) const;

  //! return probability density p(x)
  //! [ default version returns exp(log_density(x)) ]
  virtual double density(const Vec& x) const;
  
  //! return survival fn = P(X>x)
  virtual double survival_fn(const Vec& x) const;

  //! return survival fn = P(X<x)
  virtual double cdf(const Vec& x) const;

  //! return E[X] 
  virtual Vec expectation() const;

  //! return Var[X]
  virtual Mat variance() const;

  //! return a pseudo-random sample generated from the distribution.
  virtual void generate(Vec& x) const;

};

// Declares a few other classes and functions related to this class
  DECLARE_OBJECT_PTR(Distribution);
  
%> // end of namespace PLearn

#endif
