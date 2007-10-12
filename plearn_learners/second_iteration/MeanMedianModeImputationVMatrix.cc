// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2001 Pascal Vincent, Yoshua Bengio, Rejean Ducharme and University of Montreal
// Copyright (C) 2002 Pascal Vincent, Julien Keable, Xavier Saint-Mleux
// Copyright (C) 2003 Olivier Delalleau
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


/* *******************************************************************    
   * $Id: MeanMedianModeImputationVMatrix.cc 3658 2005-07-06 20:30:15  Godbout $
   ******************************************************************* */


#include "MeanMedianModeImputationVMatrix.h"

namespace PLearn {
using namespace std;

/** MeanMedianModeImputationVMatrix **/

PLEARN_IMPLEMENT_OBJECT(
  MeanMedianModeImputationVMatrix,
  "VMat class to impute the observed variable mean to replace missing values in the source matrix.",
  "This class will replace missing values in the underlying dataset with the mean, median or mode observed on the train set.\n"
  "The imputed value is based on the imputation instruction option.\n"
  );

MeanMedianModeImputationVMatrix::MeanMedianModeImputationVMatrix()
: number_of_train_samples_to_use(0.0)
{
}

MeanMedianModeImputationVMatrix::~MeanMedianModeImputationVMatrix()
{
}

void MeanMedianModeImputationVMatrix::declareOptions(OptionList &ol)
{
  declareOption(ol, "train_set", &MeanMedianModeImputationVMatrix::train_set, OptionBase::buildoption, 
                "A referenced train set.\n"
                "The mean, median or mode is computed with the observed values in this data set.\n"
                "It is used in combination with the option number_of_train_samples_to_use\n");

  declareOption(ol, "number_of_train_samples_to_use", &MeanMedianModeImputationVMatrix::number_of_train_samples_to_use, OptionBase::buildoption, 
                "The number of samples from the train set that will be examined to compute the required statistic for each variable.\n" 
                "If equal to zero, all the samples from the train set are used to calculated the statistics.\n"
                "If it is a fraction between 0 and 1, this proportion of the samples are used.\n"
                "If greater or equal to 1, the integer portion is interpreted as the number of samples to use.");
      
  declareOption(ol, "imputation_spec", &MeanMedianModeImputationVMatrix::imputation_spec, OptionBase::buildoption, 
                "Pairs of instruction of the form field_name : mean | median | mode.");

  declareOption(ol, "variable_mean", &MeanMedianModeImputationVMatrix::variable_mean, OptionBase::learntoption, 
                "The vector of variable means observed from the train set.");

  declareOption(ol, "variable_median", &MeanMedianModeImputationVMatrix::variable_median, OptionBase::learntoption, 
                "The vector of variable medians observed from the train set.");

  declareOption(ol, "variable_mode", &MeanMedianModeImputationVMatrix::variable_mode, OptionBase::learntoption, 
                "The vector of variable modes observed from the train set.");

  declareOption(ol, "variable_present_count", &MeanMedianModeImputationVMatrix::variable_present_count, OptionBase::learntoption, 
                "The vector of non missing variable counts from the train set.");

  declareOption(ol, "variable_missing_count", &MeanMedianModeImputationVMatrix::variable_missing_count, OptionBase::learntoption, 
                "The vector of missing variable counts from the train set.");

  declareOption(ol, "variable_mode_count", &MeanMedianModeImputationVMatrix::variable_mode_count, OptionBase::learntoption, 
                "The vector of variable mode counts from the train set.");

  declareOption(ol, "variable_imputation_instruction", &MeanMedianModeImputationVMatrix::variable_imputation_instruction, OptionBase::learntoption, 
                "The vector of coded instruction for each variables.");

  inherited::declareOptions(ol);
}

void MeanMedianModeImputationVMatrix::build()
{
  inherited::build();
  build_();
}

void MeanMedianModeImputationVMatrix::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
  deepCopyField(train_set, copies);
  deepCopyField(number_of_train_samples_to_use, copies);
  deepCopyField(imputation_spec, copies);
  deepCopyField(variable_mean, copies);
  deepCopyField(variable_median, copies);
  deepCopyField(variable_mode, copies);
  deepCopyField(variable_present_count, copies);
  deepCopyField(variable_missing_count, copies);
  deepCopyField(variable_imputation_instruction, copies);
  inherited::makeDeepCopyFromShallowCopy(copies);
}

