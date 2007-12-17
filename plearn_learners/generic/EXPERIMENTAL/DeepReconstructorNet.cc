// -*- C++ -*-

// DeepReconstructorNet.cc
//
// Copyright (C) 2007 Simon Lemieux, Pascal Vincent
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

// Authors: Simon Lemieux, Pascal Vincent

/*! \file DeepReconstructorNet.cc */


#include "DeepReconstructorNet.h"
#include <plearn/display/DisplayUtils.h>
#include <plearn/var/Var_operators.h>
#include <plearn/vmat/ConcatColumnsVMatrix.h>
#include <plearn/var/ConcatColumnsVariable.h>
#include <plearn/io/load_and_save.h>

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(
   DeepReconstructorNet,
   "ONE LINE DESCRIPTION",
   "MULTI-LINE \nHELP");

DeepReconstructorNet::DeepReconstructorNet()
    :supervised_nepochs(pair<int,int>(0,0)),
     supervised_min_improvement_rate(-10000),
     minibatch_size(1)
{
}

void DeepReconstructorNet::declareOptions(OptionList& ol)
{
    // ### Declare all of this object's options here.
    // ### For the "flags" of each option, you should typically specify
    // ### one of OptionBase::buildoption, OptionBase::learntoption or
    // ### OptionBase::tuningoption. If you don't provide one of these three,
    // ### this option will be ignored when loading values from a script.
    // ### You can also combine flags, for example with OptionBase::nosave:
    // ### (OptionBase::buildoption | OptionBase::nosave)

    
    declareOption(ol, "unsupervised_nepochs", &DeepReconstructorNet::unsupervised_nepochs,
                  OptionBase::buildoption,
                  "unsupervised_nepochs[k] contains a pair of integers giving the minimum and\n"
                  "maximum number of epochs for the training of layer k+1 (taking layer k"
                  "as input). Thus k=0 corresponds to the training of the first hidden layer.");

    declareOption(ol, "unsupervised_min_improvement_rate", &DeepReconstructorNet::unsupervised_min_improvement_rate,
                  OptionBase::buildoption,
                  "unsupervised_min_improvement_rate[k] should contain the minimum required relative improvement rate\n"
                  "for the training of layer k+1 (taking input from layer k.)");

    declareOption(ol, "supervised_nepochs", &DeepReconstructorNet::supervised_nepochs,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "supervised_min_improvement_rate", &DeepReconstructorNet::supervised_min_improvement_rate,
                  OptionBase::buildoption,
                  "supervised_min_improvement_rate contains the minimum required relative improvement rate\n"
                  "for the training of the supervised layer.");

    declareOption(ol, "layers", &DeepReconstructorNet::layers,
                  OptionBase::buildoption,
                  "layers[0] is the input variable ; last layer is final output layer");

    declareOption(ol, "reconstruction_costs", &DeepReconstructorNet::reconstruction_costs,
                  OptionBase::buildoption,
                  "recontruction_costs[k] is the reconstruction cost for layer[k]");

    declareOption(ol, "reconstructed_layers", &DeepReconstructorNet::reconstructed_layers,
                  OptionBase::buildoption,
                  "reconstructed_layers[k] is the reconstruction of layer k from layers[k+1]");

    declareOption(ol, "reconstruction_optimizers", &DeepReconstructorNet::reconstruction_optimizers,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "reconstruction_optimizer", &DeepReconstructorNet::reconstruction_optimizer,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "target", &DeepReconstructorNet::target,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "supervised_costs", &DeepReconstructorNet::supervised_costs,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "supervised_costvec", &DeepReconstructorNet::supervised_costvec,
                  OptionBase::learntoption,
                  "");

    declareOption(ol, "supervised_costs_names", &DeepReconstructorNet::supervised_costs_names,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "minibatch_size", &DeepReconstructorNet::minibatch_size,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "supervised_optimizer", &DeepReconstructorNet::supervised_optimizer,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "fine_tuning_optimizer", &DeepReconstructorNet::fine_tuning_optimizer,
                  OptionBase::buildoption,
                  "");

    declareOption(ol, "group_sizes", &DeepReconstructorNet::group_sizes,
                  OptionBase::buildoption,
                  "");
    
    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);
}

