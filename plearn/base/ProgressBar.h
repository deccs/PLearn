// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio and University of Montreal
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
   * $Id: ProgressBar.h,v 1.3 2004/02/20 21:11:42 chrish42 Exp $
   * AUTHORS: Pascal Vincent
   * This file is part of the PLearn library.
   ******************************************************* */

// This file contains useful functions for string manipulation
// that are used in the PLearn Library


/*! \file PLearnLibrary/PLearnCore/ProgressBar.h */

#ifndef ProgressBar_INC
#define ProgressBar_INC

#include <string>
#include <iostream>
#include "PP.h"
#include "PStream.h"


namespace PLearn {
using namespace std;
  
class ProgressBar;

//! Base class for pb plugins
class ProgressBarPlugin : public PPointable
{
public:
  ProgressBarPlugin() {}
  virtual ~ProgressBarPlugin() {}
  virtual void addProgressBar(ProgressBar * pb){};
  virtual void killProgressBar(ProgressBar * pb){};
  virtual void update(ProgressBar * pb, int newpos){};
};


//! Simple plugin for displaying text progress bar
class TextProgressBarPlugin : public ProgressBarPlugin
{
  PStream out;
public:
  virtual void addProgressBar(ProgressBar * pb);
  virtual void update(ProgressBar * pb, int newpos);

  TextProgressBarPlugin(ostream& _out);
  TextProgressBarPlugin(PStream& _out);
};


//! Simpler plugin that doesn't display a progress bar at all.  Useful to
//! disable progress bars for operations that are known to be short.
//! Use it as follows:
//!   PP<ProgressBarPlugin> OldPlugin = ProgressBar::getCurrentPlugin();
//!   ProgressBar::setPlugin(new NullProgressBarPlugin);
//!   ... short operations that might otherwise have plugins here ...
//!   ProgressBar::setPlugin(OldPlugin);
struct NullProgressBarPlugin : public ProgressBarPlugin
{ /* all inherited methods are fine... :-) */ };


//! This class will help you display progress of a calculation
//! 
//! Each progressBar you create is connected to the same ProgressBarPlugin object.
//! By default, a  TextProgressBarPlugin that dumps the text in stderr is created and used.
//! 
//! FAQ: 
//! Q #1 : How do I reuse the same progress bar?
//! A #1 : simply call progress_bar(i) again with 'i' from 0..maxpos (The text progress bar plugin will display a new progress bar)
class ProgressBar
{
public:
  string title;
  int currentpos; // current position
  int maxpos;

  // creates a new progressbar with the given title and maxpos
  // *** Note, for now, ignore the stream (someday, remove this argument for 
  // every progressBar creation in PLearn)
  ProgressBar(string _title, int the_maxpos);
  ProgressBar(ostream& _out,string _title, int the_maxpos);
  ProgressBar(PStream& _out,string _title, int the_maxpos);
  
  // moves the progressbar up to position newpos
  void operator()(int newpos){plugin->update(this,newpos);}
 
  void update(int newpos){plugin->update(this,newpos);}

  // this function assumes plugin is always a valid object (it is created statically in the .cc)
  static void setPlugin(PP<ProgressBarPlugin> plugin_) { plugin = plugin_; }
  static PP<ProgressBarPlugin> getCurrentPlugin() { return plugin; }

  // Completes and removes the progressBar 
  void close();
  
  // calls close() if not already done
  ~ProgressBar();
private:
  bool closed;
  static PP<ProgressBarPlugin> plugin;
};


} // end of namespace PLearn

#endif
