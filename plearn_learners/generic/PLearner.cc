// -*- C++ -*-

// PLearner.cc
//
// Copyright (C) 1998-2002 Pascal Vincent
// Copyright (C) 1999-2002 Yoshua Bengio, Nicolas Chapados, Charles Dugas, Rejean Ducharme, Universite de Montreal
// Copyright (C) 2001,2002 Francis Pieraut, Jean-Sebastien Senecal
// Copyright (C) 2002 Frederic Morin, Xavier Saint-Mleux, Julien Keable
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

#include "PLearner.h"
#include <plearn/base/stringutils.h>
#include <plearn/io/fileutils.h>
#include <plearn/io/pl_log.h>
#include <plearn/vmat/FileVMatrix.h>
#include <plearn/misc/PLearnService.h>
#include <plearn/misc/RemotePLearnServer.h>

namespace PLearn {
using namespace std;

PLearner::PLearner()
    : n_train_costs_(-1),
      n_test_costs_(-1),
      seed_(-1), 
      stage(0),
      nstages(1),
      report_progress(true),
      verbosity(1),
      nservers(0),
      save_trainingset_prefix(""),
      inputsize_(-1),
      targetsize_(-1),
      weightsize_(-1),
      n_examples(-1),
      forget_when_training_set_changes(false)  
{}

PLEARN_IMPLEMENT_ABSTRACT_OBJECT(PLearner,
                                 "The base class for all PLearn learners.",
                                 ""
    );

void PLearner::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);
    deepCopyField(tmp_output, copies);
    // TODO What's wrong with this?
    deepCopyField(train_set, copies);
    deepCopyField(validation_set, copies);
    deepCopyField(train_stats, copies);
}

void PLearner::declareOptions(OptionList& ol)
{
    declareOption(ol, "expdir", &PLearner::expdir, OptionBase::buildoption, 
                  "Path of the directory associated with this learner, in which\n"
                  "it should save any file it wishes to create. \n"
                  "The directory will be created if it does not already exist.\n"
                  "If expdir is the empty string (the default), then the learner \n"
                  "should not create *any* file. Note that, anyway, most file creation and \n"
                  "reporting are handled at the level of the PTester class rather than \n"
                  "at the learner's. \n");

    declareOption(ol, "seed", &PLearner::seed_, OptionBase::buildoption, 
                  "The initial seed for the random number generator used to initialize this learner's parameters\n"
                  "as typically done in the forget() method... \n"
                  "If -1 is provided, then a 'random' seed is chosen based on time of day, insuring that\n"
                  "different experiments may yield different results.\n"
                  "With a given seed, forget() should always initialize the parameters to the same values.");

    declareOption(ol, "stage", &PLearner::stage, OptionBase::learntoption, 
                  "The current training stage, since last fresh initialization (forget()): \n"
                  "0 means untrained, n often means after n epochs or optimization steps, etc...\n"
                  "The true meaning is learner-dependant."
                  "You should never modify this option directly!"
                  "It is the role of forget() to bring it back to 0,\n"
                  "and the role of train() to bring it up to 'nstages'...");

    declareOption(ol, "n_examples", &PLearner::n_examples, OptionBase::learntoption, 
                  "The number of samples in the training set.\n"
                  "Obtained from training set with setTrainingSet.");

    declareOption(ol, "inputsize", &PLearner::inputsize_, OptionBase::learntoption, 
                  "The number of input columns in the data sets."
                  "Obtained from training set with setTrainingSet.");

    declareOption(ol, "targetsize", &PLearner::targetsize_, OptionBase::learntoption, 
                  "The number of target columns in the data sets."
                  "Obtained from training set with setTrainingSet.");

    declareOption(ol, "weightsize", &PLearner::weightsize_, OptionBase::learntoption, 
                  "The number of cost weight columns in the data sets."
                  "Obtained from training set with setTrainingSet.");

    declareOption(ol, "forget_when_training_set_changes", &PLearner::forget_when_training_set_changes, OptionBase::buildoption, 
                  "Whether or not to call the forget() method (re-initialize model as before training) in setTrainingSet when the\n"
                  "training set changes (e.g. of dimension).");

    declareOption(ol, "nstages", &PLearner::nstages, OptionBase::buildoption, 
                  "The stage until which train() should train this learner and return.\n"
                  "The meaning of 'stage' is learner-dependent, but for learners whose \n"
                  "training is incremental (such as involving incremental optimization), \n"
                  "it is typically synonym with the number of 'epochs', i.e. the number \n"
                  "of passages of the optimization process through the whole training set, \n"
                  "since the last fresh initialisation.");

    declareOption(ol, "report_progress", &PLearner::report_progress, OptionBase::buildoption, 
                  "should progress in learning and testing be reported in a ProgressBar.\n");

    declareOption(ol, "verbosity", &PLearner::verbosity, OptionBase::buildoption, 
                  "Level of verbosity. If 0 should not write anything on perr. \n"
                  "If >0 may write some info on the steps performed along the way.\n"
                  "The level of details written should depend on this value.");

    declareOption(ol, "nservers", &PLearner::nservers, OptionBase::buildoption, 
                  "Max number of computation servers to use in parallel with the main process.\n"
                  "If <=0 no parallelization will occur at this level.\n");

    declareOption(ol, "save_trainingset_prefix", &PLearner::save_trainingset_prefix,
                  OptionBase::buildoption,
                  "Whether the training set should be saved upon a call to\n"
                  "setTrainingSet().  The saved file is put in the learner's expdir\n"
                  "(assuming there is one) and has the form \"<prefix>_trainset_XXX.pmat\"\n"
                  "The prefix is what this option specifies.  'XXX' is a unique\n"
                  "serial number that is globally incremented with each saved\n"
                  "setTrainingSet.  This option is useful when manipulating very\n"
                  "complex nested learner structures, and you want to ensure that\n"
                  "the inner learner is getting the correct results.  (Default="",\n"
                  "i.e. don't save anything.)\n");
  
    inherited::declareOptions(ol);
}

