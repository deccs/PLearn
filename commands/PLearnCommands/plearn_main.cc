// -*- C++ -*-

// plearn_main.cc
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
   * $Id$
   ******************************************************* */

#include "plearn_main.h"
#include "PLearnCommandRegistry.h"
#include <plearn/base/stringutils.h>
#include <plearn/math/random.h>
#include <plearn/sys/PLMPI.h>
#include <plearn/io/pl_log.h>
#include <plearn/misc/Calendar.h>
#include <plearn/db/getDataSet.h>
#include <plearn/misc/PLearnService.h>
#include <plearn/vmat/VMat.h>
#include <exception>

namespace PLearn {
using namespace std;

static bool is_command( string& possible_command )
{
  if(PLearnCommandRegistry::is_registered(possible_command))
    return true; 

  if( file_exists(possible_command) )
  {
    possible_command = "run";
    return false;
  }
  
  PLERROR( "%s appears to neither be a known PLearn command, "
           "nor an existing .plearn script", possible_command.c_str() );
  return false;
}

static void output_version(int major_version, int minor_version, int fixlevel )
{
  if(major_version == -1)
    return;
  cerr << prgname()
       << " "      << major_version;
  if (minor_version >= 0) {
    cerr << "."      << minor_version;
    if (fixlevel >= 0)
      cerr << "."      << fixlevel;
  }
  cerr << "  ("    << __DATE__ << " "
       << __TIME__ << ")"      << endl;
}

static void set_global_calendars(string command_line_option)
{
  // Assume command-line-option of the form
  // CalendarName1:CalendarFilename1,CalendarName2:CalendarFilename2,...
  vector<string> names_files = split(command_line_option, ',');
  for (int i=0, n=names_files.size() ; i<n ; ++i) {
    vector<string> namefile = split(names_files[i], ':');
    if (namefile.size() != 2)
      PLERROR("Cannot understand '%s' for specifying a global calendar.",
              names_files[i].c_str());
    string calname = namefile[0];
    string filename = namefile[1];
    VMat dates_vmat = getDataSet(filename);
    Vec dates_vec = dates_vmat.getColumn(0);
    Calendar::setGlobalCalendar(calname,
                                Calendar::makeCalendar(dates_vec));
    cerr << "Set global calendar \"" << calname << "\" from file \"" << filename << '"' << endl;
  }
}

static string global_options( vector<string>& command_line,
                              int major_version, int minor_version, int fixlevel )
{
  int argc                 = command_line.size();

  // Note that the findpos function (stringutils.h) returns -1 if the
  // option is not found.
  int no_version_pos       = findpos( command_line, "--no-version" );  

  // Note that the verbosity_value_pos IS NOT EQUAL TO verbosity_pos+1 if
  // (verbosity_pos == -1)!!!
  int verbosity_pos                = findpos( command_line, "--verbosity"  );
  int verbosity_value_pos          = -1; // ... 
  VerbosityLevel verbosity_value   = VLEVEL_NORMAL;

  if ( verbosity_pos != -1 )
  {
    // ... here we can set verbosity_value_pos:
    verbosity_value_pos = verbosity_pos+1;
    if ( verbosity_value_pos >= argc )
      PLERROR("Option --verbosity must be followed by a VerbosityLevel name "
              "or by an integer value.");
    verbosity_value =
      PL_Log::vlevel_from_string( command_line[verbosity_value_pos] );
  }

  // Option to establish global calendars loaded from a matrix
  int global_calendar_pos = findpos(command_line, "--global-calendars");
  int global_calendar_value_pos = -1;
  if (global_calendar_pos != -1) 
    {
    global_calendar_value_pos = global_calendar_pos+1;
    if (global_calendar_value_pos >= argc)
      PLERROR("Option --global-calendars must be followed by a list of the form\n"
              "CalendarName1:CalendarFilename1,CalendarName2:CalendarFilename2,...");
    set_global_calendars(command_line[global_calendar_value_pos]);
    }
  
  // Option for parallel processing through PLearnService
  int servers_pos = findpos(command_line, "--servers");
  int serversfile_pos = -1;
  if (servers_pos != -1)
    {
      serversfile_pos = servers_pos+1;
      if ( serversfile_pos >= argc)
        PLERROR("Option --servers must be followed by the name of a servers file containing a list of hostname pid tcpport\n");
      string serversfile = command_line[serversfile_pos];
      PLearnService::instance().connectToServers(serversfile);
    }
  
  // The following removes the options from the command line. It also
  // parses the plearn command as being the first non-optional argument on
  // the line. IF ANY OPTION IS ADDED, PLEASE MAKE SURE TO REMOVE IT BY
  // ADDING A CONDITION IN THE if STATEMENT.
  int    cleaned     = 0;
  string the_command = "";
  vector<string> old( command_line );

  for ( int c=0; c < argc; c++ )
    // Neglecting to copy options
    if ( c != no_version_pos             &&
         c != verbosity_pos              &&
         c != verbosity_value_pos        &&
         c != global_calendar_pos        &&
         c != global_calendar_value_pos  &&
         c != servers_pos                &&
         c != serversfile_pos
         )
    {
      if ( the_command == "" )
      {
        the_command = old[c];

        // If it is not a command, then it is a file that must be forwarded
        // to the run command (See the is_command function).
        if ( !is_command( the_command ) )  
          command_line[cleaned++] = old[c];
      }
      else
        command_line[cleaned++] = old[c];
    }
  command_line.resize( cleaned ); // Truncating the end of the vector.
  
  if ( no_version_pos == -1 )
    output_version( major_version, minor_version, fixlevel );
  PL_Log::instance().verbosity( verbosity_value );

  return the_command;
}

void plearn_terminate_handler()
{
  cerr << "PLEARN UNUSUAL TERMINATION: plearn_terminate_handler called, probably due \n"
       << "to second exception thrown while unwinding stack following a first \n"
       << "exception. ABORTING!" << endl;
  abort();
}

int plearn_main( int argc, char** argv,
                 int major_version, int minor_version, int fixlevel )
{
  set_terminate(plearn_terminate_handler);

  try {

  PLMPI::init(&argc, &argv);

  seed();

  // set program name
  prgname(argv[0]);

  
  vector<string> command_line = stringvector(argc-1, argv+1);
  string command = global_options( command_line, major_version, minor_version, fixlevel );

  if ( command == "" )
  {
    cerr << "Type '" << prgname() << " help' for help" << endl;
    return 0;
  }

  PLearnCommandRegistry::run(command, command_line);				
  PLMPI::finalize();
  
  } // end of try
  catch(const PLearnError& e)
  {
    cerr << "FATAL ERROR: " << e.message() << endl;
  }
  catch (...) 
  {
    cerr << "FATAL ERROR: uncaught unknown exception "
         << "(ex: out-of-memory when allocating a matrix)" << endl;
  }

  return 0;
}

} // end of namespace PLearn
