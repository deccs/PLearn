// -*- C++ -*-
// ObservationWindow.cc
// 
// Copyright (C) 2006 Christian Dorion
// Copyright (C) 2006 ApStat Technologies Inc.
// Copyright (C) 2007 Xavier Saint-Mleux, ApSTAT Technologies inc.
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

/*! \file ObservationWindow.cc */
#include "ObservationWindow.h"

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(
    ObservationWindow,
    "ONE LINE USER DESCRIPTION",
    "MULTI LINE\nHELP FOR USERS"
    );


ObservationWindow::ObservationWindow(int window)
    : m_window(window), unlimited_size(window == -2)
{
    forget();
}


// ### Nothing to add here, simply calls build_
void ObservationWindow::build()
{
    inherited::build();
    build_();
}

void ObservationWindow::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    // ### Call deepCopyField on all "pointer-like" fields
    // ### that you wish to be deepCopied rather than
    // ### shallow-copied.
    // ### ex:
    // deepCopyField(trainvec, copies);

    // ### Remove this line when you have fully implemented this method.
    //PLERROR("ObservationWindow::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");

    deepCopyField(m_observations, copies);
    deepCopyField(m_obs_weights,  copies);
    deepCopyField(m_last_update_rvalue,  copies);

}

void ObservationWindow::declareOptions(OptionList& ol)
{
    // ### Declare all of this object's options here.
    // ### For the "flags" of each option, you should typically specify
    // ### one of OptionBase::buildoption, OptionBase::learntoption or
    // ### OptionBase::tuningoption. If you don't provide one of these three,
    // ### this option will be ignored when loading values from a script.
    // ### You can also combine flags, for example with OptionBase::nosave:
    // ### (OptionBase::buildoption | OptionBase::nosave)

    // ### ex:
    // declareOption(ol, "myoption", &ObservationWindow::myoption,
    //               OptionBase::buildoption,
    //               "Help text describing this option");
    // ...

    declareOption(ol, "window", &ObservationWindow::m_window,
                  OptionBase::buildoption,
                  "Window length");

    declareOption(ol, "nobs", &ObservationWindow::m_nobs,
                  OptionBase::learntoption,
                  "nb. observations");

    declareOption(ol, "cursor", &ObservationWindow::m_cursor,
                  OptionBase::learntoption | OptionBase::nosave | OptionBase::remotetransmit,
                  "cursor pos.");

    declareOption(ol, "observations", &ObservationWindow::m_observations,
                  OptionBase::learntoption | OptionBase::nosave | OptionBase::remotetransmit,
                  "the observations themselves");

    declareOption(ol, "obs_weights", &ObservationWindow::m_obs_weights,
                  OptionBase::learntoption | OptionBase::nosave | OptionBase::remotetransmit,
                  "observation weights");

    declareOption(ol, "last_update_rvalue", &ObservationWindow::m_last_update_rvalue,
                  OptionBase::learntoption | OptionBase::nosave | OptionBase::remotetransmit,
                  "last_update_rvalue");

    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void ObservationWindow::build_()
{
    // ### This method should do the real building of the object,
    // ### according to set 'options', in *any* situation.
    // ### Typical situations include:
    // ###  - Initial building of an object from a few user-specified options
    // ###  - Building of a "reloaded" object: i.e. from the complete set of
    // ###    all serialised options.
    // ###  - Updating or "re-building" of an object after a few "tuning"
    // ###    options have been modified.
    // ### You should assume that the parent class' build_() has already been
    // ### called.
}



int ObservationWindow::length() const
{
    return m_observations.length();
}

void ObservationWindow::forget()        
{
    m_nobs = 0;
    m_cursor = 0;
    if(unlimited_size)
        m_window= 0;
    else if(m_window >= 0)
        m_observations.resize(m_window, 0);
    m_observations.resize(0, 0);                
}

tuple<Vec, real>
ObservationWindow::update(const Vec& obs, real weight/*=1.0*/)
{
    Vec outdated;
    real outdated_weight = 0.0;
    
    ++m_nobs;
    if(unlimited_size) m_window= m_nobs;
    m_observations.resize(MIN(m_nobs,m_window), obs.size());
    m_obs_weights.resize(MIN(m_nobs,m_window));
    if (m_nobs > m_window)
    {
        outdated.resize(obs.size());
        outdated << m_observations(m_cursor % m_window);
        outdated_weight = m_obs_weights[m_cursor % m_window];
        m_nobs--;
    }

    m_observations(m_cursor % m_window) << obs;
    m_obs_weights[m_cursor % m_window] = weight;
    m_cursor++;        
    
    PLASSERT( m_nobs <= m_window );
    m_last_update_rvalue = tuple<Vec, real>(outdated, outdated_weight);
    return m_last_update_rvalue;
}

const Vec ObservationWindow::getObs(int t) const
{
    PLASSERT( t < m_window );
    if ( length() < m_window )
        return m_observations(t);

    int obs_index = (m_cursor+t) % m_window;
    return m_observations(obs_index);
}

real ObservationWindow::getObs(int t, int col) const
{
    PLASSERT( t < m_window ); 
    if ( length() < m_window )
        return m_observations(t, col);

    int obs_index = (m_cursor+t) % m_window;
    return m_observations(obs_index, col);
}

real ObservationWindow::getWeight(int t) const
{
    PLASSERT( t < m_window ); 
    if ( length() < m_window )
        return m_obs_weights[t];

    int obs_index = (m_cursor+t) % m_window;
    return m_obs_weights[obs_index];
}

const Vec ObservationWindow::lastObs() const
{
    int last_obs = (m_cursor-1) % m_window;
    return m_observations(last_obs);
}

real ObservationWindow::lastWeight() const
{
    int last_obs = (m_cursor-1) % m_window;
    return m_obs_weights[last_obs];
}

/*
//! Deep copying
ObservationWindow* ObservationWindow::deepCopy(CopiesMap& copies) const
{
    CopiesMap::iterator it = copies.find(this);
    if(it!=copies.end())  //!<  a copy already exists, so return it
        return (ObservationWindow*) it->second;
  
    //!  Otherwise call the copy constructor to obtain a copy
    ObservationWindow* deep_copy = new ObservationWindow(*this);
    deepCopyField(deep_copy->m_observations, copies);
    deepCopyField(deep_copy->m_obs_weights,  copies);

    //!  Put the copy in the map
    copies[this] = deep_copy;

    //!  return the completed deep_copy
    return deep_copy;
}
*/

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
