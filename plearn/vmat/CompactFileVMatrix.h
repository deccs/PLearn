// -*- C++ -*-

// CompactFileVMatrix.h
//
// Copyright (C) 2007 Olivier Breuleux
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


/*! \file PLearnLibrary/PLearnCore/VMat.h */

#ifndef CompactFileVMatrix_INC
#define CompactFileVMatrix_INC

#include "RowBufferedVMatrix.h"

// While under development, we use this define to control
// whether to use the NSPR 64 bit file access or the old std C FILE*
#define USE_NSPR_FILE
struct PRFileDesc;

namespace PLearn {
using namespace std;


//! Each row contains a certain amount of field groups. This
//! struct provides information about how a field group is encoded and
//! how to put it in a Vec.
struct GroupInfoNode {
    GroupInfoNode(char type_, int length_, int max_, int bits_per_value_, GroupInfoNode* next_):
        type(type_),
        length(length_),
        max(max_),
        bits_per_value(bits_per_value_),
        next(next_)
    {}

    char type; //!< type of data (i for int, o for onehot)
    int length; //!< number of fields
    int max; //!< maximal value of a field (entries will be normalized by that, there is no boundcheck)
    int bits_per_value; //!< amount of bits used to encode each field (must be <= 8) (8 yields fastest conversion)
    bool active; //!< true if this field group is active
    int compact_length; //!< length of the group in the file
    GroupInfoNode* next; //!< next node or NULL
};


//! A VMatrix that exists in a .pmat file (native PLearn matrix format,
//! same as for Mat).
class CompactFileVMatrix: public RowBufferedVMatrix
{

private:
    typedef RowBufferedVMatrix inherited;

protected:
    PPath filename_;
//    char* active_list_;
    Vec active_list_;
    bool in_ram_;


#ifdef USE_NSPR_FILE
    PRFileDesc* f;
#else
    FILE* f;
#endif
//    bool file_is_bigendian;
//    bool file_is_float;

    int compact_width_; //!< width of a compacted row
    GroupInfoNode* info_; //!< information on how each field group is encoded
    char* contents;

public:

    CompactFileVMatrix();
    CompactFileVMatrix(const PPath& filename); //!<  opens an existing file

protected:

    static void declareOptions(OptionList & ol);
    virtual void getNewRow(int i, const Vec& v) const;

    //! Close the current '.pmat' file.
    virtual void closeCurrentFile();

public:

    //! Re-write the header with all current field values.
    virtual void updateHeader();

    //! The amount of field groups in the matrix
    virtual int nGroups() const;
    
    //! The encoding of the group-th field group ('i' or 'o')
    virtual char groupEncoding(int group) const;

    //! The amount of fields in the group-th field group
    virtual int groupNFields(int group) const;

    //! The length of the group-th field group
    virtual int groupLength(int group) const;

    virtual void put(int i, int j, real value);
    virtual void putSubRow(int i, int j, Vec v);
    virtual void appendRow(Vec v);
    virtual void flush();

    virtual void build();

    PLEARN_DECLARE_OBJECT(CompactFileVMatrix);


    //! Transform a shallow copy into a deep copy.
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);

    //! Destructor.
    virtual ~CompactFileVMatrix();

private:

    void build_();
    GroupInfoNode* getGroup(int group) const;

    // seek to element i,j in file
    void moveto(int i, int j=0) const;
    void cleanup();

};

DECLARE_OBJECT_PTR(CompactFileVMatrix);

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
