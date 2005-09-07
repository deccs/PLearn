// -*- C++ -*-

// Train.cc
//
// Copyright (C) 2004 Martin Monperrus 
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

// Authors: Martin Monperrus

/*! \file Train.cc */


#include "Train.h"
#include <plearn/db/getDataSet.h>
#include <plearn/io/load_and_save.h>

namespace PLearn {
using namespace std;

Train::Train() 
    /* ### Initialize all fields to their default value */
{
    // ...

    // ### You may or may not want to call build_() to finish building the object
    // build_();
}

PLEARN_IMPLEMENT_OBJECT(Train, "An easy PTester", "Sometimes, we want to train a learner easily.\n"
                        "But plearn learner train does not permit to use macro variable.\n"
                        "Train is the equivalent of plearn learner train but macro variables are usable\n"
                        "Example: plearn myTrain.plearn knn=12");

void Train::declareOptions(OptionList& ol)
{
    // ### Declare all of this object's options here
    // ### For the "flags" of each option, you should typically specify  
    // ### one of OptionBase::buildoption, OptionBase::learntoption or 
    // ### OptionBase::tuningoption. Another possible flag to be combined with
    // ### is OptionBase::nosave

    // ### ex:
    // declareOption(ol, "myoption", &Train::myoption, OptionBase::buildoption,
    //               "Help text describing this option");
    // ...
    declareOption(ol, "dataset", &Train::trainset, OptionBase::buildoption,
                  "Any valid vmat");

    declareOption(ol, "psave", &Train::psave, OptionBase::buildoption,
                  "Name of the file .psave");
  
    declareOption(ol, "learner", &Train::learner, OptionBase::buildoption,
                  "Any valid learner");

    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void Train::build_()
{
    // ### This method should do the real building of the object,
    // ### according to set 'options', in *any* situation. 
    // ### Typical situations include:
    // ###  - Initial building of an object from a few user-specified options
    // ###  - Building of a "reloaded" object: i.e. from the complete set of all serialised options.
    // ###  - Updating or "re-building" of an object after a few "tuning" options have been modified.
    // ### You should assume that the parent class' build_() has already been called.
}

// ### Nothing to add here, simply calls build_
void Train::build()
{
    inherited::build();
    build_();
}

void Train::run()
{
    PP<VecStatsCollector> train_stats = new VecStatsCollector();
    learner->setTrainStatsCollector(train_stats);
    learner->setTrainingSet(trainset);
    learner->train();
    cout<<"Saving learner in :"<<psave<<endl;
    PLearn::save(psave, learner);
}

void Train::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    // ### Call deepCopyField on all "pointer-like" fields 
    // ### that you wish to be deepCopied rather than 
    // ### shallow-copied.
    // ### ex:
    // deepCopyField(trainvec, copies);

    // ### Remove this line when you have fully implemented this method.
    PLERROR("Train::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
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