void MeanMedianModeImputationVMatrix::getExample(int i, Vec& input, Vec& target, real& weight)
{
  source->getExample(i, input, target, weight);
  for (int source_col = 0; source_col < input->length(); source_col++)
  {
    if (is_missing(input[source_col]) && variable_imputation_instruction[source_col] > 0)
      if (variable_imputation_instruction[source_col] == 1) input[source_col] = variable_mean[source_col];
      else if (variable_imputation_instruction[source_col] == 2) input[source_col] = variable_median[source_col];
      else if (variable_imputation_instruction[source_col] == 3) input[source_col] = variable_mode[source_col];
  }  
}

real MeanMedianModeImputationVMatrix::get(int i, int j) const
{ 
  real variable_value = source->get(i, j);
  if (!is_missing(variable_value)) return variable_value;
  if (variable_imputation_instruction[j] == 1) return variable_mean[j];
  if (variable_imputation_instruction[j] == 2) return variable_median[j];
  if (variable_imputation_instruction[j] == 3) return variable_mode[j];
  return variable_value;
}

void MeanMedianModeImputationVMatrix::put(int i, int j, real value)
{
  PLERROR("In MeanMedianModeImputationVMatrix::put not implemented");
}

void MeanMedianModeImputationVMatrix::getSubRow(int i, int j, Vec v) const
{  
  source->getSubRow(i, j, v);
  for (int source_col = 0; source_col < v->length(); source_col++) 
    if (is_missing(v[source_col]) && variable_imputation_instruction[source_col + j] > 0)
      if (variable_imputation_instruction[source_col + j] == 1) v[source_col] = variable_mean[source_col + j];
      else if (variable_imputation_instruction[source_col + j] == 2) v[source_col] = variable_median[source_col + j];
      else if (variable_imputation_instruction[source_col + j] == 3) v[source_col] = variable_mode[source_col + j];
}

void MeanMedianModeImputationVMatrix::putSubRow(int i, int j, Vec v)
{
  PLERROR("In MeanMedianModeImputationVMatrix::putSubRow not implemented");
}

void MeanMedianModeImputationVMatrix::appendRow(Vec v)
{
  PLERROR("In MeanMedianModeImputationVMatrix::appendRow not implemented");
}

void MeanMedianModeImputationVMatrix::insertRow(int i, Vec v)
{
  PLERROR("In MeanMedianModeImputationVMatrix::insertRow not implemented");
}

void MeanMedianModeImputationVMatrix::getRow(int i, Vec v) const
{  
  source-> getRow(i, v);
  for (int source_col = 0; source_col < v->length(); source_col++)
    if (is_missing(v[source_col]) && variable_imputation_instruction[source_col] > 0)
      if (variable_imputation_instruction[source_col] == 1) v[source_col] = variable_mean[source_col];
      else if (variable_imputation_instruction[source_col] == 2) v[source_col] = variable_median[source_col];
      else if (variable_imputation_instruction[source_col] == 3) v[source_col] = variable_mode[source_col]; 
}

void MeanMedianModeImputationVMatrix::putRow(int i, Vec v)
{
  PLERROR("In MeanMedianModeImputationVMatrix::putRow not implemented");
}

void MeanMedianModeImputationVMatrix::getColumn(int i, Vec v) const
{  
  source-> getColumn(i, v);
  for (int source_row = 0; source_row < v->length(); source_row++)
    if (is_missing(v[source_row]) && variable_imputation_instruction[i] > 0)
      if (variable_imputation_instruction[i] == 1) v[source_row] = variable_mean[i];
      else if (variable_imputation_instruction[i] == 2) v[source_row] = variable_median[i];
      else if (variable_imputation_instruction[i] == 3) v[source_row] = variable_mode[i];
}



