// -*- C++ -*-

// RowsSubVMatrix.cc
//
// Copyright (C) 2003 Pascal Vincent 
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
   * $Id: RowsSubVMatrix.cc,v 1.2 2004/02/20 21:14:44 chrish42 Exp $ 
   ******************************************************* */

// Authors: Pascal Vincent

/*! \file RowsSubVMatrix.cc */


#include "RowsSubVMatrix.h"

namespace PLearn {
using namespace std;


RowsSubVMatrix::RowsSubVMatrix()
  :startrow(0)
{}

RowsSubVMatrix::RowsSubVMatrix(VMat the_source, int the_startrow, int the_length)
  :SourceVMatrix(the_source), startrow(the_startrow)
{ 
  length_ = the_length;
  build_(); 
}


PLEARN_IMPLEMENT_OBJECT(RowsSubVMatrix, "ONE LINE DESCRIPTION", "MULTI-LINE \nHELP");

void RowsSubVMatrix::getRow(int i, Vec v) const
{
  source->getRow(startrow+i,v);
}

void RowsSubVMatrix::declareOptions(OptionList& ol)
{
  declareOption(ol, "startrow", &RowsSubVMatrix::startrow, OptionBase::buildoption,
                "The row where this submatrix starts in the source.\n"
                "You should also explicitly specify the length option.\n");

  // Now call the parent class' declareOptions
  inherited::declareOptions(ol);
}

void RowsSubVMatrix::build_()
{
  setMetaInfoFromSource();
}

// ### Nothing to add here, simply calls build_
void RowsSubVMatrix::build()
{
  inherited::build();
  build_();
}

void RowsSubVMatrix::makeDeepCopyFromShallowCopy(map<const void*, void*>& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);
}

} // end of namespace PLearn

