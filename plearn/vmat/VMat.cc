// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio and University of Montreal
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
* $Id: VMat.cc,v 1.17 2004/06/18 16:49:31 tihocan Exp $ *
* This file is part of the PLearn library.               *
******************************************************** */
#include "VMat.h"
#include "fileutils.h"

#include "MemoryVMatrix.h"
#include "FileVMatrix.h"
#include "SelectRowsVMatrix.h"
#include "SelectColumnsVMatrix.h"
#include "SelectRowsFileIndexVMatrix.h"

namespace PLearn {
using namespace std;

/** VMat **/

VMat::VMat() {}
VMat::VMat(VMatrix* d): PP<VMatrix>(d) {}
VMat::VMat(const VMat& d) :PP<VMatrix>(d) {}
VMat::VMat(const Mat& datamat): PP<VMatrix>(new MemoryVMatrix(datamat)) {}
VMat::~VMat() {}

VMat VMat::subMatRows(int i, int l) const
{
  VMat res = ptr->subMat(i,0,l,width());
  res->defineSizes(ptr->inputsize(), ptr->targetsize(), ptr->weightsize());
  return res;
}

VMat VMat::rows(TVec<int> rows_indices) const
{ return new SelectRowsVMatrix(*this, rows_indices); }

VMat VMat::rows(Vec rows_indices) const
{ return new SelectRowsVMatrix(*this, rows_indices); }

VMat VMat::rows(const string& indexfile) const
{ return new SelectRowsFileIndexVMatrix(*this, indexfile); }

VMat VMat::columns(TVec<int> columns_indices) const
{ return new SelectColumnsVMatrix(*this, columns_indices); }

VMat VMat::columns(Vec columns_indices) const
{ return new SelectColumnsVMatrix(*this, columns_indices); }

////////////////
// precompute //
////////////////
void VMat::precompute() {
  VMat backup = *this;
  *this = new MemoryVMatrix(Mat(*this));
  (*this)->setFieldInfos( backup->getFieldInfos() );

  // We restore the sizes info (lost in the Mat conversion).
  // Note that there would probably be more info to restore (like
  // field infos, string mappings, ...).
  (*this)->copySizesFrom(backup);
  
  //TODO
  //(*this)->copyFieldInfosFrom(backup);
}
  
void VMat::precompute(const string& pmatfile, bool use_existing_file)
{ 
  VMat backup = *this;
  Array<VMField> infos = (*this)->getFieldInfos();
  if(!use_existing_file || !file_exists(pmatfile))
    save(pmatfile); 
  *this = new FileVMatrix(pmatfile); 
  (*this)->setFieldInfos( infos );
  (*this)->copySizesFrom(backup);
  // TODO same as above
}


template <>
void deepCopyField(VMat& field, CopiesMap& copies)
{
  if (field)
    field = static_cast<VMatrix*>(field->deepCopy(copies));
}

/////////////////////
// loadAsciiAsVMat //
/////////////////////
VMat loadAsciiAsVMat(const string& filename)
{
  Mat m;
  TVec<string> fn;
  TVec< map<string,real> > map_sr;  // String -> real mappings.
  loadAscii(filename, m, fn, &map_sr);
  VMat vm = new MemoryVMatrix(m);
  vm->setMtime(mtime(filename));
  // Set the discovered string -> real mappings.
  for (int i = 0; i < map_sr.length(); i++) {
    vm->setStringMapping(i, map_sr[i]);
  }
  for(int i=0;i<fn.size();i++)
    vm->declareField(i, fn[i]);
  return vm;
}

} // end of namespace PLearn
