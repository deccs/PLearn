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
   * $Id: Range.h,v 1.1 2002/07/30 09:01:26 plearner Exp $
   * AUTHORS: Pascal Vincent & Yoshua Bengio
   * This file is part of the PLearn library.
   ******************************************************* */

// See Mat.h for a description the PLearn native binary file format for matrices and vectors (.pmat .pvec)


/*! \file PLearnLibrary/PLearnCore/Range.h */

#ifndef Range_INC
#define Range_INC

namespace PLearn <%
using namespace std;

  class Range
  {
    public:
      int start; //!<  index of first element
      int length; //!<  number of elements
      
      Range(int the_start=0, int the_length=0)
        :start(the_start), length(the_length) {}
      
      bool isEmpty() { return length<=0; }

      bool operator==(Range r) const
      { return start==r.start && length==r.length; }

      bool operator<(Range r) const
      { return start<r.start || (start==r.start && length<r.length); }

      //!  to allow if(range) statements (safer than operator bool, according to Nicolas.)
      operator void*() { return (length>0 ? this : 0); }
  };
  
  inline ostream& operator<<(ostream& out, Range r)
  { 
    out << "[" << r.start << "," << r.start+r.length-1 << "]"; 
    return out;
  }

%> // end of namespace PLearn

#endif
