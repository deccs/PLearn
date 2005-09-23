// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2001 Pascal Vincent, Yoshua Bengio, Rejean Ducharme and University of Montreal
// Copyright (C) 2002 Pascal Vincent, Julien Keable, Xavier Saint-Mleux
// Copyright (C) 2003 Olivier Delalleau
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

#include "SelectColumnsVMatrix.h"

namespace PLearn {
using namespace std;

/** SelectColumnsVMatrix **/

PLEARN_IMPLEMENT_OBJECT(SelectColumnsVMatrix,
                        "Selects variables from a source matrix according to given vector of indices.",
                        "Alternatively, the variables can be given by their names. In this case, if a\n"
                        "field does not exist and the 'extend_with_missing' option is set to 'true',\n"
                        "the field will be added and filled with missing values.\n"
    );

//////////////////////////
// SelectColumnsVMatrix //
//////////////////////////
SelectColumnsVMatrix::SelectColumnsVMatrix()
    : extend_with_missing(false),
      fields_partial_match(false)
{}

SelectColumnsVMatrix::SelectColumnsVMatrix(VMat the_source, TVec<string> the_fields, bool the_extend_with_missing)
    : extend_with_missing(the_extend_with_missing),
      fields(the_fields),
      fields_partial_match(false)
{
    source = the_source;
    build_();
}

SelectColumnsVMatrix::SelectColumnsVMatrix(VMat the_source, TVec<int> the_indices)
    : extend_with_missing(false),
      indices(the_indices),
      fields_partial_match(false)
{
    source = the_source;
    build_();
}

SelectColumnsVMatrix::SelectColumnsVMatrix(VMat the_source, Vec the_indices)
    : extend_with_missing(false),
      fields_partial_match(false)
{
    source = the_source;
    indices.resize(the_indices.length());
    // copy the real the_indices into the integer indices
    indices << the_indices;
    build_();
}

/////////
// get //
/////////
real SelectColumnsVMatrix::get(int i, int j) const {
    static int col;
    col = indices[j];
    if (col == -1)
        return MISSING_VALUE;
    return source->get(i, col);
}

///////////////
// getSubRow //
///////////////
void SelectColumnsVMatrix::getSubRow(int i, int j, Vec v) const
{
    static int col;
    for(int jj=0; jj<v.length(); jj++) {
        col = indices[j+jj];
        if (col == -1)
            v[jj] = MISSING_VALUE;
        else
            v[jj] = source->get(i, col); 
    }
}

////////////////////
// declareOptions //
////////////////////
void SelectColumnsVMatrix::declareOptions(OptionList &ol)
{
    declareOption(ol, "fields", &SelectColumnsVMatrix::fields, OptionBase::buildoption,
                  "The names of the fields to extract (will override 'indices' if provided). Can\n"
                  "be a range of the form Field_1-Field_n, if 'extend_with_missing' is false.");

    declareOption(ol, "fields_partial_match", &SelectColumnsVMatrix::fields_partial_match, OptionBase::buildoption,
                  "If set to 1, then a field will be kept iff it contains one of the strings from 'fields'.");

    declareOption(ol, "indices", &SelectColumnsVMatrix::indices, OptionBase::buildoption,
                  "The array of column indices to extract.");

    declareOption(ol, "extend_with_missing", &SelectColumnsVMatrix::extend_with_missing, OptionBase::buildoption,
                  "If set to 1, then fields specified in the 'fields' option that do not exist\n"
                  "in the source VMatrix will be filled with missing values.");

    inherited::declareOptions(ol);
}

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void SelectColumnsVMatrix::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);
    deepCopyField(indices, copies);
    deepCopyField(fields, copies);
}

///////////
// build //
///////////
void SelectColumnsVMatrix::build()
{
    inherited::build();
    build_();
}

