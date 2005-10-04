// -*- C++ -*-

// ProcessSymbolicSequenceVMatrix.cc
//
// Copyright (C) 2004 Hugo Larochelle 
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

// Authors: Hugo Larochelle

/*! \file ProcessSymbolicSequenceVMatrix.cc */


#include "ProcessSymbolicSequenceVMatrix.h"
#include <plearn/base/tostring.h>

namespace PLearn {
using namespace std;


ProcessSymbolicSequenceVMatrix::ProcessSymbolicSequenceVMatrix()
    :inherited(), n_left_context(2), n_right_context(2), conditions_offset(0), conditions_for_exclusion(1), full_context(true),
     put_only_target_attributes(false), use_last_context(true)
    /* ### Initialise all fields to their default value */
{
}

ProcessSymbolicSequenceVMatrix::ProcessSymbolicSequenceVMatrix(VMat s, int l_context,int r_context)
    :inherited(),conditions_offset(0), conditions_for_exclusion(1), full_context(true),
     use_last_context(true)
    /* ### Initialise all fields to their default value */
{
    source = s;
    n_left_context= l_context;
    n_right_context = r_context;
    build();
}
  
PLEARN_IMPLEMENT_OBJECT(ProcessSymbolicSequenceVMatrix,
                        "This VMatrix takes a VMat of a sequence of symbolic elements (corresponding to a set of symbolic attributes) and constructs context rows.",
                        "An example of a sequence of elements would be a sequence of words, with their lemma form and POS tag\n"
                        "This sequence is encoded using integers, and is represented by the source VMatrix such as each\n"
                        "row is an element, and each column is a certain type of attribute (e.g. lemma, POS tag, etc.)."
                        "The source VMat string mapping (functions getStringVal(...) and getValString(...)) contains.\n"
                        "the integer/string encoding of the symbolic data."
                        "The context rows can be of fixed length, or constrained by delimiter symbols.\n"                        
                        "Certain rows can be selected/excluded, and certain elements can be excluded of\n"
                        "a context according to some conditions on its attributes.\n"
                        "The conditions are expressed as disjunctions of conjunctions. For example: \n\n"
                        "[ [ 0 : \"person\", 1 : \"NN\" ] , [ 0 : \"kind\", 2 : \"plural\" ] ] \n\n"
                        "is equivalent in C++ logic form to : \n\n"
                        "(fields[0]==\"person\" && fields[1]==\"NN\") || (fields[0]==\"kind\" && fields[2]==\"plural\").\n\n"
                        "Conditions can be expressed in string or int format. The integer/string mapping is used to make the correspondance.\n"
                        "We call the 'target element' of a context the element around which other elements are collected to construct the context.\n"
    );
  
void ProcessSymbolicSequenceVMatrix::getNewRow(int i, const Vec& v) const
{
    if(i<0 || i>=length_) PLERROR("In SelectAttributeSequenceVMatrix::getNewRow() : invalid row acces i=%d for VMatrix(%d,%d)",i,length_,width_);
  
    int target = indices[i];

  
    // If the target element is a delimiter, do nothing: Hugo: may not be a good thing!
    source->getRow(target,target_element);
    /*
      if(is_true(delimiters,target_element))
      {
      v.fill(MISSING_VALUE);
      if(use_last_context)
      {
      current_context_pos.clear();
      current_row_i.fill(MISSING_VALUE);
      current_target_pos = -1;
      lower_bound = 1;
      upper_bound = -1;
      }
      return;
      }
    */

    int left_added_elements = 0;
    int right_added_elements = 0;

    int left_pos = 1;
    int right_pos = 1;
    int p = 0;

    bool to_add = false;

    int this_lower_bound = target;
    int this_upper_bound = target;
  
    // Left context construction

    while(n_left_context < 0 || left_added_elements < n_left_context)
    {
        to_add = false;
    

        p = target-left_pos;

        if(p < 0 || p >= source->length())
            break;

        // Verifying if context can be found in last context
        if(use_last_context && lower_bound <= p && upper_bound >= p)
        {
            if(current_context_pos.find(p) != current_context_pos.end())
            {
                int position = current_context_pos[p];
                element = current_row_i.subVec(n_attributes*position,n_attributes);
                if(!is_true(ignored_context,element))
                {
                    to_add = true;
                }
            }
            else
            {
                left_pos++;
                continue;
            }
        }  // If not, fetch element in source VMat
        else
        {
            source->getRow(p,element);
            if(!is_true(ignored_context,element))
                to_add = true;  
        }
    
        if(is_true(delimiters,element)) break;

        if(to_add)
        {
            left_context.subVec(n_attributes*left_added_elements,n_attributes) << element;
            if(use_last_context) left_positions[left_added_elements] = p;
            this_lower_bound = p;
            left_added_elements++;
        }
        else
            if(!full_context)
            {
                left_context.subVec(n_attributes*left_added_elements,n_attributes).fill(MISSING_VALUE);
                if(use_last_context) left_positions[left_added_elements] = p;
                this_lower_bound = p;
                left_added_elements++;
            }
    
        left_pos++;
    }

    // Right context construction

    while(n_right_context < 0 || right_added_elements < n_right_context)
    {
        to_add = false;

        p = target+right_pos;

        if(p < 0 || p >= source->length())
            break;

        // Verifying if context can be found in last context
        if(use_last_context && lower_bound <= p && upper_bound >= p)
        {
            if(current_context_pos.find(p) != current_context_pos.end())
            {
                int position = current_context_pos[p];
                element = current_row_i.subVec(n_attributes*position,n_attributes);
                if(!is_true(ignored_context,element))
                {
                    to_add = true;
                }
            }
            else
            {
                right_pos++;
                continue;
            }
        }  // If not, fetch element in source VMat
        else
        {
            source->getRow(p,element);
            if(!is_true(ignored_context,element))
                to_add = true;  
        }
    
        if(is_true(delimiters,element)) break;

        if(to_add)
        {
            right_context.subVec(n_attributes*right_added_elements,n_attributes) << element;
            if(use_last_context) right_positions[right_added_elements] = p;
            this_upper_bound = p;
            right_added_elements++;
        }
        else
            if(!full_context)
            {
                right_context.subVec(n_attributes*right_added_elements,n_attributes).fill(MISSING_VALUE);
                if(use_last_context) right_positions[right_added_elements] = p;
                this_upper_bound = p;
                right_added_elements++;
            }
    
        right_pos++;
    }

    current_context_pos.clear();
    current_target_pos = -1;
    lower_bound = this_lower_bound;
    upper_bound = this_upper_bound;
    //current_row_i.fill(MISSING_VALUES);

    // Constructing complete row

    int cp = 0;

    if(fixed_context)  // Adding missing value, for the case where the context is of fixed length
        if(n_left_context-left_added_elements>0)
        {
            current_row_i.subVec(cp*n_attributes,n_attributes*(n_left_context-left_added_elements)).fill(MISSING_VALUE);
            cp = n_left_context-left_added_elements;
        }

    // adding left context

    int temp = left_added_elements;

    while(temp > 0)
    {
        temp--;
        current_row_i.subVec(cp*n_attributes,n_attributes) << left_context.subVec(temp*n_attributes,n_attributes);
        if(use_last_context) current_context_pos[(int)left_positions[temp]] = cp;
        cp++;
    }

    // adding target element
    int target_position = cp;
    current_row_i.subVec(cp*n_attributes,n_attributes) << target_element;
    if(use_last_context) 
    {
        current_context_pos[target] = cp;
        current_target_pos = cp;
    }
    cp++;

    // adding right context

    int r=0;
    while(r<right_added_elements)
    {
        current_row_i.subVec(cp*n_attributes,n_attributes) << right_context.subVec(r*n_attributes,n_attributes);
        if(use_last_context) current_context_pos[(int)right_positions[r]] = cp;
        r++;
        cp++;
    }

    if(current_row_i.length()-cp*n_attributes > 0)
        current_row_i.subVec(cp*n_attributes,current_row_i.length()-cp*n_attributes).fill(MISSING_VALUE);

    // Seperating input and output fields

    for(int t=0; t<current_row_i.length(); t++)
    {
        if(t%n_attributes < source->inputsize())
            v[t/n_attributes * source->inputsize() + t%n_attributes] = current_row_i[t];
        else
            if(put_only_target_attributes)
            {
                if(t/n_attributes == target_position)
                    v[inputsize_ + t%n_attributes - source->inputsize() ] = current_row_i[t];
            }
            else
                v[inputsize_ + t/n_attributes * source->targetsize() + t%n_attributes - source->inputsize() ] = current_row_i[t];
    }

    return;
}

void ProcessSymbolicSequenceVMatrix::declareOptions(OptionList& ol)
{
    // ### Declare all of this object's options here
    // ### For the "flags" of each option, you should typically specify  
    // ### one of OptionBase::buildoption, OptionBase::learntoption or 
    // ### OptionBase::tuningoption. Another possible flag to be combined with
    // ### is OptionBase::nosave

    declareOption(ol, "n_left_context", &ProcessSymbolicSequenceVMatrix::n_left_context, OptionBase::buildoption,
                  "Number of elements at the left of (or. before) the target element (if < 0, all elements to the left are included until a delimiter is met)\n");
    declareOption(ol, "n_right_context", &ProcessSymbolicSequenceVMatrix::n_right_context, OptionBase::buildoption,
                  "Number of elements at the right of (or after) the target element (if < 0, all elements to the right are included until a delimiter is met)\n");
    declareOption(ol, "conditions_offset", &ProcessSymbolicSequenceVMatrix::conditions_offset, OptionBase::buildoption,
                  "Offset for the position of the element on which conditions are tested (default = 0)\n");
    declareOption(ol, "conditions_for_exclusion", &ProcessSymbolicSequenceVMatrix::conditions_for_exclusion, OptionBase::buildoption,
                  "Indication that the specified conditions are for the exclusion (true) or inclusion (false) of elements in the VMatrix\n");
    declareOption(ol, "full_context", &ProcessSymbolicSequenceVMatrix::full_context, OptionBase::buildoption,
                  "Indication that ignored elements of context should be replaced by the next nearest valid element\n");
    declareOption(ol, "put_only_target_attributes", &ProcessSymbolicSequenceVMatrix::put_only_target_attributes, OptionBase::buildoption,
                  "Indication that the only target fields of the VMatrix rows should be the (target) attributes of the context's target element\n");
    declareOption(ol, "use_last_context", &ProcessSymbolicSequenceVMatrix::use_last_context, OptionBase::buildoption,
                  "Indication that the last accessed context should be put in a buffer.\n");
    declareOption(ol, "conditions", &ProcessSymbolicSequenceVMatrix::conditions, OptionBase::buildoption,
                  "Conditions to be satisfied for the exclusion or inclusion (see conditions_for_exclusion) of elements in the VMatrix\n");
    declareOption(ol, "string_conditions", &ProcessSymbolicSequenceVMatrix::string_conditions, OptionBase::buildoption,
                  "Conditions, in string format, to be satisfied for the exclusion or inclusion (see conditions_for_exclusion) of elements in the VMatrix\n");
    declareOption(ol, "delimiters", &ProcessSymbolicSequenceVMatrix::delimiters, OptionBase::buildoption,
                  "Delimiters of context\n");
    declareOption(ol, "string_delimiters", &ProcessSymbolicSequenceVMatrix::string_delimiters, OptionBase::buildoption,
                  "Delimiters, in string format, of context\n");
    declareOption(ol, "ignored_context", &ProcessSymbolicSequenceVMatrix::ignored_context, OptionBase::buildoption,
                  "Elements to be ignored in context\n");
    declareOption(ol, "string_ignored_context", &ProcessSymbolicSequenceVMatrix::string_ignored_context, OptionBase::buildoption,
                  "Elements, in string format, to be ignored in context\n");
    declareOption(ol, "source", &ProcessSymbolicSequenceVMatrix::source, OptionBase::buildoption,
                  "Source VMat, from which contexts are extracted\n");
  

    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void ProcessSymbolicSequenceVMatrix::build_()
{
  
    if(!source) PLERROR("In SelectAttributeSequenceVMatrix::build_() : no source defined");
  
    //  defineSizes(source->inputsize(),source->targetsize(),source->weightsize()); pas bon car ecrase declare options
    n_attributes = source->width();
    row.resize(n_attributes);
    element.resize(n_attributes);
    target_element.resize(n_attributes);

    fixed_context = n_left_context >=0 && n_right_context >= 0;

    // from string to int format on ...

    // conditions
    from_string_to_int_format(string_conditions, conditions);
    string_conditions.clear();

    // delimiters
    from_string_to_int_format(string_delimiters, delimiters);
    string_delimiters.clear();

    // ignored_context
    from_string_to_int_format(string_ignored_context, ignored_context);
    string_ignored_context.clear();

    // gathering information from source VMat

    indices.clear();
    int current_context_length = 0;
    ProgressBar *pb = new ProgressBar("Gathering information from source VMat of length " + tostring(source->length()), source->length());
    for(int i=0; i<source->length(); i++)
    {
        source->getRow(i,row);

        if(is_true(delimiters,row))
        {
            max_context_length = max_context_length < current_context_length ? current_context_length : max_context_length;
            current_context_length = 0;
        }
        else
        {
            if(!full_context || !is_true(ignored_context,row)) current_context_length++;
        }

        // Testing conditions for inclusion/exclusion

        if(i + conditions_offset >= 0 && i + conditions_offset < source->length())
        {
            source->getRow(i+conditions_offset,row);
            if(is_true(conditions,row))
            {
                if(!conditions_for_exclusion) indices.append(i);
            }
            else
            {
                if(conditions_for_exclusion) indices.append(i);
            }
        }
        pb->update(i+1);
    }

    max_context_length = max_context_length < current_context_length ? current_context_length : max_context_length;

    if(n_left_context >= 0 && n_right_context >= 0)
        max_context_length = 1 + n_left_context + n_right_context;

    length_ = indices.length();
    width_ = n_attributes*(max_context_length);

    inputsize_ = max_context_length * source->inputsize();
    targetsize_ = max_context_length * source->targetsize();
    weightsize_ = source->weightsize();

    if(inputsize_+targetsize_ != width_) PLERROR("In ProcessSymbolicSequenceVMatrix:build_() : inputsize_ + targetsize_ != width_");

    current_row_i.resize(width_);
    current_target_pos = -1;
    current_context_pos.clear();
    lower_bound = 1;
    upper_bound = -1;

    if(n_left_context < 0) 
    {
        left_context.resize(width_);
        left_positions.resize(max_context_length);
    }
    else 
    {
        left_context.resize(n_attributes*n_left_context);
        left_positions.resize(n_left_context);
    }
   
    if(n_right_context < 0) 
    {
        right_context.resize(width_);
        right_positions.resize(max_context_length);
    }
    else 
    {
        right_context.resize(n_attributes*n_right_context);
        right_positions.resize(n_right_context);
    }

    if(put_only_target_attributes)
    {
        targetsize_ = source->targetsize();
        width_ = inputsize_ + targetsize();
    }

}

void ProcessSymbolicSequenceVMatrix::build()
{
    inherited::build();
    build_();
}

real ProcessSymbolicSequenceVMatrix::getStringVal(int col, const string & str) const
{
    if(source)
    {
        int src_col;
        if(col < inputsize_)
            src_col = col%source->inputsize();
        else
            src_col = source->inputsize() + (col-inputsize_)%source->targetsize();
        return source->getStringVal(src_col,str);
    }
      
    return MISSING_VALUE;
}

string ProcessSymbolicSequenceVMatrix::getValString(int col, real val) const
{
    if(source)
    {
        int src_col;
        if(col < inputsize_)
            src_col = col%source->inputsize();
        else
            src_col = source->inputsize() + (col-inputsize_)%source->targetsize();
        return source->getValString(src_col,val);
    }
      
    return "";
}

Vec ProcessSymbolicSequenceVMatrix::getValues(int row, int col) const
{
    if(row < 0 || row >= length_) PLERROR("In ProcessSymbolicSequenceVMatrix::getValues() : invalid row %d, length()=%d", row, length_);
    if(col < 0 || col >= length_) PLERROR("In ProcessSymbolicSequenceVMatrix::getValues() : invalid col %d, width()=%d", col, width_);
    if(source)
    {
        int src_col;
        if(col < inputsize_)
            src_col = col%source->inputsize();
        else
            src_col = source->inputsize() + (col-inputsize_)%source->targetsize();
        return source->getValues(indices[row],src_col);
    }
    return Vec(0);
}

Vec ProcessSymbolicSequenceVMatrix::getValues(const Vec& input, int col) const
{
    if(col < 0 || col >= length_) PLERROR("In ProcessSymbolicSequenceVMatrix::getValues() : invalid col %d, width()=%d", col, width_);
    if(source)
    {
        int src_col;
        if(col < inputsize_)
            src_col = col%source->inputsize();
        else
            src_col = source->inputsize() + (col-inputsize_)%source->targetsize();
        return source->getValues(input,src_col);
    }
    return Vec(0);
}

PP<Dictionary> ProcessSymbolicSequenceVMatrix::getDictionary(int col) const
{
    if(col < 0 || col >= length_) PLERROR("In ProcessSymbolicSequenceVMatrix::getDictionary() : invalid col %d, width()=%d", col, width_);
    if(source)
    {
        int src_col;
        if(col < inputsize_)
            src_col = col%source->inputsize();
        else
            src_col = source->inputsize() + (col-inputsize_)%source->targetsize();
        return source->getDictionary(src_col);
    }
    return 0;
}

void ProcessSymbolicSequenceVMatrix::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    deepCopyField(conditions, copies);
    deepCopyField(delimiters, copies);
    deepCopyField(ignored_context, copies);
    deepCopyField(source, copies);
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
