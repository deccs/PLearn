// -*- C++ -*-

// PLearn ("A C++ Machine Learning Library")
// Copyright (C) 2002 Pascal Vincent and Julien Keable
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
   * $Id: VMatLanguage.h,v 1.18 2005/02/08 21:55:42 tihocan Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#ifndef VMatLanguage_INC
#define VMatLanguage_INC

#include "RowBufferedVMatrix.h"
#include "VMat.h"
#include <plearn/base/RealMapping.h>

#ifdef __INTEL_COMPILER
#pragma warning(disable:1125) // Get rid of ICC compiler warning.
#endif

namespace PLearn {
using namespace std;

/* The VMatLanguage object contains a VPL bytecode program that can be applied to a row of a VMat.
   
   The VPL Language is described in /u/jkeable/visu/help/vpl.html. 

   By the way, the define statements in the VPL language can be recursive. Cool!

*/

  class VMatLanguage: public Object
  {
    typedef Object inherited;

    VMat vmsource;
    TVec<int> program; 
    TVec<RealMapping> mappings;
    mutable Vec pstack;
    mutable Vec myvec;
    mutable Vec mem;

    // maps opcodes strings to opcodes numbers
    static map<string, int> opcodes;
    
    //! builds the opcodes map if it does not already exist
    static void build_opcodes_map();

    // generates bytecode from a preprocessed text sourcecode
    void generateCode(const string& processed_sourcecode);
    void generateCode(PStream& processed_sourcecode);

    // this function takes raw VPL code and returns the preprocessed sourcecode 
    // along with the defines and the fieldnames it generated
    void preprocess(PStream& in, map<string, string>& defines, string& processed_sourcecode,
                    vector<string>& fieldnames);
    
public:
    VMatLanguage():vmsource(Mat()) { build_(); }
    VMatLanguage(VMat vmsrc):vmsource(vmsrc) { build_(); }

    PLEARN_DECLARE_OBJECT(VMatLanguage);
    static void declareOptions(OptionList &ol);

    virtual void build();

    //! Executes the program on the srcvec, copy resulting stack to result
    //! rowindex is only there for instruction 'rowindex' that pushes it on the stack
    virtual void run(const Vec& srcvec, const Vec& result, int rowindex=-1) const;

    //! Gets the row with the given rowindex from the vmsource VMat
    //! and applies program to it.
    void run(int rowindex, const Vec& result) const;

    inline void setSource(VMat the_source) 
    { 
      vmsource = the_source;
      // program must be compiled with the right source in place...
      program.resize(0);
    }

    // from the outside, use the next 3 high-level functions
    /////////////////////////////////////////////////////////

    //! takes a string, filename, or PStream and generate the bytecode from it
    //! On exit, fieldnames will contain fieldnames of the resulting matrix
    //! The source vmat must be set before compiling a VPL program. 
    void compileStream(PStream &in, vector<string>& fieldnames);
    void compileString(const string & code, vector<string>& fieldnames);
    void compileFile(const PPath& filename, vector<string>& fieldnames);
    
    int pstackSize() const {return pstack.size();}

    // set this to true when debugging. Will dump the preprocessed code when you call compilexxxx
    static bool output_preproc;
  private:
      void build_();
  };

DECLARE_OBJECT_PTR(VMatLanguage);

/*
  PreprocessingVMatrix : a VMatLanguage derivated product, a la sauce VMat.
  Construct a PreprocessingVMatrix by specifying the source VMat and a string containing the VPL code to process each row
*/

  class PreprocessingVMatrix: public RowBufferedVMatrix
  {
    typedef RowBufferedVMatrix inherited;

  protected:
    VMat source;
    VMatLanguage program;
    Vec sourcevec;
    vector<string> fieldnames;    

  public:
    PreprocessingVMatrix(){}
    PreprocessingVMatrix(VMat the_source, const string& program_string);

    PLEARN_DECLARE_OBJECT(PreprocessingVMatrix);

    virtual void build();

  protected:

    virtual void getNewRow(int i, const Vec& v) const;
    static void declareOptions(OptionList &ol);

  private:

      void build_();
  };

  DECLARE_OBJECT_PTR(PreprocessingVMatrix);

  time_t getDateOfCode(const string& codefile);

} // end of namespace PLearn

#ifdef __INTEL_COMPILER
#pragma warning(default:1125)
#endif

#endif


