// -*- C++ -*-

// VecDictionary.cc
//
// Copyright (C) 2004 Hugo Larochelle, Christopher Kermorvant
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
 * $Id$ 
 ******************************************************* */

// Authors: Hugo Larochelle, Christopher Kermorvant

/*! \file VecDictionary.cc */


#include "VecDictionary.h"

namespace PLearn {
using namespace std;
  
VecDictionary::VecDictionary():inherited(){}
  
VecDictionary::VecDictionary(TVec<string> symbols, bool up_mode)
{
    setUpdateMode(up_mode);
    vector_dict=symbols;
    build_();
}

PLEARN_IMPLEMENT_OBJECT(VecDictionary,
                        "Dictionary instantiation from a TVec<string>",
                        "This class implements a Dictionary instantiated from a TVec<string>.\n" 
                        "Each element of the TVec<string> is a symbol, and its index in the TVec<string> is\n"
                        "its id.\n");

void VecDictionary::declareOptions(OptionList& ol)
{
    declareOption(ol, "vector_dict", &VecDictionary::vector_dict, OptionBase::buildoption, "TVec<string> containing the symbols of the dictionary");
    inherited::declareOptions(ol);
}

void VecDictionary::build_()
{
    //initial building
    if(string_to_int.size()==0)
    {
        // save update mode for later
        int saved_up_mode=update_mode;
        // set the dictionary in update mode to insert the words
        update_mode =  UPDATE;
    
        for(int i=0; i<vector_dict.size(); i++){
            getId(vector_dict[i]);
        }
        if(saved_up_mode==NO_UPDATE){
            // the dictionary must contain oov
            getId(OOV_TAG);
        }
        // restore update mode;
        update_mode=saved_up_mode;
    }
}

// ### Nothing to add here, simply calls build_
void VecDictionary::build()
{
    inherited::build();
    build_();
}

void VecDictionary::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);
    deepCopyField(vector_dict, copies);
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
