// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2001 Pascal Vincent, Yoshua Bengio, Rejean Ducharme and University of Montreal
// Copyright (C) 2002 Pascal Vincent, Julien Keable, Xavier Saint-Mleux
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
   * $Id: MemoryVMatrix.cc,v 1.27 2004/11/26 16:57:23 tihocan Exp $
   ******************************************************* */

#include "MemoryVMatrix.h"

namespace PLearn {
using namespace std;



/** MemoryVMatrix **/

PLEARN_IMPLEMENT_OBJECT(MemoryVMatrix,
    "A VMatrix whose data is stored in memory.",
    "The data can either be given directly by a Mat, or by another VMat that\n"
    "will be precomputed in memory at build time.\n"
);

MemoryVMatrix::MemoryVMatrix()
: synch_data(true),
  data(Mat())
{
  memory_data = data;
}

MemoryVMatrix::MemoryVMatrix(int l, int w)
: VMatrix(l, w),
  synch_data(false)
{
  data.resize(l,w);
  memory_data = data;
  defineSizes(data.width(), 0, 0);
}

MemoryVMatrix::MemoryVMatrix(const Mat& the_data)
: VMatrix(the_data.length(), the_data.width()),
  synch_data(true),
  data(the_data)
{
  memory_data = the_data;
  defineSizes(the_data.width(), 0, 0);
}

MemoryVMatrix::MemoryVMatrix(VMat the_data_vm)
: VMatrix(the_data_vm->length(), the_data_vm->width()),
  memory_data(the_data_vm->toMat()),
  synch_data(false)
{
  copySizesFrom(the_data_vm);
  setMetaInfoFrom(the_data_vm);
}

////////////////////
// declareOptions //
////////////////////
void MemoryVMatrix::declareOptions(OptionList& ol)
{
  declareOption(ol, "data", &MemoryVMatrix::data, OptionBase::buildoption,
      "The underlying Mat.");

  declareOption(ol, "data_vm", &MemoryVMatrix::data_vm, OptionBase::buildoption,
      "The underlying VMatrix. Will overwrite 'data' if provided.");

  inherited::declareOptions(ol);
}

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void MemoryVMatrix::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);
  deepCopyField(memory_data, copies);
  deepCopyField(data, copies);
  deepCopyField(data_vm, copies);
}

////////////
// build_ //
////////////
void MemoryVMatrix::build_()
{
  if (data_vm) {
    // Precompute data from data_vm;
    memory_data = data_vm->toMat();
    copySizesFrom(data_vm);
    setMetaInfoFrom(data_vm);
    synch_data = false;
  } else {
    synch_data = true;
  }
  if (synch_data) {
    memory_data = data;
    // We temporarily set data to a new empty Mat, so that memory_data
    // can be safely resized.
    data = Mat();
  }
  if (this->length() >= 0 && this->length() != memory_data.length()) {
    // New length specified.
    memory_data.resize(this->length(), memory_data.width());
  }
  if (this->width() >= 0 && this->width() != memory_data.width()) {
    // New width specified.
    memory_data.resize(memory_data.length(), this->width());
  }
  if (this->length() < 0 && memory_data.length() >= 0) {
    // Take the length from the data matrix.
    this->length_ = memory_data.length();
  }
  if (this->width() < 0 && memory_data.width() >= 0) {
    // Take the width from the data matrix.
    this->width_ = memory_data.width();
  }
  if (synch_data)
    // Restore data so that it is equal to memory_data.
    data = memory_data;
}

///////////
// build //
///////////
void MemoryVMatrix::build()
{
  inherited::build();
  build_();
}

/////////
// get //
/////////
real MemoryVMatrix::get(int i, int j) const
{ return memory_data(i,j); }

void MemoryVMatrix::put(int i, int j, real value)
{ memory_data(i,j) = value; }

void MemoryVMatrix::getColumn(int i, Vec v) const
{ v << memory_data.column(i); }

void MemoryVMatrix::getSubRow(int i, int j, Vec v) const
{
#ifdef BOUNDCHECK
  if (j+v.length()>width())
    PLERROR("MemoryVMatrix::getSubRow(int i, int j, Vec v) OUT OF BOUNDS. "
            "j=%d, v.length()=%d, width()=%d", j, v.length(), width());
#endif
  if (v.length() > 0)
    v.copyFrom(memory_data[i]+j, v.length());
}

////////////
// getRow //
////////////
void MemoryVMatrix::getRow(int i, Vec v) const
{
  if (v.length() > 0)
    v.copyFrom(memory_data[i], width_);
}

////////////
// getMat //
////////////
void MemoryVMatrix::getMat(int i, int j, Mat m) const
{ m << memory_data.subMat(i,j,m.length(),m.width()); }

///////////////
// putSubRow //
///////////////
void MemoryVMatrix::putSubRow(int i, int j, Vec v)
{
#ifdef BOUNDCHECK
  if (j+v.length()>width())
    PLERROR("MemoryVMatrix::putSubRow(int i, int j, Vec v) OUT OF BOUNDS. "
            "j=%d, v.length()=%d, width()=%d", j, v.length(), width());
#endif
  if (v.length() > 0)
    v.copyTo(memory_data[i]+j);
}

//////////
// fill //
//////////
void MemoryVMatrix::fill(real value)
{ memory_data.fill(value); }


////////////
// putRow //
////////////
void MemoryVMatrix::putRow(int i, Vec v)
{
  if (v.length() > 0)
    v.copyTo(memory_data[i]);
}

////////////
// putMat //
////////////
void MemoryVMatrix::putMat(int i, int j, Mat m)
{ memory_data.subMat(i,j,m.length(),m.width()) << m; }

///////////////
// appendRow //
///////////////
void MemoryVMatrix::appendRow(Vec v)
{ 
  memory_data.appendRow(v); 
  length_++;
}

///////////
// toMat //
///////////
Mat MemoryVMatrix::toMat() const
{ return memory_data; }

////////////
// subMat //
////////////
VMat MemoryVMatrix::subMat(int i, int j, int l, int w)
{ return new MemoryVMatrix(memory_data.subMat(i,j,l,w)); }

/////////
// dot //
/////////
real MemoryVMatrix::dot(int i1, int i2, int inputsize) const
{
#ifdef BOUNDCHECK
  if(inputsize>width())
    PLERROR("In MemoryVMatrix::dot inputsize>width()");
#endif
  real* v1 = memory_data.rowdata(i1);
  real* v2 = memory_data.rowdata(i2);
  real res = 0.;
  for(int k=0; k<inputsize; k++)
    res += (*v1++) * (*v2++);
  return res;
}

real MemoryVMatrix::dot(int i, const Vec& v) const
{
#ifdef BOUNDCHECK
  if(v.length()>width())
    PLERROR("In MemoryVMatrix::dot length of vector v is greater than VMat's width");
#endif
  if (v.length() > 0) {
    real* v1 = memory_data.rowdata(i);
    real* v2 = v.data();
    real res = 0.;
    for(int k=0; k<v.length(); k++)
      res += v1[k]*v2[k];
    return res;
  }
  return 0.0;                                // in the case of a null vector
}

} // end of namespcae PLearn