void DeepReconstructorNet::declareMethods(RemoteMethodMap& rmm)
{
    rmm.inherited(inherited::_getRemoteMethodMap_());

    declareMethod(rmm,
                  "getParameterValue",
                  &DeepReconstructorNet::getParameterValue,
                  (BodyDoc("Returns the matValue of the parameter variable with the given name"),
                   ArgDoc("varname", "name of the variable searched for"),
                   RetDoc("Returns the value of the parameter as a Mat")));

    declareMethod(rmm,
                  "getParameterRow",
                  &DeepReconstructorNet::getParameterRow,
                  (BodyDoc("Returns the matValue of the parameter variable with the given name"),
                   ArgDoc("varname", "name of the variable searched for"),
                   ArgDoc("n", "row number"),
                   RetDoc("Returns the nth row of the value of the parameter as a Mat")));



    declareMethod(rmm,
                  "listParameterNames",
                  &DeepReconstructorNet::listParameterNames,
                  (BodyDoc("Returns a list of the names of the parameters"),
                   RetDoc("Returns a list of the names of the parameters")));

    declareMethod(rmm,
                  "listParameter",
                  &DeepReconstructorNet::listParameter,
                  (BodyDoc("Returns a list of the parameters"),
                   RetDoc("Returns a list of the names")));

    declareMethod(rmm,
                  "computeRepresentations",
                  &DeepReconstructorNet::computeRepresentations,
                  (BodyDoc("Compute the representation of each hidden layer"),
                   ArgDoc("input", "the input"),
                   RetDoc("The representations")));

    declareMethod(rmm,
                  "computeReconstructions",
                  &DeepReconstructorNet::computeReconstructions,
                  (BodyDoc("Compute the reconstructions of the input of each hidden layer"),
                   ArgDoc("input", "the input"),
                   RetDoc("The reconstructions")));

    declareMethod(rmm,
                   "getMatValue",
                   &DeepReconstructorNet::getMatValue,
                   (BodyDoc(""),
                    ArgDoc("layer", "no of the layer"),
                    RetDoc("the matValue")));

    declareMethod(rmm,
                   "setMatValue",
                   &DeepReconstructorNet::setMatValue,
                   (BodyDoc(""),
                    ArgDoc("layer", "no of the layer"),
                    ArgDoc("values", "the values")));

    declareMethod(rmm,
                   "fpropOneLayer",
                   &DeepReconstructorNet::fpropOneLayer,
                   (BodyDoc(""),
                    ArgDoc("layer", "no of the layer"),
                    RetDoc("")));


    declareMethod(rmm,
                   "reconstructOneLayer",
                   &DeepReconstructorNet::reconstructOneLayer,
                   (BodyDoc(""),
                    ArgDoc("layer", "no of the layer"),
                    RetDoc("")));
}

void DeepReconstructorNet::build_()
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
    
    int nlayers = layers.length();
    compute_layer.resize(nlayers-1);
    for(int k=0; k<nlayers-1; k++)
        compute_layer[k] = Func(layers[k], layers[k+1]);
    compute_output = Func(layers[0], layers[nlayers-1]);
    nout = layers[nlayers-1]->size();

    output_and_target_to_cost = Func(layers[nlayers-1]&target, supervised_costvec); 


    if(supervised_costs.isNull())
        PLERROR("You must provide a supervised_cost");

    supervised_costvec = hconcat(supervised_costs);

    if(supervised_costs.length()>0)
        fullcost += supervised_costs[0];
    for(int i=1; i<supervised_costs.length(); i++)
        fullcost += supervised_costs[i];
    
    int n_rec_costs = reconstruction_costs.length();
    for(int k=0; k<n_rec_costs; k++)
        fullcost += reconstruction_costs[k];
    //displayVarGraph(fullcost);
    Var input = layers[0];
    Func f(input&target, fullcost);
    parameters = f->parameters;
    outmat.resize(n_rec_costs);
}

// ### Nothing to add here, simply calls build_
void DeepReconstructorNet::build()
{
    if(random_gen.isNull())
        random_gen = new PRandom();
    inherited::build();
    build_();
}

