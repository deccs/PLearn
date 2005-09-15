// -*- C++ -*-

// Dictionary.h
//
// Copyright (C) 2004 Hugo Larochelle Christopher Kermorvant
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

/*! \file Dictionary.h */


#ifndef Dictionary_INC
#define Dictionary_INC
#include <plearn/base/stringutils.h>
#include <plearn/base/Object.h>
#include <map>
#include <string>

#define NO_UPDATE 0
#define UPDATE 1
// Default mode for the dicionary
#define DEFAULT_UPDATE 0

#define OOV_TAG "<oov>"

namespace PLearn {
using namespace std;

/*! A dictionary is a mapping between a string and and index (int).
  Depending on the update mode, the dictionay can include an unknown 
  word when asking for its Id with getId();
  if update_mode == UPDATE, add the word and return its Id
  if  update_mode == NO_UPDATE, return -1
  
  Also, when asking for the symbol associated to an Id, no update is possible.
  If the id is not in the dictionary, than the symbol is ""

  An object from Dictionary is instantiated empty, and then symbols can be added in the
  dictionary by using getId() (with update_mode == UPDATE). Subclasses will normaly 
  permit more sophisticated instantiation.
*/

class Dictionary: public Object
{

private:
  
    typedef Object inherited;

protected:
    // *********************
    // * protected options *
    // *********************

    //! string to int mapping
    map<string,int> string_to_int;
    //! int to string mapping
    map<int,string> int_to_string;

public:

    // ************************
    // * public build options *
    // ************************
    //! update mode update/no update 
    int update_mode;
  
    // ****************
    // * Constructors *
    // ****************

    //! Default constructor.
    Dictionary();

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

    PLEARN_DECLARE_OBJECT(Dictionary);

    //! Set update dictionary mode : UPDATE/NO_UPDATE.
    virtual void  setUpdateMode(int up_mode);

    //! Gives the id of a symbol in the dictionary
    //! If the symbol is not in the dictionary, 
    //! returns the index of oov if update_mode = NO_UPDATE.
    //! Insert the new word otherwise and return its index
    //! When a symbol is added to the dictionary, the following fields
    //! are updated: string_to_int, int_to_string, values
    //! Options can be specified ...
    virtual  int getId(string symbol, TVec<string> options = TVec<string>(0));

    //! Const version. Do not insert unknown words
    //! Options can be specified ...
    virtual  int getId(string symbol, TVec<string> options = TVec<string>(0))const;
  
    //! Gives the symbol from an id of the dictionary
    //! If the id is invalid, the string returned is ""
    //! Options can be specified ...
    virtual  string getSymbol(int id, TVec<int> options = TVec<int>(0))const;
  
    //! Get dimension of the dictionary (number of differents values in the dictionary)
    //! Options can be specified to restrict the number of possible values. 
    virtual int getDimension(TVec<int> options=TVec<int>(0)){return string_to_int.size();}

    //! Returns a Vec containing every possible id values of the Dictionary
    //! Options can be specified to restrict the number of possible values. 
    //! Here, values is simply copied (which can be costly!), and then the copy is returned
    virtual Vec getValues(TVec<int> options=TVec<int>(0))
    { 
        Vec ret(string_to_int.size());
        int i=0;
        for(map<string,int>::iterator it = string_to_int.begin(); it != string_to_int.end(); it++)
            ret[i++] = it->second;
        return ret;
    }

    //! Indicates if a symbol is in the dictionary
    virtual bool isIn(string symbol){return string_to_int.find(symbol) != string_to_int.end();};
  
    //! Indicates if an id is in the dictionary
    virtual bool isIn(int id){return int_to_string.find(id) != int_to_string.end();};

    //! Get Dictionary size
    virtual int size(){return int_to_string.size();};
    
    //! Indicates if Dictionary is empty
    //! Note that a Dictionary is considered empty is
    //! its size is 0 or if it only contains the OOV_TAG symbol
    virtual bool isEmpty(){return size()==0 || (size()==1 && isIn(OOV_TAG));};

    //! Clear or reinitialize the Dictionary
    virtual void clear(){string_to_int.clear(); int_to_string.clear();};

    // simply calls inherited::build() then build_() 
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(Dictionary);
  
} // end of namespace PLearn

#endif


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
