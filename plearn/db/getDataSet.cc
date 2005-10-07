// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999,2000 Pascal Vincent, Yoshua Bengio and University of Montreal
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
 * AUTHORS: Pascal Vincent
 * This file is part of the PLearn library.
 ******************************************************* */

#include "getDataSet.h"
#include <plearn/io/MatIO.h>              //!< For loadAsciiSingleBinaryDescriptor().

#include <plearn/base/stringutils.h>      //!< For split_on_first().
#include <plearn/io/fileutils.h>          //!< For isfile().
#include <plearn/io/pl_log.h>
#include <plearn/io/PyPLearnScript.h>
#include <plearn/vmat/DiskVMatrix.h>
#include <plearn/vmat/FileVMatrix.h>
#include <plearn/vmat/VMat.h>
#include <plearn/vmat/VVMatrix.h>

namespace PLearn {
using namespace std;

////////////////////
// getDataSetDate //
////////////////////
time_t getDataSetDate(const PPath& dataset_path)
{
    return getDataSet(dataset_path)->getMtime();
}

////////////////
// getDataSet //
////////////////
VMat getDataSet(const PPath& dataset_path)
{
    VMat vm;
    // Parse the base file name and the potential parameters.
    string dataset_abs;
    map<string, string> params;
    parseBaseAndParameters(dataset_path.absolute(), dataset_abs, params);
    PPath dataset(dataset_abs);
    bool use_params = false;

    // See getDataSetHelp() for supported formats.
    string ext = dataset.extension();
    if (isfile(dataset)) {
        if (ext == "amat") {
            // Check if the extension is ".bin.amat".
            // (old deprecated extension)
            if (dataset.find(".bin.", ((string::size_type) dataset.size()) - 9) != string::npos) {
                PLERROR("In getDataSet - The '.bin.amat' extension is deprecated, you "
                        "must now use the .abmat extension");
            } else
                vm = loadAsciiAsVMat(dataset);
        } else if (ext == "abmat") {
            Mat tempMat;
            loadAsciiSingleBinaryDescriptor(dataset, tempMat);
            vm = VMat(tempMat);
        } else if (ext == "pmat") {
            vm = new FileVMatrix(dataset);
        } else if (ext == "txtmat") {
            PLERROR("In getDataSet - The old .txtmat files are deprecated, please "
                    "use a standard .vmat or .pymat script");
        } else if (ext == "vmat") {
            use_params = true;
            const string code = readFileAndMacroProcess(dataset, params);
            if (removeblanks(code)[0] == '<') {
                // Old XML-like format.
                PLDEPRECATED("In getDataSet - File %s is using the old XML-like VMat format, " 
                             "you should switch to a PLearn script (ideally a .pymat file).",
                             dataset.absolute().c_str());
                vm = new VVMatrix(dataset);
            } else {
                vm = dynamic_cast<VMatrix*>(newObject(code));
                if (vm.isNull())
                    PLERROR("In getDataSet - Object described in %s is not a VMatrix subclass",
                            dataset.absolute().c_str());
            }
        } else if (ext == "pymat" || ext == "py") {
            use_params = true;
            if (ext == "py")
                PLWARNING("In getDataSet - Note that the Python code in a '.py' file must return a pl.VMatrix");
            // Convert 'params' to a vector<string> with elements "paramX=valueX".
            map<string, string>::const_iterator it = params.begin();
            vector<string> params_vec;
            for (; it != params.end(); it++)
                params_vec.push_back(it->first + "=" + it->second);
            PP<PyPLearnScript> pyplearn_script = PyPLearnScript::process(dataset, params_vec);
            const string code = pyplearn_script->getScript();
            vm = dynamic_cast<VMatrix*>(newObject(code));
            if (vm.isNull())
                PLERROR("In getDataSet - Object described in %s is not a VMatrix subclass",
                        dataset.absolute().c_str());
        }
        else 
            PLERROR("In getDataSet - Unknown extension for VMat file: %s", ext.c_str());
        if (!use_params && !params.empty())
            PLWARNING("In getDataSet - Ignoring parameters when reading file %s",
                      dataset.absolute().c_str());
        // Set default metadata directory if not already set.
        if (!vm->hasMetaDataDir())
            vm->setMetaDataDir(dataset.dirname() / (dataset.basename() + ".metadata"));
    }
    else if (isdir(dataset)) {
        if (ext == "dmat")
            vm = new DiskVMatrix(dataset);
        else
            PLERROR("In getDataSet - Unknown extension for VMat directory: %s", ext.c_str());
    }
    else
        PLERROR("In getDataSet - cannot open dataset \"%s\"", dataset.c_str());
  
    vm->loadAllStringMappings();
    vm->unduplicateFieldNames();

    if (vm->inputsize() < 0 && vm->targetsize() < 0 && vm->weightsize() < 0) {
        DBG_LOG << "In getDataSet - The loaded VMat has no inputsize, targetsize "
                << "or weightsize specified, setting them to (" << vm->width() << ",0,0)"
                << endl;
        vm->defineSizes(vm->width(), 0, 0);
    }

    // Ensure sizes do not conflict with width.
    if (vm->inputsize() >= 0 && vm->targetsize() >= 0 && vm->weightsize() >= 0 &&
        vm->width() >= 0 &&  vm->width() < vm->inputsize() + vm->targetsize() + vm->weightsize())
        PLERROR("In getDataSet - The matrix width (%d) should not be smaller than inputsize (%d) "
                "+ targetsize (%d) + weightsize (%d)",
                vm->width(), vm->inputsize(), vm->targetsize(), vm->weightsize());

    return vm;
}

////////////////////
// getDataSetHelp //
////////////////////
string getDataSetHelp() {
    return "Dataset specification must be either:\n"
        "- a file with extension:\n"
        "  .amat   : ASCII VMatrix\n"
        "  .abmat  : ASCII binary (0/1) VMatrix\n"
        "  .pmat   : PLearn file VMatrix\n"
        "  .vmat   : PLearn script\n"
        "  .pymat  : Python script\n"
        "- a directory with extension:\n"
        "  .dmat   : Disk VMatrix\n"
        "\n"
        "Optionally, arguments for scripts can be given with the following syntax:\n"
        "  path/file.ext::arg1=val1::arg2=val2::arg3=val3\n";
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