void DeepReconstructorNet::initializeParams(bool set_seed)
{
    perr << "Initializing parameters..." << endl;
    if (set_seed && seed_ != 0)
        random_gen->manual_seed(seed_);

    for(int i=0; i<parameters.length(); i++)
        dynamic_cast<SourceVariable*>((Variable*)parameters[i])->randomInitialize(random_gen);
}


void DeepReconstructorNet::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    deepCopyField(unsupervised_nepochs, copies);
    deepCopyField(unsupervised_min_improvement_rate, copies);
    deepCopyField(supervised_nepochs, copies);
    deepCopyField(supervised_min_improvement_rate, copies);

    deepCopyField(layers, copies);
    deepCopyField(reconstruction_costs, copies);
    deepCopyField(reconstructed_layers, copies);
    deepCopyField(reconstruction_optimizers, copies);
    deepCopyField(reconstruction_optimizer, copies);
    varDeepCopyField(target, copies);
    deepCopyField(supervised_costs, copies);
    varDeepCopyField(supervised_costvec, copies);
    deepCopyField(supervised_costs_names, copies);
    varDeepCopyField(fullcost, copies);    
    deepCopyField(parameters,copies);
    deepCopyField(supervised_optimizer, copies);
    deepCopyField(fine_tuning_optimizer, copies);

    deepCopyField(compute_layer, copies);
    deepCopyField(compute_output, copies);
    deepCopyField(output_and_target_to_cost, copies);
    deepCopyField(outmat, copies);
    deepCopyField(group_sizes, copies);
}

int DeepReconstructorNet::outputsize() const
{
    // Compute and return the size of this learner's output (which typically
    // may depend on its inputsize(), targetsize() and set options).

    //TODO : retourner la bonne chose ici
    return 0;
}

void DeepReconstructorNet::forget()
{
    //! (Re-)initialize the PLearner in its fresh state (that state may depend
    //! on the 'seed' option) and sets 'stage' back to 0 (this is the stage of
    //! a fresh learner!)
    /*!
      A typical forget() method should do the following:
      - call inherited::forget() to initialize its random number generator
        with the 'seed' option
      - initialize the learner's parameters, using this random generator
      - stage = 0
    */    
    inherited::forget();
    initializeParams();    
}

void DeepReconstructorNet::train()
{
    // The role of the train method is to bring the learner up to
    // stage==nstages, updating train_stats with training costs measured
    // on-line in the process.

    // This generic PLearner method does a number of standard stuff useful for
    // (almost) any learner, and return 'false' if no training should take
    // place. See PLearner.h for more details.
    if (!initTrain())
        return;

    while(stage<nstages)
    {
        if(stage<1)
        {
            PPath outmatfname = expdir/"outmat";

            int nreconstructions = reconstruction_costs.length();
            int insize = train_set->inputsize();
            VMat inputs = train_set.subMatColumns(0,insize);
            VMat targets = train_set.subMatColumns(insize, train_set->targetsize());
            VMat dset = inputs;

            bool must_train_supervised_layer = supervised_nepochs.second>0;
            
            PLearn::save(expdir/"learner.psave", this);
            for(int k=0; k<nreconstructions; k++)
            {
                trainHiddenLayer(k, dset);
                PLearn::save(expdir/"learner.psave", this);
                // 'if' is a hack to avoid precomputing last hidden layer if not needed
                if(k<nreconstructions-1 ||  must_train_supervised_layer) 
                { 
                    int width = layers[k+1].width();
                    outmat[k] = new FileVMatrix(outmatfname+tostring(k+1)+".pmat",0,width);
                    outmat[k]->defineSizes(width,0);
                    buildHiddenLayerOutputs(k, dset, outmat[k]);
                    dset = outmat[k];
                }
            }

            if(must_train_supervised_layer)
            {
                trainSupervisedLayer(dset, targets);
                PLearn::save(expdir/"learner.psave", this);
            }
            perr << "\n\n*********************************************" << endl;
            perr << "****      Now performing fine tuning     ****" << endl;
            perr << "********************************************* \n" << endl;

        }
        else
        {
            perr << "+++ Fine tuning stage " << stage+1 << " **" << endl;
            prepareForFineTuning();
            fineTuningFor1Epoch();
        }
        ++stage;
        train_stats->finalize(); // finalize statistics for this epoch
    }
    /*
    while(stage<nstages)
    {        
        // clear statistics of previous epoch
        train_stats->forget();

        //... train for 1 stage, and update train_stats,
        // using train_set->getExample(input, target, weight)
        // and train_stats->update(train_costs)

        ++stage;
        train_stats->finalize(); // finalize statistics for this epoch
    }
    */
}

