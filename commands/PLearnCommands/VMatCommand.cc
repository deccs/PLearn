
// -*- C++ -*-

// VMatCommand.cc
//
// Copyright (C) 2003  Pascal Vincent 
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

/*! \file VMatCommand.cc */
#include "VMatCommand.h"
#include <plearn/db/getDataSet.h>
#include <plearn/misc/vmatmain.h>
#include <plearn/base/lexical_cast.h>

namespace PLearn {
using namespace std;

//! This allows to register the 'VMatCommand' command in the command registry
PLearnCommandRegistry VMatCommand::reg_(new VMatCommand);

VMatCommand::VMatCommand():
    PLearnCommand(
        "vmat",
        "Examination and manipulation of vmat datasets",
        "Usage: vmat info <dataset> \n"
        "       Will info about dataset (size, etc..)\n"
        "   or: vmat fields <dataset> [name_only] [transpose] \n"
        "       To list the fields with their names (if 'name_only' is specified, the indexes won't be displayed,\n"
        "       and if 'transpose' is also added, the fields will be listed on a single line)\n"
        "   or: vmat fieldinfo <dataset> <fieldname_or_num> [--bin]\n"
        "       To display statistics for that field \n"
        "   or: vmat bbox <dataset> [<extra_percent>] \n"
        "       To display the data bounding box (i.e., for each field, its min and max, possibly extended by +-extra_percent ex: 0.10 for +-10% of the data range )\n"
        "   or: vmat cat <dataset> [<optional_vpl_filtering_code>]\n"
        "       To display the dataset \n"
        "   or: vmat sascat <dataset.vmat> <dataset.txt>\n"
        "       To output in <dataset.txt> the dataset in SAS-like tab-separated format with field names on the first line\n"
        "   or: vmat view <dataset>\n"
        "       Interactive display to browse on the data. \n"
        "   or: vmat stats <dataset> \n"
        "       Will display basic statistics for each field \n"
        "   or: vmat convert <source> <destination> [--cols=col1,col2,col3,...]\n"
        "       To convert any dataset into a .amat, .pmat, .dmat or .csv format. \n"
        "       The extension of the destination is used to determine the format you want. \n"
        "       If the option --cols is specified, it requests to keep only the given columns\n"
        "       (no space between the commas and the columns); columns can be given either as a\n"
        "       number (zero-based) or a column name (string).  You can also specify a range,\n"
        "       such as 0-18, or any combination thereof, e.g. 5,3,8-18,Date,74-85\n"
        "       If .csv (Comma-Separated Value) is specified as the destination file, the \n"
        "       following additional options are also supported:\n"
        "         --skip-missings: if a row (after selecting the appropriate columns) contains\n"
        "                          one or more missing values, it is skipped during export\n"
        "         --precision=N:   a maximum of N digits is printed after the decimal point\n"
        "         --delimiter=C:   use character C as the field delimiter (default = ',')\n"
        "   or: vmat gendef <source> [binnum1 binnum2 ...] \n"
        "       Generate stats for dataset (will put them in its associated metadatadir). \n"
        "   or: vmat genvmat <source_dataset> <dest_vmat> [binned{num} | onehot{num} | normalized]\n"
        "       Will generate a template .vmat file with all the fields of the source preprocessed\n"
        "       with the processing you specify\n"
        "   or: vmat genkfold <source_dataset> <fileprefix> <kvalue>\n"
        "       Will generate <kvalue> pairs of .vmat that are splitted so they can be used for kfold trainings\n"
        "       The first .vmat-pair will be named <fileprefix>_train_1.vmat (all source_dataset except the first 1/k)\n"
        "       and <fileprefix>_test_1.vmat (the first 1/k of <source_dataset>\n"
        "   or: vmat diff <dataset1> <dataset2> [<tolerance> [<verbose>]]\n"
        "       Will report all elements that differ by more than tolerance (defauts to 1e-6).\n"
        "       If verbose==0 then print only total number of differences \n"
        "   or: vmat cdf <dataset> [<dataset> ...] \n"
        "       To interactively display cumulative density function for each field \n"
        "       along with its basic statistics \n"
        // "   or: vmat cond <dataset> <condfield#> \n"
        // "       Interactive display of coditional statistics conditioned on the \n"
        // "       conditioning field <condfield#> \n"
        "   or: vmat diststat <dataset> <inputsize>\n"
        "       Will compute and output basic statistics on the euclidean distance \n"
        "       between two consecutive input points \n\n"
        "<dataset> is a parameter understandable by getDataSet: \n"
        + getDataSetHelp()
        ) 
{}


//! The actual implementation of the 'VMatCommand' command 
void VMatCommand::run(const vector<string>& args)
{
    // new vmat sub-commands
    string command = args[0];
    if(command=="bbox")
    {
        string dataspec = args[1];
        real extra_percent = 0.00;
        if(args.size()==3)
            extra_percent = toreal(args[2]);
      
        VMat vm = getDataSet(dataspec);
        TVec< pair<real, real> > bbox = vm->getBoundingBox(extra_percent);
        for(int k=0; k<bbox.length(); k++)
            cout << bbox[k].first << " : " << bbox[k].second << endl;
    }
    else
    {
        // Dirty hack to plug into old vmatmain code
        // Eventually, should get vmatmain code in here and clean

        int argc = (int)args.size()+1;
        char** argv = new char*[argc];
        string commandname = "vmat";
        argv[0] = const_cast<char*>(commandname.c_str());
        for(int i=1 ; i<argc; i++)
            argv[i] = const_cast<char*>(args[i-1].c_str());
        vmatmain(argc, argv);
        delete[] argv;
    }
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