void MeanMedianModeImputationVMatrix::build_()
{
    if (!train_set || !source) PLERROR("In MeanMedianModeImputationVMatrix::train set and source vmat must be supplied");
    train_length = train_set->length();
    if (number_of_train_samples_to_use > 0.0)
        if (number_of_train_samples_to_use < 1.0) train_length = (int) (number_of_train_samples_to_use * (real) train_length);
        else train_length = (int) number_of_train_samples_to_use;
    if (train_length > train_set->length()) train_length = train_set->length();
    if(train_length < 1) PLERROR("In MeanMedianModeImputationVMatrix::length of the number of train samples to use must be at least 1, got: %i", train_length);
    train_width = train_set->width();
    train_targetsize = train_set->targetsize();
    train_weightsize = train_set->weightsize();
    train_inputsize = train_set->inputsize();
    if(train_inputsize < 1) PLERROR("In MeanMedianModeImputationVMatrix::inputsize of the train vmat must be supplied, got : %i", train_inputsize);
    source_width = source->width();
    source_targetsize = source->targetsize();
    source_weightsize = source->weightsize();
    source_inputsize = source->inputsize();
    if (train_width != source_width) PLERROR("In MeanMedianModeImputationVMatrix::train set and source width must agree, got : %i, %i", train_width, source_width);
    if (train_targetsize != source_targetsize) PLERROR("In MeanMedianModeImputationVMatrix::train set and source targetsize must agree, got : %i, %i", train_targetsize, source_targetsize);
    if (train_weightsize != source_weightsize) PLERROR("In MeanMedianModeImputationVMatrix::train set and source weightsize must agree, got : %i, %i", train_weightsize, source_weightsize);
    if (train_inputsize != source_inputsize) PLERROR("In MeanMedianModeImputationVMatrix::train set and source inputsize must agree, got : %i, %i", train_inputsize, source_inputsize);
    train_field_names.resize(train_width);
    train_field_names = train_set->fieldNames();
    source_length = source->length();
    length_ = source_length;
    width_ = source_width;
    inputsize_ = source_inputsize;
    targetsize_ = source_targetsize;
    weightsize_ = source_weightsize;
    declareFieldNames(train_field_names);
    variable_mean.resize(train_width);
    variable_median.resize(train_width);
    variable_mode.resize(train_width);
    variable_imputation_instruction.resize(train_width);
    variable_imputation_instruction.clear();
    for (int spec_col = 0; spec_col < imputation_spec.size(); spec_col++)
    {
        for (train_col = 0; train_col < train_width; train_col++)
        {
            if (imputation_spec[spec_col].first == train_field_names[train_col]) break;
        }
        if (train_col >= train_width) PLERROR("In MeanMedianModeImputationVMatrix: no field with this name in train data set: %s", (imputation_spec[spec_col].first).c_str());
        if (imputation_spec[spec_col].second == "mean") variable_imputation_instruction[train_col] = 1;
        else if (imputation_spec[spec_col].second == "median") variable_imputation_instruction[train_col] = 2;
        else if (imputation_spec[spec_col].second == "mode") variable_imputation_instruction[train_col] = 3;
        else PLERROR("In MeanMedianModeImputationVMatrix: unsupported imputation instruction: %s : %s", (imputation_spec[spec_col].first).c_str(), (imputation_spec[spec_col].second).c_str());
    }
    train_metadata = train_set->getMetaDataDir();
    mean_median_mode_file_name = train_metadata + "mean_median_mode_file.pmat";
    
    if (!isfile(mean_median_mode_file_name))
    {
        computeMeanMedianModeVectors();
        createMeanMedianModeFile();
    }
    else loadMeanMedianModeFile();
}

void MeanMedianModeImputationVMatrix::createMeanMedianModeFile()
{
    mean_median_mode_file = new FileVMatrix(mean_median_mode_file_name, 3, train_field_names);
    mean_median_mode_file->putRow(0, variable_mean);
    mean_median_mode_file->putRow(1, variable_median);
    mean_median_mode_file->putRow(2, variable_mode);
}

void MeanMedianModeImputationVMatrix::loadMeanMedianModeFile()
{
    mean_median_mode_file = new FileVMatrix(mean_median_mode_file_name);
    mean_median_mode_file->getRow(0, variable_mean);
    mean_median_mode_file->getRow(1, variable_median);
    mean_median_mode_file->getRow(2, variable_mode);
}

VMat MeanMedianModeImputationVMatrix::getMeanMedianModeFile()
{
    return mean_median_mode_file;
}