void DeepReconstructorNet::buildHiddenLayerOutputs(int which_input_layer, VMat inputs, VMat outputs)
{
    int l = inputs.length();
    Vec in;
    Vec target;
    real weight;
    Func f = compute_layer[which_input_layer];
    Vec out(f->outputsize);
    for(int i=0; i<l; i++)
    {
        inputs->getExample(i,in,target,weight);
        f->fprop(in, out);
        outputs->putOrAppendRow(i,out);
    }
}

void DeepReconstructorNet::prepareForFineTuning()
{
    Func f(layers[0]&target, supervised_costvec);
    Var totalcost = sumOf(train_set, f, minibatch_size);
    // displayVarGraph(supervised_costvec);
    // displayVarGraph(totalcost);

    VarArray params = totalcost->parents();
    supervised_optimizer->setToOptimize(params, totalcost);
}


TVec<Mat> DeepReconstructorNet::computeRepresentations(Mat input)
{
    int nlayers = layers.length();
    TVec<Mat> representations(nlayers);
    VarArray proppath = propagationPath(layers[0],layers[nlayers-1]);
    layers[0]->matValue << input;
    proppath.fprop();
    perr << "Graph for computing representations" << endl;
    // displayVarGraph(proppath,true, 333, "repr");
    for(int k=0; k<nlayers; k++)
        representations[k] = layers[k]->matValue.copy();
    return representations;
}

void DeepReconstructorNet::reconstructInputFromLayer(int layer)
{
    for(int k=layer; k>0; k--)
    {
        VarArray proppath = propagationPath(layers[k],reconstructed_layers[k-1]);
        proppath.fprop();
        perr << "Graph for reconstructing layer " << k-1 << " from layer " << k << endl;
        //displayVarGraph(proppath,true, 333, "reconstr");

        //WARNING MEGA-HACK
        if (reconstructed_layers[k-1].width() == 2*layers[k-1].width())
        {
            Mat temp(layers[k-1].length(), layers[k-1].width());
            for (int n=0; n < layers[k-1].length(); n++)
                for (int i=0; i < layers[k-1].width(); i++)
                    temp(n,i) = reconstructed_layers[k-1]->matValue(n,i*2);
            temp >> layers[k-1]->matValue;
        }        
        //END OF MEGA-HACK
        else
            reconstructed_layers[k-1]->matValue >> layers[k-1]->matValue;
    }
}

TVec<Mat> DeepReconstructorNet::computeReconstructions(Mat input)
{
    int nlayers = layers.length();
    VarArray proppath = propagationPath(layers[0],layers[nlayers-1]);
    layers[0]->matValue << input;
    proppath.fprop();

    TVec<Mat> reconstructions(nlayers-2);
    for(int k=1; k<nlayers-1; k++)
    {
        reconstructInputFromLayer(k);
        reconstructions[k-1] = layers[0]->matValue.copy();
    }
    return reconstructions;
}


void DeepReconstructorNet::fineTuningFor1Epoch()
{
    if(train_stats.isNull())
        train_stats = new VecStatsCollector();

    int l = train_set->length();
    supervised_optimizer->reset();
    supervised_optimizer->nstages = l/minibatch_size;
    supervised_optimizer->optimizeN(*train_stats);
}

