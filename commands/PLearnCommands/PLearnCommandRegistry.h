// -*- C++ -*-4 1999/10/29 20:41:34 dugas

// PLearnCommandRegistry.h
// Copyright (C) 2002 Pascal Vincent
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
   * $Id: PLearnCommandRegistry.h,v 1.4 2004/02/16 20:24:59 tihocan Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#ifndef PLearnCommandRegistry_INC
#define PLearnCommandRegistry_INC

#include <map>
#include <string>
#include "PLearnCommand.h"


namespace PLearn <%
using namespace std;

class PLearnCommandRegistry
{
protected:
  typedef map<string, PLearnCommand*> command_map;

  //! Returns a reference to the unique command map
  static command_map& commands();

  //! Issues a "bad command" message
  static void badcommand(const string& commandname);

 
public:

  //! registers a command
  static void do_register(PLearnCommand* command);

  //! checks wether a command is registered with that name
  static bool is_registered(const string& commandname);
  
  //! prints a list of all commands with their one-line summary
  static void print_command_summary(ostream& out);

  //! run the given (registered) command with the given arguments
  static void run(const string& commandname, const vector<string>& args);
  
  //! prints out detailed help for the given command on the given stream
  static void help(const string& commandname, ostream& out);

  //! This constructor will simply register the given PLearnCommand
  inline PLearnCommandRegistry(PLearnCommand* plcommand)
  { do_register(plcommand); }
  
};
  

%> // end of namespace PLearn

#endif