void MeanMedianModeImputationVMatrix::computeMeanMedianModeVectors()
{
    variable_present_count.resize(train_width);
    variable_missing_count.resize(train_width);
    variable_mode_count.resize(train_width);
    variable_mean.clear();
    variable_median.clear();
    variable_mode.clear();
    variable_present_count.clear();
    variable_missing_count.clear();
    variable_mode_count.clear();
    variable_vec.resize(train_set->length());
    cout << fixed << showpoint;
    ProgressBar* pb = 0;
    pb = new ProgressBar("Computing the mean, median and mode vectors", train_width);
    for (train_col = 0; train_col < train_width; train_col++)
    {
        current_value = 0.0;
        current_value_count = 0;
        train_set->getColumn(train_col, variable_vec);
        sortColumn(variable_vec, 0, train_length);
        for (train_row = 0; train_row < train_length; train_row++)
        {
            if (is_missing(variable_vec[train_row]))
            {
                variable_missing_count[train_col] += 1;
                continue;
            }
            variable_mean[train_col] += variable_vec[train_row];
            variable_present_count[train_col] += 1;
            if (variable_vec[train_row] != current_value)
            {
                if (current_value_count > variable_mode_count[train_col])
                {
                    variable_mode[train_col] = current_value;
                    variable_mode_count[train_col] = current_value_count;
                }
                current_value_count = 0;
                current_value = variable_vec[train_row];
            }
            current_value_count += 1;
        }
        if (variable_present_count[train_col] > 0)
        {
            variable_mean[train_col] = variable_mean[train_col] / variable_present_count[train_col];
            variable_median[train_col] = variable_vec[(variable_present_count[train_col] / 2)];
        }
        if (current_value_count > variable_mode_count[train_col])
        {
            variable_mode[train_col] = current_value;
            variable_mode_count[train_col] = current_value_count;
        }
        pb->update( train_col );
        /*
        cout << "col: "         << setw(3)  <<                     train_col
             << " present: "    << setw(5)  <<                     variable_present_count[train_col]
             << " missing: "    << setw(5)  <<                     variable_missing_count[train_col]
             << " mean: "       << setw(11) << setprecision(2) <<  variable_mean[train_col]
             << " median: "     << setw(11) << setprecision(2) <<  variable_median[train_col]
             << " mode count: " << setw(5)  <<                     variable_mode_count[train_col]
             << " mode: "       << setw(11) << setprecision(2) <<  variable_mode[train_col]
             << " name: "       <<                                 train_field_names[train_col]
             << endl;
        */
    }
    delete pb;
}

void MeanMedianModeImputationVMatrix::sortColumn(Vec input_vec, int start, int end)
{
  int start_index = start;
  int end_index = end - 1;
  int forward_index;
  int backward_index;
  int stack_index = -1;
  real pivot_value;
  TVec<int> stack(50);
  for (;;)
  {
    if ((end_index - start_index) < 7)
    {
      if (end_index > start_index)
      {
        sortSmallSubArray(input_vec, start_index, end_index);
      }
      if (stack_index < 0)
      {
        break;
      }
      end_index = stack[stack_index--];
      start_index = stack[stack_index--];
    }
    else
    {
      swapValues(input_vec, start_index + 1, (start_index + end_index) / 2);
      if (compare(input_vec[start_index], input_vec[end_index]) > 0.0) swapValues(input_vec, start_index, end_index);
      if (compare(input_vec[start_index + 1], input_vec[end_index]) > 0.0) swapValues(input_vec, start_index + 1, end_index);
      if (compare(input_vec[start_index], input_vec[start_index + 1]) > 0.0) swapValues(input_vec, start_index, start_index + 1);
      forward_index = start_index + 1;
      backward_index = end_index;
      pivot_value = input_vec[start_index + 1];
      for (;;)
      {
        do forward_index++; while (compare(input_vec[forward_index], pivot_value) < 0.0);
        do backward_index--; while (compare(input_vec[backward_index], pivot_value) > 0.0);
        if (backward_index < forward_index)
        {
          break;
        }
        swapValues(input_vec, forward_index, backward_index);
      }
      swapValues(input_vec, start_index + 1, backward_index);
      stack_index += 2;
      if (stack_index > 50)
        PLERROR("RegressionTreeRegistersVMatrix: the stack for sorting the rows is too small");
      if ((end_index - forward_index + 1) >= (backward_index - start_index))
      {
        stack[stack_index] = end_index;
        stack[stack_index - 1] = forward_index;
        end_index = backward_index - 1;
      }
      else
      {
        stack[stack_index] = backward_index - 1;
        stack[stack_index - 1] = start_index;
        start_index = forward_index;
      }
    }
  }
}
  
void MeanMedianModeImputationVMatrix::sortSmallSubArray(Vec input_vec, int start_index, int end_index)
{
  int index_i;
  int index_j;
  for (index_i = start_index + 1; index_i <= end_index; index_i++)
  {
    real saved_value = input_vec[index_i];
    for (index_j = index_i - 1; index_j >= start_index; index_j--)
    {
      if (compare(input_vec[index_j], saved_value) <= 0.0)
      {
        break;
      }
      input_vec[index_j + 1] = input_vec[index_j];
    }
    input_vec[index_j + 1] = saved_value;
  }  
}

void MeanMedianModeImputationVMatrix::swapValues(Vec input_vec, int index_i, int index_j)
{
  real saved_value = input_vec[index_i];
  input_vec[index_i] = input_vec[index_j];
  input_vec[index_j] = saved_value;
}

real MeanMedianModeImputationVMatrix::compare(real field1, real field2)
{
  if (is_missing(field1) && is_missing(field2)) return 0.0;
  if (is_missing(field1)) return +1.0;
  if (is_missing(field2)) return -1.0;
  return field1 - field2;
}

} // end of namespcae PLearn