/*
void DeepReconstructorNet::fineTuningFullOld()
{
    prepareForFineTuning();

    int l = train_set->length();
    int nepochs = nstages;
    perr << "\n\n*********************************************" << endl;
    perr << "*** Performing fine tuning for max. " << nepochs << " epochs " << endl;
    perr << "*** each epoch has " << l << " examples and " << l/minibatch_size << " optimizer stages (updates)" << endl;

    VecStatsCollector st;
    real prev_mean = -1;
    real relative_improvement = fine_tuning_improvement_rate;
    for(int n=0; n<nepochs && relative_improvement >= fine_tuning_improvement_rate; n++)
    {
        st.forget();
        supervised_optimizer->nstages = l/minibatch_size;
        supervised_optimizer->optimizeN(st);
        const StatsCollector& s = st.getStats(0);
        real m = s.mean();
        perr << "Epoch " << n+1 << " mean error: " << m << " +- " << s.stderror() << endl;
        if(prev_mean>0)
        {
            relative_improvement = ((prev_mean-m)/prev_mean)*100;
            perr << "Relative improvement: " << relative_improvement << " %"<< endl;
        }
        prev_mean = m;
    }
}
*/

void DeepReconstructorNet::trainSupervisedLayer(VMat inputs, VMat targets)
{
    int l = inputs->length();
    pair<int,int> nepochs = supervised_nepochs;
    real min_improvement = supervised_min_improvement_rate;

    int last_hidden_layer = layers.length()-2;
    perr << "\n\n*********************************************" << endl;
    perr << "*** Training only supervised layer for max. " << nepochs.second << " epochs " << endl;
    perr << "*** each epoch has " << l << " examples and " << l/minibatch_size << " optimizer stages (updates)" << endl;

    Func f(layers[last_hidden_layer]&target, supervised_costvec);
    // displayVarGraph(supervised_costvec);
    VMat inputs_targets = hconcat(inputs, targets);
    inputs_targets->defineSizes(inputs.width(),targets.width());

    Var totalcost = sumOf(inputs_targets, f, minibatch_size);
    // displayVarGraph(totalcost);

    VarArray params = totalcost->parents();
    supervised_optimizer->setToOptimize(params, totalcost);
    supervised_optimizer->reset();
    VecStatsCollector st;
    real prev_mean = -1;
    real relative_improvement = 1000;
    for(int n=0; n<nepochs.first || (n<nepochs.second && relative_improvement >= min_improvement); n++)
    {
        st.forget();
        supervised_optimizer->nstages = l/minibatch_size;
        supervised_optimizer->optimizeN(st);
        const StatsCollector& s = st.getStats(0);
        real m = s.mean();
        perr << "Epoch " << n+1 << " Mean costs: " << st.getMean() << " stderr " << st.getStdError() << endl;
        perr << "mean error: " << m << " +- " << s.stderror() << endl;
        if(prev_mean>0)
        {
            relative_improvement = (prev_mean-m)/prev_mean;
            perr << "Relative improvement: " << relative_improvement*100 << " %"<< endl;
        }
        prev_mean = m;
        //displayVarGraph(supervised_costvec, true);

    }
    
}

