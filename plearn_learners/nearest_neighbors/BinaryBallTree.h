// -*- C++ -*-

// BinaryBallTree.h
//
// Copyright (C) 2004 Pascal Lamblin 
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
   * $Id: BinaryBallTree.h,v 1.1 2005/03/04 21:01:58 lamblin Exp $ 
   ******************************************************* */

// Authors: Pascal Lamblin

/*! \file BinaryBallTree.h */


#ifndef BinaryBallTree_INC
#define BinaryBallTree_INC

#include <plearn/base/Object.h>

namespace PLearn {
using namespace std;

class BinaryBallTree;
typedef PP<BinaryBallTree> BinBallTree;

class BinaryBallTree: public Object
{

private:
  
  typedef Object inherited;

protected:
  // *********************
  // * protected options *
  // *********************

  BinaryBallTree* parent;
  BinBallTree child1;
  BinBallTree child2;

public:

  // ************************
  // * public build options *
  // ************************

  Vec pivot;
  real radius;
  TVec<int> point_set;

  // ****************
  // * Constructors *
  // ****************

  //! Default constructor.
  BinaryBallTree();


  // ******************
  // * Object methods *
  // ******************

private: 
  //! This does the actual building. 
  void build_();

protected: 
  //! Declares this class' options.
  static void declareOptions(OptionList& ol);

public:
  // Declares other standard object methods.
  PLEARN_DECLARE_OBJECT(BinaryBallTree);

  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  virtual void setFirstChild( const BinBallTree& first_child );

  virtual void setSecondChild( const BinBallTree& second_child );

  virtual BinBallTree getFirstChild();

  virtual BinBallTree getSecondChild();

  virtual BinaryBallTree* getParent();

  bool isEmpty() const
  {
    bool result = !pivot && !child1 && !child2 ;
    return result;
  }

};

// Declares a few other classes and functions related to this class
  DECLARE_OBJECT_PTR(BinaryBallTree);
  
} // end of namespace PLearn

#endif