////////////
// build_ //
////////////
void SelectColumnsVMatrix::build_()
{
    if (source) {
        if (fields.isNotEmpty()) {
            // Find out the indices from the fields.
            indices.resize(0);
            if (!fields_partial_match) {
                for (int i = 0; i < fields.length(); i++) {
                    string the_field = fields[i];
                    int the_index = source->fieldIndex(the_field);  // string only
                    if (!extend_with_missing && the_index == -1) {
                        // The_field does not exist AS A STRING in the vmat
                        // It may be of the form FIELD1-FIELDN (a range of fields).
                        size_t pos = the_field.find('-');
                        bool ok = false;
                        if (pos != string::npos) {
                            string field1 = the_field.substr(0, pos);
                            string fieldn = the_field.substr(pos + 1);
                            int the_index1 = source->getFieldIndex(field1);  // either string or number
                            int the_indexn = source->getFieldIndex(fieldn);  // either string or number
                            if (the_index1 >= 0 && the_indexn > the_index1) {
                                // Indeed, this is a range.
                                ok = true;
                                for (int j = the_index1; j <= the_indexn; j++)
                                    indices.append(j);
                            }
                        }
                        // OR it may be a number by itself only
                        else if ((the_index = source->getFieldIndex(the_field)) != -1) {
                            ok = true;
                            indices.append(the_index);
                        }
                        if (!ok)
                            PLERROR("In SelectColumnsVMatrix::build_ - Unknown field (%s) in source VMat "
                                    "(you may want to use the 'extend_with_missing' option)", the_field.c_str());
                    } else
                        indices.append(the_index);
                }
            } else {
                // We need to check whether or not we should add each field.
                TVec<string> source_fields = source->fieldNames();
                for (int i = 0; i < source_fields.length(); i++)
                    for (int j = 0; j < fields.length(); j++)
                        if (source_fields[i].find(fields[j]) != string::npos) {
                            // We want to add this field.
                            indices.append(i);
                            break;
                        }
            }
        }

        // Copy matrix dimensions
        width_ = indices.length();
        length_ = source->length();

        // Give warning if inputsize, and friends are not specified, since we
        // can't just copy the values from the underlying VMat. A smarter
        // version of this class could Figure out the right size for inputsize,
        // targetsize and weightsize, at least for some cases (for example when
        // the indices are nondecreasing).
        if (inputsize_ < 0 || targetsize_ < 0 || weightsize_ < 0) 
            PLWARNING("In SelectColumnsVMatrix::build_ inputsize, targetsize or weightsize "
                      "not set. You may want to set them yourself in the .plearn file.");
        
        // Copy the appropriate VMFields
        fieldinfos.resize(width());
        if (source->getFieldInfos().size() > 0) {
            for (int i=0; i<width(); ++i) {
                int col = indices[i];
                if (col == -1) {
                    if (extend_with_missing)
                        // This must be because it is a field that did not exist in
                        // the source VMat.
                        fieldinfos[i] = VMField(fields[i]);
                    else
                        PLERROR("In SelectColumnsVMatrix::build_ - '-1' is not a valid value in indices");
                } else {
                    fieldinfos[i] = source->getFieldInfos(col);
                }
            }
        }
    }
}

////////////////////////////
// getStringToRealMapping //
////////////////////////////
const map<string,real>& SelectColumnsVMatrix::getStringToRealMapping(int col) const {
    static int the_col;
    static map<string, real> empty_mapping;
    the_col = indices[col];
    if (the_col == -1)
        return empty_mapping;
    return source->getStringToRealMapping(the_col);
}

//////////////////
// getStringVal //
//////////////////
real SelectColumnsVMatrix::getStringVal(int col, const string & str) const {
    static int the_col;
    the_col = indices[col];
    if (the_col == -1)
        return MISSING_VALUE;
    return source->getStringVal(the_col, str);
}

////////////////////////////
// getRealToStringMapping //
////////////////////////////
const map<real,string>& SelectColumnsVMatrix::getRealToStringMapping(int col) const {
    static int the_col;
    static map<real, string> empty_mapping;
    the_col = indices[col];
    if (the_col == -1)
        return empty_mapping;
    return source->getRealToStringMapping(the_col);
}

//////////////////
// getValString //
//////////////////
string SelectColumnsVMatrix::getValString(int col, real val) const {
    static int the_col;
    the_col = indices[col];
    if (the_col == -1)
        return "";
    return source->getValString(the_col, val);
}

} // end of namespcae PLearn


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