void DeepReconstructorNet::trainHiddenLayer(int which_input_layer, VMat inputs)
{
    int l = inputs->length();
    pair<int,int> nepochs = unsupervised_nepochs[which_input_layer];
    real min_improvement = -10000;
    if(unsupervised_min_improvement_rate.length()!=0)
        min_improvement = unsupervised_min_improvement_rate[which_input_layer];
    perr << "\n\n*********************************************" << endl;
    perr << "*** Training (unsupervised) layer " << which_input_layer+1 << " for max. " << nepochs.second << " epochs " << endl;
    perr << "*** each epoch has " << l << " examples and " << l/minibatch_size << " optimizer stages (updates)" << endl;
    Func f(layers[which_input_layer], reconstruction_costs[which_input_layer]);
    //displayVarGraph(reconstruction_costs[which_input_layer]);
    //displayFunction(f,false,false, 333, "train_func");
    Var totalcost = sumOf(inputs, f, minibatch_size);
    VarArray params = totalcost->parents();
    
    if ( reconstruction_optimizers.size() !=0 )
    {
        reconstruction_optimizers[which_input_layer]->setToOptimize(params, totalcost);
        reconstruction_optimizers[which_input_layer]->reset();    
    }
    else 
    {
        reconstruction_optimizer->setToOptimize(params, totalcost);
        reconstruction_optimizer->reset();    
    }

    TVec<string> colnames(4);
    colnames[0] = "nepochs";
    colnames[1] = "reconstr_cost";
    colnames[2] = "stderror";
    colnames[3] = "relative_improvement";
    VMat training_curve = new FileVMatrix(expdir/"training_costs_layer_"+tostring(which_input_layer+1)+".pmat",0,colnames);
    Vec costrow(4);

    VecStatsCollector st;
    real prev_mean = -1;
    real relative_improvement = 1000;
    for(int n=0; n<nepochs.first || (n<nepochs.second && relative_improvement >= min_improvement); n++)
    {
        st.forget();
        if ( reconstruction_optimizers.size() !=0 )
        {
            reconstruction_optimizers[which_input_layer]->nstages = l/minibatch_size;
            reconstruction_optimizers[which_input_layer]->optimizeN(st);
        }
        else 
        {
            reconstruction_optimizer->nstages = l/minibatch_size;
            reconstruction_optimizer->optimizeN(st);
        }        
        const StatsCollector& s = st.getStats(0);
        real m = s.mean();
        real er = s.stderror();
        perr << "Epoch " << n+1 << " mean error: " << m << " +- " << s.stderror() << endl;
        if(prev_mean>0)
        {
            relative_improvement = (prev_mean-m)/prev_mean;
            perr << "Relative improvement: " << relative_improvement*100 << " %"<< endl;
        }
        costrow[0] = n+1;
        costrow[1] = m;
        costrow[2] = er;
        costrow[3] = relative_improvement*100;
        training_curve->appendRow(costrow);
        training_curve->flush();

        prev_mean = m;
    }
}


void DeepReconstructorNet::computeOutput(const Vec& input, Vec& output) const
{
    output.resize(nout);
    compute_output->fprop(input, output);
}

void DeepReconstructorNet::computeCostsFromOutputs(const Vec& input, const Vec& output,
                                           const Vec& target, Vec& costs) const
{
    costs.resize(supervised_costs_names.length());
    output_and_target_to_cost->fprop(output&target, costs);
}

TVec<string> DeepReconstructorNet::getTestCostNames() const
{
    return supervised_costs_names;
}

TVec<string> DeepReconstructorNet::getTrainCostNames() const
{ 
    return supervised_costs_names;
}

Mat DeepReconstructorNet::getParameterValue(const string& varname)
{
    for(int i=0; i<parameters.length(); i++)
        if(parameters[i]->getName() == varname)
            return parameters[i]->matValue;
    PLERROR("There is no parameter  named %s", varname.c_str());
    return Mat(0,0);
}


Vec DeepReconstructorNet::getParameterRow(const string& varname, int n)
{
    for(int i=0; i<parameters.length(); i++)
        if(parameters[i]->getName() == varname)
            return parameters[i]->matValue(n);
    PLERROR("There is no parameter  named %s", varname.c_str());
    return Vec(0);
}

TVec<string> DeepReconstructorNet::listParameterNames()
{
    TVec<string> nameListe(0);
    for (int i=0; i<parameters.length(); i++)
        if (parameters[i]->getName() != "")
            nameListe.append(parameters[i]->getName());
    return nameListe;
}

TVec<Mat> DeepReconstructorNet::listParameter()
{
    TVec<Mat> matList(0);
    for (int i=0; i<parameters.length(); i++)
        matList.append(parameters[i]->matValue);
    return matList;
}


Mat DeepReconstructorNet::getMatValue(int layer)
{
    return layers[layer]->matValue;
}

void DeepReconstructorNet::setMatValue(int layer, Mat values)
{
    layers[layer]->matValue << values;
}

Mat DeepReconstructorNet::fpropOneLayer(int layer)
{
    VarArray proppath = propagationPath( layers[layer], layers[layer+1] );
    proppath.fprop();
    return getMatValue(layer+1);
}

Mat DeepReconstructorNet::reconstructOneLayer(int layer)
{
    VarArray proppath = propagationPath(layers[layer],reconstructed_layers[layer-1]);
    proppath.fprop();       
    return reconstructed_layers[layer-1]->matValue;
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
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:encoding=utf-8:textwidth=79 :50
