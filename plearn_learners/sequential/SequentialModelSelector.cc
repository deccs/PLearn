// -*- C++ -*-

// SequentialModelSelector.cc
//
// Copyright (C) 2003 Rejean Ducharme, Yoshua Bengio
// Copyright (C) 2003 Pascal Vincent
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



#include "SequentialModelSelector.h"
//#include "TMat_maths_impl.h"

//#define DEBUG

namespace PLearn {
using namespace std;


PLEARN_IMPLEMENT_OBJECT(SequentialModelSelector, "ONE LINE DESCR", "NO HELP");

SequentialModelSelector::SequentialModelSelector(): 
  init_train_size(1), 
  stepwise_save(true),
  comparison_type(1),
  comparison_window(-1)
{}

void SequentialModelSelector::setExperimentDirectory(const string& _expdir)
{
  PLearner::setExperimentDirectory(_expdir);
  string model_m;
  for(int m=0; m < models.size(); m++)
  {
    model_m = models[m]->getExperimentDirectory();
    if(model_m == "")
      model_m = "Model_" + tostring(m);
    models[m]->setExperimentDirectory(append_slash(_expdir)+ model_m);
  }
}

void SequentialModelSelector::build_()
{
  // Precondition check on init_train_size
  if(init_train_size < horizon)
  {
    PLWARNING("The init_train_size provided by the user was %d. However since the horizon is %d\n"
              "the init_train_size will be modified to be %d, the horizon value. This modification is\n"
              "necessary because, within a model selection context, a first internal test is needed to\n"
              "determine the first best model (see the SequentialModelSelector::train method\n"
              "implementation). If you want to avoid getting this warning, you should set init_train_size\n"
              "to horizon (%d) directly.", 
              init_train_size, horizon, horizon, horizon);
    init_train_size = horizon;
  }
  
  int nb_models = models.length();
  if(nb_models < 1)
    PLERROR("The 'models' option is mandatory in a SequentialModelSelector.");

  int nb_common_costs = common_costs.length();
  if(nb_common_costs < 1)
    PLERROR("The 'common_costs' requires at least 1 cost from which models will\n"
            "be compared to choose the best model.");  

  common_costs_index.resize(nb_models, nb_common_costs); 
  for(int m=0; m < nb_models; m++)
    for(int c=0; c < nb_common_costs; c++) 
      common_costs_index(m, c) = models[m]->getTestCostIndex( common_costs[c] );
  
  best_model.resize(max_seq_len);
  sequence_costs.resize(nb_models);

  for (int i=0; i<models.size(); i++)
    models[i]->horizon = horizon;

  forget();
}

void SequentialModelSelector::build()
{
  inherited::build();
  build_();
}

void SequentialModelSelector::declareOptions(OptionList& ol)
{    
  declareOption(ol, "init_train_size", &SequentialModelSelector::init_train_size,
                OptionBase::buildoption, "Size of first training set.\n");

  declareOption(ol, "stepwise_save", &SequentialModelSelector::stepwise_save, OptionBase::buildoption,
                "Does the model selector hass to save errors at each step.\n"
                "Default: true.");

  declareOption(ol, "models", &SequentialModelSelector::models,
                OptionBase::buildoption, "List of all the models.\n");

  declareOption(ol, "common_costs", &SequentialModelSelector::common_costs, OptionBase::buildoption,
                "The names of costs that are common to all models and that the user wishes the model\n"
                "selector to keep track of. The first one is considered to be the main cost, the one\n"
                "from which models will be compared to choose the best model.");

  declareOption(ol, "comparison_type", &SequentialModelSelector::comparison_type,
                OptionBase::buildoption, 
                "From the common_costs list, the first cost is the one from which models will be compared\n"
                "to choose the best model. But should the best model be chosen according to the\n"
                "\n"
                "max/min\n"
                "  +/-   1: Mean\n"
                "  +/-   2: Mean / Variance\n"
                "  +/-   3: more to come.\n"
                "\n"
                "of the cost realizations. \n"
                "Default: 1.\n");

  declareOption(ol, "comparison_window", &SequentialModelSelector::comparison_window, OptionBase::buildoption,
                "If positive, the comparison performed on the basis of common_cost[0] will be applyed only\n"
                "the comparison_window last elements of the cost sequence.\n"
                "Default: -1. (No window)");

  inherited::declareOptions(ol);
}

real SequentialModelSelector::sequenceCost(const Vec& sequence_errors)
{
  int seq_len = sequence_errors.length();
  if(seq_len == 0)
    return MISSING_VALUE;
  
  int window_start = seq_len - comparison_window;
  Vec windowed;
  if(comparison_window < 1 || window_start < 0)     
    windowed = sequence_errors; // The option is inactive or the sequence is not yet comparison_window long 
  else
    windowed = sequence_errors.subVec(window_start, comparison_window);
  
  int type = abs(comparison_type);
  real mean_ = mean(windowed);
  if (type == 1)
    return mean_;
  
  if (type == 2)
    return mean_ / variance(windowed, mean_);
  
  PLERROR("Invalid comparison type %d!", comparison_type);
  return MISSING_VALUE;
}

void SequentialModelSelector::forget()
{
  last_train_t = init_train_size;
  best_model.resize(max_seq_len);
  best_model.fill(-1);  // by default
  for (int i=0; i<models.size(); i++)
    models[i]->forget();

  inherited::forget();
}

void SequentialModelSelector::train()
{  
  last_call_train_t = train_set.length()-1;

  // The init_train_size option is the train_set size at which we should start training.
  //  The corresponding time index is (init_train_size-1). See also the build_ precondition
  //  check on init_train_size.
  int init_train_t = init_train_size-1;
  int start_t = MAX(init_train_t, last_train_t+train_step);
  if( start_t < last_test_t )
    PLERROR("SequentialModelSelector::train -- start_t = %d < %d = last_test_t", start_t, last_test_t);

  if( last_call_train_t < start_t )
    return;
  
  ProgressBar* pb = NULL;
  if (report_progress)
    pb = new ProgressBar("Training SequentialModelSelector learner",train_set.length());

  TVec< PP<VecStatsCollector> > dummy_stats(models.length());
  for (int t=start_t; t < train_set.length(); t++)
  {
    // The time t index is equal to a set last row index. Its length will therefore be: 
    int set_length = t+1;

#ifdef DEBUG
    cout << "SequentialModelSelector::train() -- sub_train.length = " << set_length-horizon << " et sub_test.length = " << set_length << endl;
#endif
        
    VMat sub_train = train_set.subMatRows(0,set_length-horizon); // last training pair is (t-2*horizon,t-horizon)
    sub_train->defineSizes(train_set->inputsize(), train_set->targetsize(), train_set->weightsize());

    VMat sub_test  = train_set.subMatRows(0,set_length); // last test pair is (t-horizon,t)
    sub_test->defineSizes(train_set->inputsize(), train_set->targetsize(), train_set->weightsize());

    for (int i=0; i < models.size(); i++)
    {
      if(t == start_t)
        dummy_stats[i] = new VecStatsCollector();

      models[i]->setTrainingSet(sub_train, false);
      models[i]->train();
      models[i]->test(sub_test, dummy_stats[i]); // last cost computed goes at t-1, last prediction at t-1-horizon
      
      Vec sequence_errors = remove_missing(models[i]->errors.column( common_costs_index(i, 0) ).toVecCopy());
      
#ifdef DEBUG
      cout << "models["<<i<<"]->getTestCostNames()[common_costs_index(i, 0)]: " 
           << models[i]->getTestCostNames()[common_costs_index(i, 0)] << endl;
      PLWARNING("models[%d]->errors.subMat(0,%d,%d,1)", i, common_costs_index(i, 0), t);
      cout << remove_missing( models[i]->errors.subMat(0,common_costs_index(i,0),set_length,1).toVecCopy() ) << endl;
      cout << "---\nOR\n---\n" << sequence_errors << endl; 
#endif
      
      sequence_costs[i] = sequenceCost(sequence_errors);
    }
    
    // we set the best model for this time step
    if(comparison_type > 0)
      best_model[t] = argmax(sequence_costs, true);
    else
      best_model[t] = argmin(sequence_costs, true);

    if(best_model[t] == -1)
      best_model[t] = 0;   // All models provided only MISSING_VALUE as common_costs[0] !!!

#ifdef DEBUG
    cout << "sequence_costs: " << sequence_costs << endl;
    cout << "SequentialModelSelector::train() -- t = " << t << " et best_model = " << best_model[t] << endl;
#endif

    if(predictions(t-horizon).hasMissing())
      predictions(t-horizon) << models[best_model[t]]->predictions(t-horizon);
    
#ifdef DEBUG
    cout << "SequentialModelSelector::train() -- train_set.length = " << set_length << endl;
#endif
    if (pb)
      pb->update(t);
  }
  
  // Now train with everything that is available -- last training pair is (t-horizon,t)
  Vec best_model_costs; 
  Vec models_update;
  for (int i=0; i<models.size(); i++)
  {
    models[i]->setTrainingSet(train_set, false);
    models[i]->train();
    
    if(i == best_model[last_call_train_t])
    {
      best_model_costs = models[i]->errors(last_call_train_t);
      models_update.append( best_model_costs );
    }
    else
      models_update.append( models[i]->errors(last_call_train_t) );
  }
  Vec update = best_model_costs( common_costs_index(best_model[last_call_train_t]) );
  update.append(models_update);
  if(train_stats) train_stats->update(models_update);

  predictions(last_call_train_t) << models[ best_model[last_call_train_t] ]->predictions(last_call_train_t);
  errors(last_call_train_t) << update;
#ifdef DEBUG
  cout << "update " << last_call_train_t << ": " << update << endl;
#endif

  if(pb)
    pb->close();


  last_train_t = last_call_train_t;
#ifdef DEBUG
  cout << "SequentialModelSelector.last_train_t = " << last_train_t << endl;
#endif
  

  if(stepwise_save)
  {
    string s1 = append_slash(expdir) + "predictions_train_t=" + tostring(last_train_t); //"seq_model/predictions_train_t=" + tostring(last_train_t);
    saveAsciiWithoutSize(s1, predictions);

    string s2 = append_slash(expdir) + "errors_train_t=" + tostring(last_train_t);
    saveAsciiWithoutSize(s2, errors);
  }
}

void SequentialModelSelector::test(VMat test_set, PP<VecStatsCollector> test_stats,
    VMat testoutputs, VMat testcosts) const
{
  // Since model selection train method may not train before (init_train_size+horizon)
  //  we have to verify if a first train was actually done prior to entering the 
  //  test core
  if(last_train_t == -1)
  {
    PLWARNING("SequentialModelSelector::test -- Skipped because there were no prior train.");
    return;
  }

  int start_t = MAX(last_train_t+1,last_test_t+1);
  if( test_set.length()-1 < start_t )
    return;

  ProgressBar* pb = NULL;
  if (report_progress)
    pb = new ProgressBar("Testing SequentialModelSelector learner",test_set.length());

  TVec< PP<VecStatsCollector> > dummy_stats(models.length());  
  for (int t=start_t; t < test_set.length(); t++)
  { 
    Vec best_model_costs;      
    Vec models_update;
    
    for (int i=0; i<models.size(); i++)
    {    
      if(t == start_t)
        dummy_stats[i] = new VecStatsCollector();

      models[i]->test(test_set, dummy_stats[i]);
      
      if(i == best_model[last_train_t])
      {
        best_model_costs = models[i]->errors(t);
        models_update.append( best_model_costs );
      }
      else
        models_update.append( models[i]->errors(t) );
    }
    
    // The update is composed of the common_costs and the modelwise costs
    Vec update = best_model_costs( common_costs_index(best_model[last_train_t]) );
    update.append(models_update);
    
    test_stats->update( update );
  
    predictions(t) << models[best_model[last_train_t]]->predictions(t); 
    errors(t) << update;
    if (testoutputs) testoutputs->appendRow( predictions(t) );
    if (testcosts) testcosts->appendRow(update);

    if(pb)
      pb->update(t);
  }
  if(pb)
    pb->close();

  
  last_test_t = test_set.length()-1;
#ifdef DEBUG
  cout << "SequentialModelSelector.last_test_t = " << last_test_t << endl;
#endif
  

  if(stepwise_save)
  {
    string s1 = append_slash(expdir) + "predictions_test_t=" + tostring(last_test_t);
    saveAsciiWithoutSize(s1, predictions);
    
    string s2 = append_slash(expdir) + "errors_test_t=" + tostring(last_test_t);
    saveAsciiWithoutSize(s2, errors);
  }
}

void SequentialModelSelector::computeOutput(const Vec& input, Vec& output) const
{
  models[best_model[last_train_t]]->computeOutput(input, output);
}

void SequentialModelSelector::computeCostsFromOutputs(const Vec& input,
    const Vec& output, const Vec& target, Vec& costs) const
{
  models[best_model[last_train_t]]->computeCostsFromOutputs(input, output, target, costs);
}

void SequentialModelSelector::computeOutputAndCosts(const Vec& input,
    const Vec& target, Vec& output, Vec& costs) const
{
  models[best_model[last_train_t]]->computeOutputAndCosts(input, target, output, costs);
}
 
void SequentialModelSelector::computeCostsOnly(const Vec& input,
    const Vec& target, Vec& costs) const
{
  models[best_model[last_train_t]]->computeCostsOnly(input, target, costs);
}

TVec<string> SequentialModelSelector::getTestCostNames() const
{ 
  TVec<string> tcnames = common_costs;
  
  int nb_models = models.length();
  for(int m=0; m < nb_models; m++)
  {
    TVec<string> tcm = models[m]->getTestCostNames();
    for(int c=0; c < tcm.length(); c++)
      tcnames.append("model" + tostring(m) + "::" + tcm[c]);
  }

  return tcnames;
}

TVec<string> SequentialModelSelector::getTrainCostNames() const
{
  TVec<string> tcnames;
  
  int nb_models = models.length();
  for(int m=0; m < nb_models; m++)
  {
    TVec<string> tcm = models[m]->getTrainCostNames();
    for(int c=0; c < tcm.length(); c++)
      tcnames.append("model" + tostring(m) + "::" + tcm[c]);
  }
  
  return tcnames;
}

void SequentialModelSelector::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  inherited::makeDeepCopyFromShallowCopy(copies);
  deepCopyField(models, copies);
  //deepCopyField(mean_costs, copies);
} 

} // end of namespace PLearn