////////////////////////////
// setExperimentDirectory //
////////////////////////////
void PLearner::setExperimentDirectory(const PPath& the_expdir) 
{ 
    if(the_expdir=="")
        expdir = "";
    else
    {
        if(!force_mkdir(the_expdir))
            PLERROR("In PLearner::setExperimentDirectory Could not create experiment directory %s",
                    the_expdir.absolute().c_str());
        expdir = the_expdir / "";
    }
}

void PLearner::setTrainingSet(VMat training_set, bool call_forget)
{ 
    // YB: je ne suis pas sur qu'il soit necessaire de faire un build si la
    // LONGUEUR du train_set a change?  les methodes non-parametriques qui
    // utilisent la longueur devrait faire leur "resize" dans train, pas dans
    // build.
    bool training_set_has_changed = !train_set || !(train_set->looksTheSameAs(training_set));
    train_set = training_set;
    if (training_set_has_changed)
    {
        inputsize_ = train_set->inputsize();
        targetsize_ = train_set->targetsize();
        weightsize_ = train_set->weightsize();
        if (forget_when_training_set_changes)
            call_forget=true;
    }
    n_examples = train_set->length();
    if (training_set_has_changed || call_forget)
        build(); // MODIF FAITE PAR YOSHUA: sinon apres un setTrainingSet le build n'est pas complete dans un NNet train_set = training_set;
    if (call_forget)
        forget();

    // Save the new training set if desired
    if (save_trainingset_prefix != "" && expdir != "") {
        static int trainingset_serial = 1;
        PPath fname = expdir / (save_trainingset_prefix + "_trainset_" +
                                tostring(trainingset_serial++) + ".pmat");
        train_set->savePMAT(fname);
    }
}

void PLearner::setValidationSet(VMat validset)
{ validation_set = validset; }


void PLearner::setTrainStatsCollector(PP<VecStatsCollector> statscol)
{ train_stats = statscol; }


int PLearner::inputsize() const
{ 
    if (inputsize_<0)
        PLERROR("Must specify a training set before calling PLearner::inputsize()"); 
    return inputsize_; 
}

int PLearner::targetsize() const 
{ 
    if(targetsize_ == -1) 
        PLERROR("In PLearner::targetsize - 'targetsize_' is -1, either no training set has beeen specified or its sizes were not set properly");
    return targetsize_; 
}

int PLearner::weightsize() const 
{ 
    if(weightsize_ == -1) 
        PLERROR("In PLearner::weightsize - 'weightsize_' is -1, either no training set has beeen specified or its sizes were not set properly");
    return weightsize_; 
}

void PLearner::build_()
{
    if(expdir!="")
    {
        if(!force_mkdir(expdir))
            PLERROR("In PLearner Could not create experiment directory %s",expdir.c_str());
        expdir = expdir.absolute() / "";
    }
}

void PLearner::build()
{
    inherited::build();
    build_();
}

PLearner::~PLearner()
{
}

