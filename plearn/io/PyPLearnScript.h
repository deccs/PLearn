// -*- C++ -*-

// PyPLearnScript.h
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
 * $Id$ 
 ******************************************************* */

// Authors: Christian Dorion

/*! \file PyPLearnScript.h */


#ifndef PyPLearnScript_INC
#define PyPLearnScript_INC

#include <plearn/io/PPath.h>
#include <plearn/io/openString.h>
#include <plearn/base/Object.h>

namespace PLearn {

class PyPLearnScript: public Object
{
public:
    // STATIC METHODS

    /*!
      Given a filename, call an external process with the given name (default
      = "pyplearn_driver.py") to preprocess it and return the preprocessed
      version.  Arguments to the subprocess can be passed.
      Passing '--help' passes it unmodified to the subprogram
      and is assumed to print a help string.  If '--dump' is passed,
      the Python-preprocessed script is printed to standard output.
    */
    static PP<PyPLearnScript> process(
        const std::string& filename,
        const std::vector<std::string>& args = std::vector<std::string>(),
        const std::string& drivername        = "pyplearn_driver.py" );

    /*!
     * This static method forwards its arguments to process() and returns a 
     * pointer on an object of template type Obj by loading the resulting plearn 
     * script. Note that the object IS NOT BUILT since one may want to set other 
     * options prior to calling build().  
     */
    template<class Obj>
    static PP<Obj> load(
        const std::string& filename,
        const std::vector<std::string>& args = std::vector<std::string>(),
        const std::string& drivername        = "pyplearn_driver.py" )
    {
        PP<PyPLearnScript> script = PyPLearnScript::process(filename, args, drivername);

        PStream in = openString( script->plearn_script, PStream::plearn_ascii );
        PP<Obj> o  = new Obj();
        in >> o;

        return o;
    }

 
private:
    typedef Object inherited;

    //! This does the actual building. 
    void build_();

protected:
    //! Declares this class' options.
    static void declareOptions(OptionList& ol);

public:

    // ************************
    // * public build options *
    // ************************

    //! The plearn_script
    std::string plearn_script;
  
    //! Variables set at command line
    std::map<std::string, std::string> vars;

    //! Script is in fact an help message
    bool do_help;

    //! Dump the script and forget it
    bool do_dump;  

    /*!
      Informations relative to the experiment settings, to be wrote in an
      expdir file.
    */
    std::string metainfos;

    //! The expdir of the experiment described by the script
    PPath expdir;
  
    // ****************
    // * Constructors *
    // ****************

    //! Default constructor.
    PyPLearnScript();

    //! Returns the internal script representation
    string getScript() { return plearn_script; }

    //! Saving metainfos to the expdir
    void close();
  
    // Declares other standard object methods.
    PLEARN_DECLARE_OBJECT(PyPLearnScript);

    // simply calls inherited::build() then build_() 
    virtual void build();

    //! Transforms a shallow copy into a deep copy
    virtual void makeDeepCopyFromShallowCopy(CopiesMap& copies);
};

// Declares a few other classes and functions related to this class
DECLARE_OBJECT_PTR(PyPLearnScript);
  
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
