// -*- C++ -*-

// SurfaceTemplateLearner.cc
//
// Copyright (C) 2006 Pascal Lamblin and Olivier Delalleau
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

// Authors: Pascal Lamblin and Olivier Delalleau

/*! \file SurfaceTemplateLearner.cc */


#include "SurfaceTemplateLearner.h"
#include "ScoreLayerVariable.h"

namespace PLearn {
using namespace std;

PLEARN_IMPLEMENT_OBJECT(
    SurfaceTemplateLearner,
    "Neural-network to learn from molecular alignment.",
    ""
);

////////////////////////////
// SurfaceTemplateLearner //
////////////////////////////
SurfaceTemplateLearner::SurfaceTemplateLearner():
    min_feature_dev(1e-3),
    min_geom_dev(1e-3),
    simple_mixture(false)
{
    nhidden2 = 10;
    // Set some NNet options whose value is fixed in this learner.
    nhidden = 0;
    noutputs = 1;
    output_transfer_func = "sigmoid";
    cost_funcs = TVec<string>(1, "stable_cross_entropy");
    transpose_first_hidden_layer = false;
    n_non_params_in_first_hidden_layer = 1;
}

////////////////////
// declareOptions //
////////////////////
void SurfaceTemplateLearner::declareOptions(OptionList& ol)
{
    declareOption(ol, "min_feature_dev",
                  &SurfaceTemplateLearner::min_feature_dev,
                  OptionBase::buildoption,
        "Minimum feature standard deviations allowed.");

    declareOption(ol, "min_geom_dev",
                  &SurfaceTemplateLearner::min_geom_dev,
                  OptionBase::buildoption,
        "Minimum geometric standard deviations allowed.");

    // We rename 'first_hidden_layer' into 'score_layer' to avoid potential
    // confusion.
    declareOption(ol, "score_layer",
                  &SurfaceTemplateLearner::first_hidden_layer,
                  OptionBase::buildoption,
        "The layer of scores (should be a ScoreLayerVariable).");

    declareOption(ol, "simple_mixture",
                  &SurfaceTemplateLearner::simple_mixture,
                  OptionBase::buildoption,
        "If true, then instead of building another hidden layer on top of\n"
        "alignment scores, we use them directly in a mixture of Gaussians\n"
        "fashion to estimate the probability of being active.");

    declareOption(ol, "templates_source",
                  &SurfaceTemplateLearner::templates_source,
                  OptionBase::buildoption,
        "The dataset where templates are taken from. If not provided, the\n"
        "training set will be used instead.");

    // Now call the parent class' declareOptions
    inherited::declareOptions(ol);

    // Redeclare parent's option to make this learner more user-friendly.

    // 'nhidden' now modifies the 'nhidden2' parameter in NNet, since a
    // SurfaceTemplateLearner has always a first hidden layer that is a
    // ScoreLayerVariable.
    redeclareOption(ol, "nhidden", &SurfaceTemplateLearner::nhidden2,
                                   OptionBase::buildoption,
        "Number of hidden units.");

    redeclareOption(ol, "nhidden2", &SurfaceTemplateLearner::nhidden2,
                                    OptionBase::nosave,
        "Not used (see nhidden).");

    redeclareOption(ol, "noutputs", &SurfaceTemplateLearner::noutputs,
                                    OptionBase::nosave,
        "Not used (= 1).");

    redeclareOption(ol, "bias_decay", &SurfaceTemplateLearner::bias_decay,
                                      OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "layer1_weight_decay",
                    &SurfaceTemplateLearner::layer1_weight_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "layer1_bias_decay",
                    &SurfaceTemplateLearner::layer1_bias_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "layer2_weight_decay",
                    &SurfaceTemplateLearner::layer2_weight_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "layer2_bias_decay",
                    &SurfaceTemplateLearner::layer2_bias_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "output_layer_weight_decay",
                    &SurfaceTemplateLearner::output_layer_weight_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "output_layer_bias_decay",
                    &SurfaceTemplateLearner::output_layer_bias_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "direct_in_to_out_weight_decay",
                    &SurfaceTemplateLearner::direct_in_to_out_weight_decay,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "L1_penalty", &SurfaceTemplateLearner::L1_penalty,
                                      OptionBase::nosave,
        "Not used (deprecated).");

    redeclareOption(ol, "fixed_output_weights",
                    &SurfaceTemplateLearner::fixed_output_weights,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "input_reconstruction_penalty",
                    &SurfaceTemplateLearner::input_reconstruction_penalty,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "direct_in_to_out",
                    &SurfaceTemplateLearner::direct_in_to_out,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "rbf_layer_size",
                    &SurfaceTemplateLearner::rbf_layer_size,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "first_class_is_junk",
                    &SurfaceTemplateLearner::first_class_is_junk,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "output_transfer_func",
                    &SurfaceTemplateLearner::output_transfer_func,
                    OptionBase::nosave,
        "Not used (= sigmoid or none, depending on 'simple_mixture').");

    redeclareOption(ol, "hidden_transfer_func",
                    &SurfaceTemplateLearner::hidden_transfer_func,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "first_hidden_layer",
                    &SurfaceTemplateLearner::first_hidden_layer,
                    OptionBase::nosave,
        "Not used (renamed to 'score_layer').");

    redeclareOption(ol, "transpose_first_hidden_layer",
                    &SurfaceTemplateLearner::transpose_first_hidden_layer,
                    OptionBase::nosave,
        "Not used (= false).");

    redeclareOption(ol, "n_non_params_in_first_hidden_layer",
                   &SurfaceTemplateLearner::n_non_params_in_first_hidden_layer,
                   OptionBase::nosave,
        "Not used (= 1 because of the 'final_output' variable in the\n"
        "ScoreLayerVariable).");

    redeclareOption(ol, "margin", &SurfaceTemplateLearner::margin,
                                  OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "do_not_change_params",
                    &SurfaceTemplateLearner::do_not_change_params,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "initialization_method",
                    &SurfaceTemplateLearner::initialization_method,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "forget_when_training_set_changes",
                    &SurfaceTemplateLearner::forget_when_training_set_changes,
                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "nservers", &SurfaceTemplateLearner::nservers,
                                    OptionBase::nosave,
        "Not used (simplification).");

    redeclareOption(ol, "save_trainingset_prefix",
                    &SurfaceTemplateLearner::save_trainingset_prefix,
                    OptionBase::nosave,
        "Not used (simplification).");

}

////////////
// build_ //
////////////
void SurfaceTemplateLearner::build_()
{
    // Ensure the first hidden layer is a subclass of ScoreLayerVariable.
    if (first_hidden_layer) {
        PP<ScoreLayerVariable> score_layer =
            (ScoreLayerVariable*) ((Variable*) first_hidden_layer);
        if (!score_layer)
            PLERROR("In SurfaceTemplateLearner::build_ - The first hidden "
                    "layer, as given by the 'score_layer' option, must be a "
                    "subclass of ScoreLayerVariable");
        // Set the minimum value for template standard deviations.
        if (score_layer->run_icp_var) {
            TVec< PP<ChemicalICP> > icp_aligners =
                score_layer->run_icp_var->icp_aligners;
            for (int i = 0; i < icp_aligners.length(); i++) {
                icp_aligners[i]->all_template_feat_dev->
                    setMinValue(min_feature_dev);
                icp_aligners[i]->template_geom_dev->setMinValue(min_geom_dev);
            }
        }
        // Set value of 'simple_mixture' option.
        score_layer->simple_mixture = this->simple_mixture;
    }
}

///////////
// build //
///////////
void SurfaceTemplateLearner::build()
{
    // Very ugly hack: because NNet::build_() will perform a fprop(), we need a
    // sensible input value for this fprop, which means we need a training set.
    // A simple way to fix this would be to remove the
    // output_and_target_to_cost->recomputeParents() in the NNet build, but one
    // should make sure it does not break anything first.
    if (!train_set && templates_source)
        this->train_set = templates_source;

    // Because the overall network is built in the NNet build, the simple
    // mixture case must be handled before calling it.
    first_hidden_layer_is_output = simple_mixture;

    // Since we are already hacking this method, we may continue doing so:
    // depending on the value of 'simple_mixture' (true or false), the output
    // transfer function should be either nothing or a sigmoid.
    output_transfer_func = simple_mixture ? "none" : "sigmoid";

    inherited::build();
    build_();
}

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void SurfaceTemplateLearner::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);

    // ### Call deepCopyField on all "pointer-like" fields
    // ### that you wish to be deepCopied rather than
    // ### shallow-copied.
    // ### ex:
    // deepCopyField(trainvec, copies);
    deepCopyField(templates_source, copies);

    // ### Remove this line when you have fully implemented this method.
    // PLERROR("SurfaceTemplateLearner::makeDeepCopyFromShallowCopy not fully (correctly) implemented yet!");
}