int PLearner::nTestCosts() const 
{ 
    if(n_test_costs_<0)
        n_test_costs_ = getTestCostNames().size(); 
    return n_test_costs_;
}

int PLearner::nTrainCosts() const 
{ 
    if(n_train_costs_<0)
        n_train_costs_ = getTrainCostNames().size();
    return n_train_costs_; 
}

int PLearner::getTestCostIndex(const string& costname) const
{
    TVec<string> costnames = getTestCostNames();
    for(int i=0; i<costnames.length(); i++)
        if(costnames[i]==costname)
            return i;
    PLERROR("In PLearner::getTestCostIndex, No test cost named %s in this learner.\n"
            "Available test costs are: %s", costname.c_str(),
            tostring(costnames).c_str());
    return -1;
}

int PLearner::getTrainCostIndex(const string& costname) const
{
    TVec<string> costnames = getTrainCostNames();
    for(int i=0; i<costnames.length(); i++)
        if(costnames[i]==costname)
            return i;
    PLERROR("In PLearner::getTrainCostIndex, No train cost named %s in this learner.\n"
            "Available train costs are: %s", costname.c_str(), tostring(costnames).c_str());
    return -1;
}
                                
void PLearner::computeOutputAndCosts(const Vec& input, const Vec& target, 
                                     Vec& output, Vec& costs) const
{
    computeOutput(input, output);
    computeCostsFromOutputs(input, output, target, costs);
}

void PLearner::computeCostsOnly(const Vec& input, const Vec& target,  
                                Vec& costs) const
{
    tmp_output.resize(outputsize());
    computeOutputAndCosts(input, target, tmp_output, costs);
}

bool PLearner::computeConfidenceFromOutput(
    const Vec& input, const Vec& output,
    real probability,
    TVec< pair<real,real> >& intervals) const
{
    // Default version does not know how to compute confidence intervals
    intervals.resize(output.size());
    intervals.fill(std::make_pair(MISSING_VALUE,MISSING_VALUE));  
    return false;
}

void PLearner::batchComputeOutputAndConfidence(VMat inputs, real probability, VMat outputs_and_confidence) const
{
    Vec input(inputsize());
    Vec output(outputsize());
    int outsize = outputsize();
    Vec output_and_confidence(3*outsize);
    TVec< pair<real,real> > intervals;
    int l = inputs.length();
    for(int i=0; i<l; i++)
    {
        inputs->getRow(i,input);
        computeOutput(input,output);
        computeConfidenceFromOutput(input,output,probability,intervals);
        for(int j=0; j<outsize; j++)
        {
            output_and_confidence[3*j] = output[j];
            output_and_confidence[3*j+1] = intervals[j].first;
            output_and_confidence[3*j+2] = intervals[j].second;
        }
        outputs_and_confidence->putOrAppendRow(i,output_and_confidence);
    }
}

/////////
// use //
/////////
void PLearner::use(VMat testset, VMat outputs) const
{
    int l = testset.length();
    int w = testset.width();

    TVec< PP<RemotePLearnServer> > servers;
    if(nservers>0)
        servers = PLearnService::instance().reserveServers(nservers);

    if(servers.length()==0) 
    { // sequential code      
        Vec input;
        Vec target;
        real weight;
        Vec output(outputsize());

        ProgressBar* pb = NULL;
        if(report_progress)
            pb = new ProgressBar("Using learner",l);

        for(int i=0; i<l; i++)
        {
            testset.getExample(i, input, target, weight);
            computeOutput(input, output);
            outputs->putOrAppendRow(i,output);
            if(pb)
                pb->update(i);
        }

        if(pb)
            delete pb;
    }
    else // parallel code
    {
        int n = servers.length(); // number of allocated servers
        DBG_LOG << "PLearner::use parallel code using " << n << " servers" << endl;
        for(int k=0; k<n; k++)  // send this object with objid 0
            servers[k]->newObject(0, *this);
        int chunksize = l/n;
        if(chunksize*n<l)
            ++chunksize;
        if(chunksize*w>1000000) // max 1 Mega elements
            chunksize = max(1,1000000/w);
        Mat chunk(chunksize,w);
        int send_i=0;
        Mat outmat;
        int receive_i = 0;
        while(send_i<l)
        {
            for(int k=0; k<n && send_i<l; k++)
            {
                int actualchunksize = chunksize;
                if(send_i+actualchunksize>l)
                    actualchunksize = l-send_i;
                chunk.resize(actualchunksize,w);
                testset->getMat(send_i, 0, chunk);
                VMat inputs(chunk);
                inputs->copySizesFrom(testset);
                DBG_LOG << "PLearner::use calling use2 remote method with chunk starting at " 
                        << send_i << " of length " << actualchunksize << ":" << inputs << endl;
                servers[k]->callMethod(0,"use2",inputs);
                send_i += actualchunksize;
            }
            for(int k=0; k<n && receive_i<l; k++)
            {
                outmat.resize(0,0);
                servers[k]->getResults(outmat);
                for(int ii=0; ii<outmat.length(); ii++)
                    outputs->putOrAppendRow(receive_i++,outmat(ii));
            }
        }
        if(send_i!=l || receive_i!=l)
            PLERROR("In PLearn::use parallel execution failed to complete successfully.");
    }
}



