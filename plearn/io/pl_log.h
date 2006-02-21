// -*- C++ -*-

// pl_log.h
//
// Copyright (C) 2004-2006 Nicolas Chapados 
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

// Authors: Nicolas Chapados, Christian Dorion

/**
 *  @file pl_log.h
 *
 *  
 */


#ifndef pl_log_INC
#define pl_log_INC

#include <vector>
#include <set>
#include <string>
#include "PStream.h"

namespace PLearn {

#ifndef PL_LOG_MAXVERBOSITY
#define PL_LOG_MAXVERBOSITY 1000
#endif

#ifndef PL_LOG_MINVERBOSITY
#define PL_LOG_MINVERBOSITY 1
#endif

#ifndef PL_LOG_VERBOSITY
#define PL_LOG_VERBOSITY 499                 // block EXTREME by default
#endif

enum VerbosityLevel {
    VLEVEL_MAND     = 0,    // Mandatory
    VLEVEL_IMP      = 1,    // Important
    VLEVEL_NORMAL   = 5,    // Normal
    VLEVEL_DBG      = 10,   // Debug Info
    VLEVEL_EXTREME  = 500   // Extreme Verbosity
}; 
  
class PL_Log
{
public:
    //! Constructor
    //! (Use default destructor, copy constructor, etc.)
    PL_Log();
  
    //! Set the actual runtime verbosity.  This is a verbosity threshold;
    //! any "requested_verbosity" less than or equal to this verbosity is
    //! displayed by PL_LOG.
    void verbosity(int v)                     { runtime_verbosity = v; }

    //! Return the current runtime verbosity
    int verbosity() const                     { return runtime_verbosity; }

    //! Bind the actual output PStream; default is stream constructed from cout
    void outputStream(PStream stream)         { output_stream = stream; }

    //! Return the actual output PStream
    PStream outputStream() const              { return output_stream; }

    //! Changes the output_stream outmode
    void outmode(PStream::mode_t outmode_) { output_stream.outmode = outmode_; }

    /**
     *  Underlying logging function.  If "requested_verbosity" is less than
     *  or equal to verbosity, then output_stream is returned; otherwise
     *  null_stream is returned.
     */
    PStream& logger(int requested_verbosity);

    /**
     *  Underlying named logging function.  Use this in conjunction with
     *  the MODULE_LOG define.  If logging is enabled for the specified module,
     *  then return output_stream; otherwise return null_stream.  Note that
     *  named logging operates at the level of "NORMAL" log, so an implicit
     *  "requested_verbosity" of NORMAL is assumed.  If either the runtime or
     *  compile-time verbosity is less than "NORMAL", then null_stream is
     *  returned.
     */
    PStream& namedLogger(const string& module_name);

    /**
     *  Named logging support.  Enable logging for the specified list
     *  of modules.  If the special "__ALL__" keyword is used, then all
     *  named logging is enabled.  If the special "__NONE__" keyword is
     *  used, then no named logging is enabled.  (Default = "__NONE__")
     */
    void enableNamedLogging(const vector<string>& module_names);

    //! Shortcut for enableNamedLogging: first split the module names by comma
    //! and call enableNamedLogging -- convenient for in-code logging enabling
    void enableNamedLogging(const string& comma_separated_module_names);

    //! Return the list of modules for which named logging is enabled
    vector<string> namedLogging() const;
    
    //! Return number of times logger() has been called
    long loggerCount() const                   { return logger_count; }
  
    //! Return system-wide PL_Log
    static PL_Log& instance();

    //! Support stuff for heading manipulator
    struct Heading {
        Heading(string h_) : h(h_) {}
        string h;
    };

    /**
     * Parses a string to see whether or not it names a VerbosityLevel. If it
     * doesn't, tries the cast to an int.
     */
    static VerbosityLevel vlevelFromString(const string& v);
  
protected:
    int runtime_verbosity;
    PStream output_stream;
    PStream null_stream;
    long logger_count;              //!< Number of times logger() has been called
    set<string> enabled_modules;    //!< Modules for which logging is enabled.

    enum {
        AllModules = 0,           //!< Named logging enabled for all modules
        NoModules = 1,            //!< Named logging enabled for no modules
        SomeModules = 2           //!< Named logging enabled for some modules
    } named_logging_kind;
};


//#####  Main interface to the logging system  ################################

#define PL_LOG(v) if (v <= PL_LOG_VERBOSITY) PL_Log::instance().logger(v)
#define MAND_LOG      PL_LOG(VLEVEL_MAND)
#define IMP_LOG       PL_LOG(VLEVEL_IMP) 
#define NORMAL_LOG    PL_LOG(VLEVEL_NORMAL)
#define DBG_LOG       PL_LOG(VLEVEL_DBG)   
#define EXTREME_LOG   PL_LOG(VLEVEL_EXTREME)

/**
 *  If the "PL_LOG_MODULE_NAME" variable is defined before pl_log.h is
 *  included, then MODULE_LOG is defined to provide module-specified logging
 *  for that module.  From an implementation standpoint, we define a static
 *  string variable that holds the module name, and that variable is used.
 *
 *  NOTE: to avoid conflicts and strange behavior, you should only use this
 *  define within a .cc file (outside of templates).  For templates, you have
 *  to rely on NAMED_LOG for now.
 */
#ifdef PL_LOG_MODULE_NAME
# ifndef MODULE_LOG

   namespace {
       string pl_log_module_name_string(PL_LOG_MODULE_NAME);
   }

#  define MODULE_LOG                                                  \
      if (VLEVEL_NORMAL <= PL_LOG_VERBOSITY)                          \
          PL_Log::instance().namedLogger(pl_log_module_name_string)

#endif // MODULE_LOG
#endif // PL_LOG_MODULE_NAME


//! Finally, an explicitly named log.  For use within templates.
#define NAMED_LOG(name)                         \
    if (VLEVEL_NORMAL <= PL_LOG_VERBOSITY)      \
        PL_Log::instance().namedLogger(name)

//#####  Heading Support  #####################################################

//! Manipulator that displays a separator with the Logger count
PStream& plsep(PStream&);

//! Manipulator that displays a nice heading
inline PL_Log::Heading plhead(string s)
{
    return PL_Log::Heading(s);
}

//! Actually draw the heading
PStream& operator<<(PStream&, PL_Log::Heading);

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