////////////////////
// setTrainingSet //
////////////////////
void SurfaceTemplateLearner::setTrainingSet(VMat training_set,
                                            bool call_forget)
{
    // Rebuild the internal score layer.
    PP<ScoreLayerVariable> score_layer =
        (ScoreLayerVariable*) ((Variable*) first_hidden_layer);
    score_layer->templates_source =
        this->templates_source ? this->templates_source : training_set;
    score_layer->setMappingsSource(training_set);
    score_layer->build();

    inherited::setTrainingSet(training_set, call_forget);
}

//////////
// test //
//////////
void SurfaceTemplateLearner::test(VMat testset,
                                  PP<VecStatsCollector> test_stats,
                                  VMat testoutputs, VMat testcosts) const
{
    PP<ScoreLayerVariable> score_layer =
        (ScoreLayerVariable*) ((Variable*) first_hidden_layer);
    score_layer->setMappingsSource(testset);
    inherited::test(testset, test_stats, testoutputs, testcosts);
}

/*
void SurfaceTemplateLearner::forget()
{
    inherited::forget();
}
*/

///////////
// train //
///////////
void SurfaceTemplateLearner::train()
{
    PP<ScoreLayerVariable> score_layer =
        (ScoreLayerVariable*) ((Variable*) first_hidden_layer);
    score_layer->setMappingsSource(train_set);
    if (stage == 0) {
        // Make sure all ICP aligners forget any previously computed alignment.
        // This can be important when they use some memory scheme, since at
        // build time a first alignment might be performed, and should probably
        // be forgotten.
        TVec< PP<ChemicalICP> > icps = score_layer->run_icp_var->icp_aligners;
        for (int i = 0; i < icps.length();  i++)
            icps[i]->forgetMemorizedAlignments();
    }
    inherited::train();
}

/*
void SurfaceTemplateLearner::computeOutput(const Vec& input, Vec& output) const
{
    // Compute the output from the input.
    // int nout = outputsize();
    // output.resize(nout);
    // ...
}

void SurfaceTemplateLearner::computeCostsFromOutputs(const Vec& input, const Vec& output,
                                           const Vec& target, Vec& costs) const
{
// Compute the costs from *already* computed output.
// ...
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