TVec<string> PLearner::getOutputNames() const
{
    int n = outputsize();
    TVec<string> outnames(n);
    char tmp[21];
    tmp[20] = '\0';
    for(int k=0; k<n; k++)
    {
        snprintf(tmp,20,"out%d",k);
        outnames[n] = tmp;
    }
    return outnames;
}

////////////////
// useOnTrain //
////////////////
void PLearner::useOnTrain(Mat& outputs) const {
    // NC declares this method to be tested...
    // PLWARNING("In PLearner::useOnTrain - This method has not been tested yet, remove this warning if it works fine");
    VMat train_output(outputs);
    use(train_set, train_output);
}

//////////
// test //
//////////
void PLearner::test(VMat testset, PP<VecStatsCollector> test_stats, 
                    VMat testoutputs, VMat testcosts) const
{
    int l = testset.length();
    Vec input;
    Vec target;
    real weight;

    Vec output(outputsize());

    Vec costs(nTestCosts());

    // testset->defineSizes(inputsize(),targetsize(),weightsize());

    ProgressBar* pb = NULL;
    if(report_progress) 
        pb = new ProgressBar("Testing learner",l);

    if (l == 0) {
        // Empty test set: we give -1 cost arbitrarily.
        costs.fill(-1);
        test_stats->update(costs);
    }

    for(int i=0; i<l; i++)
    {
        testset.getExample(i, input, target, weight);
      
        // Always call computeOutputAndCosts, since this is better
        // behaved with stateful learners
        computeOutputAndCosts(input,target,output,costs);
      
        if(testoutputs)
            testoutputs->putOrAppendRow(i,output);

        if(testcosts)
            testcosts->putOrAppendRow(i, costs);

        if(test_stats)
            test_stats->update(costs,weight);

        if(report_progress)
            pb->update(i);
    }

    if(pb)
        delete pb;

}

void PLearner::resetInternalState()
{}

bool PLearner::isStatefulLearner() const
{ return false; }

