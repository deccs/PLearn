// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 1998 Pascal Vincent
// Copyright (C) 1999-2002 Pascal Vincent, Yoshua Bengio, Rejean Ducharme and University of Montreal
// Copyright (C) 2001-2002 Nicolas Chapados, Ichiro Takeuchi, Jean-Sebastien Senecal
// Copyright (C) 2002 Xiangdong Wang, Christian Dorion

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
   * $Id: SumOfVariable.cc,v 1.9 2004/04/27 16:05:57 morinf Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "SumOfVariable.h"
#include "PLMPI.h"
#include "DisplayUtils.h"

namespace PLearn {
using namespace std;



/** SumOfVariable **/

PLEARN_IMPLEMENT_OBJECT(SumOfVariable,
                        "Variable that sums the value of a Func evaluated on each row of a VMat",
                        "NO HELP");

SumOfVariable::SumOfVariable(VMat the_distr, Func the_f, int the_nsamples)
  : inherited(nonInputParentsOfPath(the_f->inputs,the_f->outputs), 
                the_f->outputs[0]->length(), 
                the_f->outputs[0]->width()),
    distr(the_distr), f(the_f), nsamples(the_nsamples), curpos(0),
    //input_value(the_distr->inputsize()+the_distr->targetsize()+the_distr->weightsize()), 
    //input_gradient(the_distr->inputsize()+the_distr->targetsize()+the_distr->weightsize()), 
    input_value(the_distr->width()),
    input_gradient(the_distr->width()),
    output_value(the_f->outputs[0]->size())
{
    build_();
}

void
SumOfVariable::build()
{
    inherited::build();
    build_();
}

void
SumOfVariable::build_()
{
    if (f && distr) {
        input_value.resize(distr->inputsize() + distr->targetsize() + distr->weightsize());
        input_gradient.resize(distr->inputsize() + distr->targetsize() + distr->weightsize());
        if(f->outputs.size() != 1)
            PLERROR("In SumOfVariable: function must have a single variable output (maybe you can vconcat the vars into a single one prior to calling sumOf, if this is really what you want)");

        if(nsamples == -1)
            nsamples = distr->length();
        f->inputs.setDontBpropHere(true);
    }
}

void
SumOfVariable::declareOptions(OptionList &ol)
{
    declareOption(ol, "distr", &SumOfVariable::distr, OptionBase::buildoption, "");
    declareOption(ol, "f", &SumOfVariable::f, OptionBase::buildoption, "");
    declareOption(ol, "nsamples", &SumOfVariable::nsamples, OptionBase::buildoption, "");
    declareOption(ol, "curpos", &SumOfVariable::curpos, OptionBase::buildoption, "");
    inherited::declareOptions(ol);
}


void SumOfVariable::recomputeSize(int& l, int& w) const
{
    if (f && f->outputs.size()) {
        l = f->outputs[0]->length();
        w = f->outputs[0]->width();
    } else
        l = w = 0;
}


void SumOfVariable::makeDeepCopyFromShallowCopy(map<const void*, void*>& copies)
{
  NaryVariable::makeDeepCopyFromShallowCopy(copies);
  deepCopyField(distr, copies);
  deepCopyField(f, copies);
}


void SumOfVariable::fprop()
{
  f->recomputeParents();

  if(nsamples==1)
  {
    input_value.resize(distr->width());
    distr->getRow(curpos, input_value);
    input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
    f->fprop(input_value, value);
    if(++curpos == distr->length())
      curpos = 0;
  }
  else
  {
    value.clear();
#if USING_MPI
    if (nsamples > distr->length())
      PLERROR("In SumOfVariable::fprop, the case where nsamples is greater than distr->length is not supported in parallel computation");
    int nb_sample = nsamples/PLMPI::size;
    int start_pos = PLMPI::rank * nb_sample;
    int end_pos = (PLMPI::rank==PLMPI::size-1) ? nsamples : start_pos + nb_sample;
    Vec dummy_value(value.length());
    for(int i=start_pos; i<end_pos; i++)
    {
      input_value.resize(distr->width());
      distr->getRow(i, input_value);
      input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
      f->fprop(input_value, output_value);
      dummy_value += output_value;
    }
    MPI_Allreduce(dummy_value.data(), value.data(), value.length(), PLMPI_REAL, MPI_SUM, MPI_COMM_WORLD);
#else
    for(int i=0; i<nsamples; i++)
    {
      input_value.resize(distr->width());
      distr->getRow(curpos, input_value);
      input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
      f->fprop(input_value, output_value);
      value += output_value;
      if(++curpos == distr->length())
        curpos = 0;
    }
#endif
  }
}


void SumOfVariable::bprop()
{ fbprop(); }


void SumOfVariable::fbprop()
{
  f->recomputeParents();
  
  if(nsamples==1)
  {
    input_value.resize(distr->width());
    distr->getRow(curpos, input_value);
    input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
    //displayFunction(f, true, false, 250);
    f->fbprop(input_value, value, input_gradient, gradient);
    //displayFunction(f, true, false, 250);
    if(++curpos == distr->length()) 
      curpos = 0;
  }
  else
  {
    value.clear();
#if USING_MPI
    if (nsamples > distr->length())
      PLERROR("In SumOfVariable::fbprop, the case where nsamples is greater than distr->length is not supported in parallel computation");
    int nb_sample = nsamples/PLMPI::size;
    int start_pos = PLMPI::rank * nb_sample;
    int end_pos = (PLMPI::rank==PLMPI::size-1) ? nsamples : start_pos + nb_sample;
    Vec dummy_value(value.length());
    for(int i=start_pos; i<end_pos; i++)
    {
      input_value.resize(distr->width());
      distr->getRow(i, input_value);
      input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
      f->fbprop(input_value, output_value, input_gradient, gradient);
      dummy_value += output_value;
    }
    MPI_Allreduce(dummy_value.data(), value.data(), value.length(), PLMPI_REAL, MPI_SUM, MPI_COMM_WORLD);
    VarArray params = f->parameters;
    for (int i=0; i<params->length(); i++)
    {
      Vec buffer(params[i]->size());
      MPI_Reduce(params[i]->gradientdata, buffer.data(), buffer.length(), PLMPI_REAL, MPI_SUM, 0, MPI_COMM_WORLD);
      buffer >> params[i]->gradient;
      MPI_Bcast(params[i]->gradientdata, buffer.length(), PLMPI_REAL, 0, MPI_COMM_WORLD);
    }
#else
    for(int i=0; i<nsamples; i++)
    {
      input_value.resize(distr->width());
      distr->getRow(curpos, input_value);
      input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
      static bool display_fn=false;
      if (display_fn)
        displayFunction(f, true, false, 250);
      f->fbprop(input_value, output_value, input_gradient, gradient);
      value += output_value;
      if(++curpos == distr->length()) 
        curpos = 0;
    }
#endif
  }
}


void SumOfVariable::symbolicBprop()
{
  /*
  // f is a function of its inputs, what we want is a function of the parameters of f (which are in the inputs field of this SumOfVariable)
  VarArray& params = varray; 
  int nparams = params.size();
  f->bproppath.symbolicBprop();

  VarArray dparams(nparams);    
  for(int i=0; i<nparams; i++)
    dparams[i] = params[i]->g;

  Var dparams_concat = new ConcatElementsVariable(dparams);
  Var dparams_sum = new SumOfVariable(distr, Func(params,dparams_concat), nsamples);

  for(int i=0; i<nparams; i++)
    params[i]->g += dparams_sum.sub(...)
  */
}


void SumOfVariable::rfprop()
{
  if (rValue.length()==0) resizeRValue();
  // TODO... (we will need a rfprop() in Func)
  
//    f->recomputeParents();
  
//    if(nsamples==1)
//    {
//      distr->getRow(curpos, input_value);
//      f->fprop(input_value, value);
//      if(++curpos == distr->length())
//        curpos = 0;
//    }
//    else
//    {
//      value.clear();
//  #if USING_MPI
//      if (nsamples > distr->length())
//        PLERROR("In SumOfVariable::fprop, the case where nsamples is greater than distr->length is not supported in parallel computation");
//      int nb_sample = nsamples/PLMPI::size;
//      int start_pos = PLMPI::rank * nb_sample;
//      int end_pos = (PLMPI::rank==PLMPI::size-1) ? nsamples : start_pos + nb_sample;
//      Vec dummy_value(value.length());
//      for(int i=start_pos; i<end_pos; i++)
//      {
//        distr->getRow(i, input_value);
//        f->fprop(input_value, output_value);
//        dummy_value += output_value;
//      }
//      MPI_Allreduce(dummy_value.data(), value.data(), value.length(), PLMPI_REAL, MPI_SUM, MPI_COMM_WORLD);
//  #else
//      for(int i=0; i<nsamples; i++)
//      {
//        distr->getRow(curpos, input_value);
//        f->fprop(input_value, output_value);
//        value += output_value;
//        if(++curpos == distr->length())
//          curpos = 0;
//      }
//  #endif
//    }
}


void SumOfVariable::printInfo(bool print_gradient)
{
  Vec input_value(distr->width());
  Vec input_gradient(distr->width());
  Vec output_value(nelems());

  f->recomputeParents();
  value.clear();

  for(int i=0; i<nsamples; i++)
  {
    input_value.resize(distr->width());
    distr->getRow(curpos++,input_value);
    input_value.resize(distr->inputsize()+distr->targetsize()+distr->weightsize());
    if (print_gradient)
      f->fbprop(input_value, output_value, input_gradient, gradient);
    else
      f->fprop(input_value, output_value);
    value += output_value;
    if(curpos>=distr->length())
      curpos = 0;
    f->fproppath.printInfo(print_gradient);
  }
  cout << info() << " : " << getName() << " = " << value;
  if (print_gradient) cout << " gradient=" << gradient;
  cout << endl; 
}



} // end of namespace PLearn


