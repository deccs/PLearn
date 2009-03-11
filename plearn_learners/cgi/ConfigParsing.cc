// -*- C++ -*-

// ConfigParsing.cc
//
// Copyright (C) 2009 Frederic Bastien
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

// Authors: Frederic Bastien

/*! \file ConfigParsing.cc */


#include "ConfigParsing.h"
#include <plearn/vmat/TextFilesVMatrix.h>
#include <plearn/io/openFile.h>
#include <plearn/base/stringutils.h>

namespace PLearn {
using namespace std;

//! This allows to register the 'ConfigParsing' command in the command registry
PLearnCommandRegistry ConfigParsing::reg_(new ConfigParsing);

ConfigParsing::ConfigParsing()
    : PLearnCommand(
        "ConfigParsing",
        ">>>> INSERT A SHORT ONE LINE DESCRIPTION HERE",
        ">>>> INSERT SYNTAX AND \n"
        "FULL DETAILED HELP HERE \n"
        )
{}

//! The actual implementation of the 'ConfigParsing' command
void ConfigParsing::run(const vector<string>& args)
{
    // *** PLEASE COMPLETE HERE ****
/*    args1 = conf/conf.all.csv;
    args2 = conf/1convertCSV0709toPLearn.inc;
    args3 = conf/3b_remove_col.inc;
    args4 = conf/3fix_missing.inc;
    args5 = conf/9dichotomize.inc;
    args6 = conf/global_imputation_specifications.inc;
*/
    PLCHECK(args.size()==6);
    TextFilesVMatrix input = TextFilesVMatrix();
    input.auto_build_map = 0  ;
    input.default_spec="char";
//#auto_extend_map = 0  ;
    input.build_vmatrix_stringmap = 1  ;
    input.delimiter = ","  ;//TODO ; or auto?
    input.quote_delimiter = '"';
    input.skipheader.append(1);
    input.reorder_fieldspec_from_headers=1;
    input.txtfilenames.append(args[0]);
    input.partial_match=1;
    input.setMetaDataDir(args[0]+".metadatadir");
    input.build();
    PStream f_csv = openFile(PPath(args[1]),PStream::raw_ascii,"w");
    PStream f_remove = openFile(args[2],PStream::raw_ascii,"w");
    PStream f_missing = openFile(args[3],PStream::raw_ascii,"w");
    PStream f_dichotomize = openFile(args[4],PStream::raw_ascii,"w");
    PStream f_imputation = openFile(args[5],PStream::raw_ascii,"w");

    f_csv<<"$INCLUDE{conf/date.inc}"<<endl;
    f_remove<<"$INCLUDE{conf/date.inc}"<<endl;
    f_missing<<"$INCLUDE{conf/date.inc}"<<endl;
    f_dichotomize<<"$INCLUDE{conf/date.inc}"<<endl;
    f_imputation<<"$INCLUDE{conf/date.inc}"<<endl;
    for(int i=0;i<input.length();i++){
        TVec<string> r = input.getTextFields(i);
        char c = r[0][0];
        if(c=='#' || r[0].empty())//comment
            continue;
        if(!r[1].empty()){
            f_csv << (r[0]);
            f_csv << (" : ");
            f_csv << (r[1]) << endl;
        }
        string y = lowerstring(r[2]);//TODO check that this is an accepted command.
        if(y=="y" ||y=="yes"){//comment
            f_remove << (r[0]) << endl;
        }else if(y=="n" ||y=="no"||y==""){
        }else{
            PLERROR("Unknow value in column C:'%s'",r[2].c_str());
        }
        if(!r[3].empty()){
            f_missing << (r[0]);
            f_missing << (" : ");
            f_missing << (r[3]);//TODO check that this is an accepted command.
            f_missing << endl;
        }
        if(!r[4].empty()){
            f_imputation << (r[0]);
            f_imputation << (" : ");
            f_imputation << (r[4]);//TODO check that this is an accepted command.
            f_imputation << endl;
        }
        if(!r[5].empty()){
            f_dichotomize <<r[0]<<" : ["<< (r[5]) << " ]"<<endl;
        }

        
    }
    f_csv<<"$INCLUDE{conf/date_undef.inc}"<<endl;
    f_remove<<"$INCLUDE{conf/date_undef.inc}"<<endl;
    f_missing<<"$INCLUDE{conf/date_undef.inc}"<<endl;
    f_dichotomize<<"$INCLUDE{conf/date_undef.inc}"<<endl;
    f_imputation<<"$INCLUDE{conf/date_undef.inc}"<<endl;

        

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