void PLearner::call(const string& methodname, int nargs, PStream& io)
{
    if(methodname=="setTrainingSet")
    {
        if(nargs!=2) PLERROR("PLearner remote method setTrainingSet takes 2 argument");
        VMat training_set;
        bool call_forget;
        io >> training_set >> call_forget;
        setTrainingSet(training_set, call_forget);
        prepareToSendResults(io, 0);
        io.flush();
    }
    else if(methodname=="setExperimentDirectory")
    {
        if(nargs!=1) PLERROR("PLearner remote method setExperimentDirectory takes 1 argument");
        PPath the_expdir;
        io >> the_expdir;
        setExperimentDirectory(the_expdir);
        prepareToSendResults(io, 0);
        io.flush();      
    }
    else if(methodname=="getExperimentDirectory")
    {
        if(nargs!=0) PLERROR("PLearner remote method getExperimentDirectory takes 0 arguments");
        PPath result = getExperimentDirectory();
        prepareToSendResults(io, 1);
        io << result;
        io.flush();      
    }
    else if(methodname=="forget")
    {
        if(nargs!=0) PLERROR("PLearner remote method forget takes 0 arguments");
        forget();
        prepareToSendResults(io, 0);
        io.flush();      
    }
    else if(methodname=="train")
    {
        if(nargs!=0) PLERROR("PLearner remote method train takes 0 arguments");
        train();
        prepareToSendResults(io, 0);
        io.flush();      
    }
    else if(methodname=="resetInternalState")
    {
        if(nargs!=0) PLERROR("PLearner remote method resetInternalState takes 0 arguments");
        resetInternalState();
        prepareToSendResults(io, 0);
        io.flush();
    }
    else if(methodname=="computeOutput")
    {
        if(nargs!=1) PLERROR("PLearner remote method computeOutput takes 1 argument");
        Vec input;
        io >> input;
        tmp_output.resize(outputsize());
        computeOutput(input,tmp_output);
        prepareToSendResults(io, 1);
        io << tmp_output;
        io.flush();    
    }
    else if(methodname=="use") // use inputs_vmat output_pmat_fname --> void
    {
        if(nargs!=2) PLERROR("PLearner remote method use requires 2 argument");
        VMat inputs;
        string output_fname;
        io >> inputs >> output_fname;
        VMat outputs = new FileVMatrix(output_fname, inputs.length(), outputsize());
        use(inputs,outputs);
        prepareToSendResults(io, 0);
        io.flush();      
    }
    else if(methodname=="use2") // use inputs_vmat --> outputs
    {
        if(nargs!=1) PLERROR("PLearner remote method use2 requires 1 argument");
        VMat inputs;
        io >> inputs;
        // DBG_LOG << " Arg0 = " << inputs << endl;
        Mat outputs(inputs.length(),outputsize());
        use(inputs,outputs);
        prepareToSendResults(io, 1);
        io << outputs;
        io.flush();      
    }
    else if(methodname=="computeOutputAndCosts")
    {
        if(nargs!=2) PLERROR("PLearner remote method computeOutputAndCosts takes 2 arguments");
        Vec input, target;
        io >> input >> target;
        tmp_output.resize(outputsize());
        Vec costs(nTestCosts());
        computeOutputAndCosts(input,target,tmp_output,costs);
        prepareToSendResults(io, 2);
        io << tmp_output << costs;
        io.flush();
    }
    else if(methodname=="computeCostsFromOutputs")
    {
        if(nargs!=3) PLERROR("PLearner remote method computeCostsFromOutputs takes 3 arguments");
        Vec input, output, target;
        io >> input >> output >> target;
        Vec costs;
        computeCostsFromOutputs(input,output,target,costs);
        prepareToSendResults(io, 1);
        io << costs;
        io.flush();
    }
    else if(methodname=="computeCostsOnly")
    {
        if(nargs!=3) PLERROR("PLearner remote method computeCostsOnly takes 3 arguments");
        Vec input, target;
        io >> input >> target;
        Vec costs(nTestCosts());
        computeCostsOnly(input,target,costs);
        prepareToSendResults(io, 1);
        io << costs;
        io.flush();
    }
    else if(methodname=="computeConfidenceFromOutput")
    {
        if(nargs!=3) PLERROR("PLearner remote method computeConfidenceFromOutput takes 3 arguments: input, output, probability");
        Vec input, output;
        real probability;
        io >> input >> output >> probability;
      
        TVec< pair<real,real> > intervals(output.length());
        bool ok = computeConfidenceFromOutput(input, output, probability, intervals);
        prepareToSendResults(io, 2);
        io << ok << intervals;
        io.flush();
    }
    else if(methodname=="batchComputeOutputAndConfidencePMat") // input_vmat probability result_pmat_filename
    {
        if(nargs!=3) 
            PLERROR("PLearner remote method batchComputeOutputAndConfidencePMat takes 3 arguments:\n"
                    "input_vmat, probability, result_pmat_filename");
        VMat inputs;
        real probability;
        string pmat_fname;
        io >> inputs >> probability >> pmat_fname;
        TVec<string> fieldnames;
        for(int j=0; j<outputsize(); j++)
        {
            fieldnames.append("output_"+tostring2(j));
            fieldnames.append("low_"+tostring2(j));
            fieldnames.append("high_"+tostring2(j));
        }
        VMat out_and_conf = new FileVMatrix(pmat_fname,inputs.length(),fieldnames);
        batchComputeOutputAndConfidence(inputs, probability, out_and_conf);
        prepareToSendResults(io,0);
        io.flush();
    }  
    else if(methodname=="getTestCostNames")
    {
        if(nargs!=0) PLERROR("PLearner remote method getTestCostNames takes 0 arguments");
        TVec<string> result = getTestCostNames();
        prepareToSendResults(io, 1);
        io << result;
        io.flush();     
    }
    else if(methodname=="getTrainCostNames")
    {
        if(nargs!=0) PLERROR("PLearner remote method getTrainCostNames takes 0 arguments");
        TVec<string> result = getTrainCostNames();
        prepareToSendResults(io, 1);
        io << result;
        io.flush();     
    }
    else
        inherited::call(methodname, nargs, io);
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
