// -*- C++ -*-

// lexical_cast.h
//
// Copyright (C) 2005 Christian Dorion 
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
   * $Id: lexical_cast.h,v 1.1 2005/01/20 20:07:30 dorionc Exp $ 
   ******************************************************* */

// Authors: Christian Dorion

/*! \file lexical_cast.h */


#ifndef lexical_cast_INC
#define lexical_cast_INC

// Put includes here
#include <string>

namespace PLearn {
using namespace std;

// this function handle numbers with exponents (such as 10.2E09)
// as well as Nans. String can have trailing whitespaces on both sides
bool pl_isnumber(const string& s,double* dbl=NULL);
bool pl_isnumber(const string& s,float* dbl);

//!  conversions from string to numerical types
long     tolong    (const string& s, int base=10);  
double   todouble  (const string& s);
bool     tobool    (const string& s);


  inline float  tofloat   (const string& s)
  {
    return float(todouble(s));
  }
  
  inline int    toint     (const string& s, int base=10)
  {
    return int(tolong(s,base));
  }

#if    defined(USEFLOAT)
  inline float  toreal    (const string& s)
  {
    return tofloat(s);
  }
  
#elif  defined(USEDOUBLE)
  inline double toreal    (const string& s)
  {
    return todouble(s);
  }

#endif 


} // end of namespace PLearn

#endif
