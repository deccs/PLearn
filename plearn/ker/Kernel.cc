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
   * $Id: Kernel.cc,v 1.8 2003/12/15 22:08:32 dorionc Exp $
   * This file is part of the PLearn library.
   ******************************************************* */

#include "Kernel.h"

// From Old Kernel.cc: all includes are putted in every file.
// To be revised manually 
#include <cmath>
#include "stringutils.h"
#include "Kernel.h"
#include "TMat_maths.h"
#include "PLMPI.h"
//////////////////////////
namespace PLearn <%
using namespace std;

using namespace std;
  
PLEARN_IMPLEMENT_ABSTRACT_OBJECT(Kernel, "ONE LINE DESCR", "NO HELP");
Kernel::~Kernel() {}


void Kernel::declareOptions(OptionList &ol)
{
  declareOption(ol, "is_symmetric", &Kernel::is_symmetric, OptionBase::buildoption,
                "TODO: Give some comments");
  
  declareOption(ol, "is_sequential", &Kernel::is_sequential, OptionBase::buildoption,
                "To be set true if the kernel is to be used is a sequential context.\n"
                "That is if the data matrix is meant to be grown from the previous one\n"
                "at each call to setDataForKernelMatrix. Given this information,\n"
                "the kernel may not recompute informations still true from the previous data matrix.");
  
  inherited::declareOptions(ol);
}


void Kernel::setDataForKernelMatrix(VMat the_data)
{ 
  if(data.isNotNull() && is_sequential && the_data.length() < data.length())
    PLERROR("The Kernel::is_sequential flag was set to true but the new data\n"
            "matrix is shorter (%d) than the previous one (%d)", the_data.length(), data.length());
  data = the_data; 
}


real Kernel::evaluate_i_j(int i, int j) const
{ return evaluate(data(i),data(j)); }


real Kernel::evaluate_i_x(int i, const Vec& x, real squared_norm_of_x) const 
{ return evaluate(data(i),x); }


real Kernel::evaluate_x_i(const Vec& x, int i, real squared_norm_of_x) const
{ 
  if(is_symmetric)
    return evaluate_i_x(i,x,squared_norm_of_x);
  else
    return evaluate(x,data(i));
}


void Kernel::computeGramMatrix(Mat K) const
{
  if (!data) PLERROR("Kernel::computeGramMatrix should be called only after setDataForKernelMatrix");
  int l=data->length();
  int m=K.mod();
  for (int i=0;i<l;i++)
  {
    real* Ki = K[i];
    real* Kji_ = &K[0][i];
    for (int j=0;j<=i;j++,Kji_+=m)
    {
      real Kij = evaluate_i_j(i,j);
      Ki[j]=Kij;
      if (j<i)
        *Kji_ =Kij;
    }
  }
}


void Kernel::setParameters(Vec paramvec)
{ PLERROR("setParameters(Vec paramvec) not implemented for this kernel"); }


Vec Kernel::getParameters() const
{ return Vec(); }


void 
Kernel::apply(VMat m1, VMat m2, Mat& result) const
{
  result.resize(m1->length(), m2->length());
  Vec m1_i(m1->width());
  Vec m2_j(m2->width());
  if(is_symmetric && m1==m2)
    {
      for(int i=0; i<m1->length(); i++)
        {
          m1->getRow(i,m1_i);
          for(int j=0; j<=i; j++)
            {
              m2->getRow(j,m2_j);
              real val = evaluate(m1_i,m2_j);
              result(i,j) = val;
              result(j,i) = val;
            }
        }
    }
  else
    {
      for(int i=0; i<m1->length(); i++)
        {
          m1->getRow(i,m1_i);
          for(int j=0; j<m2->length(); j++)
            {
              m2->getRow(j,m2_j);
              result(i,j) = evaluate(m1_i,m2_j);
            }
        }
    }
}


void 
Kernel::apply(VMat m, const Vec& x, Vec& result) const
{
  result.resize(m->length());
  Vec m_i(m->width());
  for(int i=0; i<m->length(); i++)
    {
      m->getRow(i,m_i);
      result[i] = evaluate(m_i,x);
    }
}


void 
Kernel::apply(Vec x, VMat m, Vec& result) const
{
  result.resize(m->length());
  Vec m_i(m->width());
  for(int i=0; i<m->length(); i++)
    {
      m->getRow(i,m_i);
      result[i] = evaluate(x,m_i);
    }
}


Mat 
Kernel::apply(VMat m1, VMat m2) const
{
  Mat result;
  apply(m1,m2,result);
  return result;
}


real 
Kernel::test(VMat d, real threshold, real sameness_below_threshold, real sameness_above_threshold) const
{
  int nerrors = 0;
  int inputsize = (d->width()-1)/2;
  for(int i=0; i<d->length(); i++)
    {
      Vec inputs = d(i);
      Vec input1 = inputs.subVec(0,inputsize);
      Vec input2 = inputs.subVec(inputsize,inputsize);
      real sameness = inputs[inputs.length()-1];
      real kernelvalue = evaluate(input1,input2);
      cerr << "[" << kernelvalue << " " << sameness << "]\n";
      if(kernelvalue<threshold)
        {
          if(sameness==sameness_above_threshold)
            nerrors++;
        }
      else // kernelvalue>=threshold
        {
          if(sameness==sameness_below_threshold)
            nerrors++;
        }
    }
  return real(nerrors)/d->length();
}



Mat 
Kernel::computeNeighbourMatrixFromDistanceMatrix(const Mat& D, bool insure_self_first_neighbour)
{
  int npoints = D.length();
  Mat neighbours(npoints, npoints);  
  Mat tmpsort(npoints,2);
  
    //for(int i=0; i<2; i++)
  for(int i=0; i<npoints; i++)
    {
      for(int j=0; j<npoints; j++)
        {
          tmpsort(j,0) = D(i,j);
          tmpsort(j,1) = j;
        }
      if(insure_self_first_neighbour)
        tmpsort(i,0) = -FLT_MAX;

      sortRows(tmpsort);
      neighbours(i) << tmpsort.column(1);
    }
  return neighbours;
}


Mat 
Kernel::estimateHistograms(VMat d, real sameness_threshold, real minval, real maxval, int nbins) const
{
  real binwidth = (maxval-minval)/nbins;
  int inputsize = (d->width()-1)/2;
  Mat histo(2,nbins);
  Vec histo_below = histo(0);
  Vec histo_above = histo(1);
  int nbelow=0;
  int nabove=0;
  for(int i=0; i<d->length(); i++)
    {
      Vec inputs = d(i);
      Vec input1 = inputs.subVec(0,inputsize);
      Vec input2 = inputs.subVec(inputsize,inputsize);
      real sameness = inputs[inputs.length()-1];
      real kernelvalue = evaluate(input1,input2);
      if(kernelvalue>=minval && kernelvalue<maxval)
        {
          int binindex = int((kernelvalue-minval)/binwidth);
          if(sameness<sameness_threshold)
            {
              histo_below[binindex]++;
              nbelow++;
            }
          else
            {
              histo_above[binindex]++;
              nabove++;
            }
        }
    }
  histo_below /= real(nbelow);
  histo_above /= real(nabove);
  return histo;
}


Mat Kernel::estimateHistograms(Mat input_and_class, real minval, real maxval, int nbins) const
{
  real binwidth = (maxval-minval)/nbins;
  int inputsize = input_and_class.width()-1;
  Mat inputs = input_and_class.subMatColumns(0,inputsize);
  Mat classes = input_and_class.column(inputsize);
  Mat histo(4,nbins);
  Vec histo_mean_same = histo(0);
  Vec histo_mean_other = histo(1);
  Vec histo_min_same = histo(2);
  Vec histo_min_other = histo(3);

  for(int i=0; i<inputs.length(); i++)
    {
      Vec input = inputs(i);
      real input_class = classes(i,0);
      real sameclass_meandist = 0.0;
      real otherclass_meandist = 0.0;
      real sameclass_mindist = FLT_MAX;
      real otherclass_mindist = FLT_MAX;
      for(int j=0; j<inputs.length(); j++)
        if(j!=i)
          {
            real dist = evaluate(input, inputs(j));
            if(classes(j,0)==input_class)
              {
                sameclass_meandist += dist;
                if(dist<sameclass_mindist)
                  sameclass_mindist = dist;
              }
            else
              {
                otherclass_meandist += dist;
                if(dist<otherclass_mindist)
                  otherclass_mindist = dist;
              }
          }
      sameclass_meandist /= (inputs.length()-1);
      otherclass_meandist /= (inputs.length()-1);      
      if(sameclass_meandist>=minval && sameclass_meandist<maxval)
        histo_mean_same[int((sameclass_meandist-minval)/binwidth)]++;
      if(sameclass_mindist>=minval && sameclass_mindist<maxval)
        histo_min_same[int((sameclass_mindist-minval)/binwidth)]++;
      if(otherclass_meandist>=minval && otherclass_meandist<maxval)
        histo_mean_other[int((otherclass_meandist-minval)/binwidth)]++;
      if(otherclass_mindist>=minval && otherclass_mindist<maxval)
        histo_min_other[int((otherclass_mindist-minval)/binwidth)]++;
    }
  histo_mean_same /= sum(histo_mean_same);
  histo_min_same /= sum(histo_min_same);
  histo_mean_other /= sum(histo_mean_other);
  histo_min_other /= sum(histo_min_other);
  return histo;
}


void
Kernel::oldwrite(ostream& out) const
{
	writeHeader(out,"Kernel");
	writeField(out,"is_symmetric",is_symmetric);
	writeFooter(out,"Kernel");
}


void
Kernel::oldread(istream& in)
{
	readHeader(in,"Kernel");
	readField(in,"is_symmetric",is_symmetric);
	readFooter(in,"Kernel");
}

// last column of data is supposed to be a classnum
// returns a matrix of (index1, index2, distance)
Mat findClosestPairsOfDifferentClass(int k, VMat data, Ker dist)
{
  Mat result(k,3);
  real maxdistinlist = -FLT_MAX;
  int posofmaxdistinlist = -1;
  int kk=0; // number of pairs already in list
  Vec rowi(data.width());
  Vec inputi = rowi.subVec(0,rowi.length()-1);
  real& targeti = rowi[rowi.length()-1];
  Vec rowj(data.width());
  Vec inputj = rowj.subVec(0,rowj.length()-1);
  real& targetj = rowj[rowj.length()-1];
  for(int i=0; i<data.length(); i++)
  {
    data->getRow(i,rowi);
    for(int j=0; j<data.length(); j++)
    {
      data->getRow(j,rowj);
      if(targeti!=targetj)
      {
        real d = dist(inputi,inputj);
        if(kk<k)
        {
          result(kk,0) = i;
          result(kk,1) = j;
          result(kk,2) = d;
          if(d>maxdistinlist)
          {
            maxdistinlist = d;
            posofmaxdistinlist = kk;
          }
          kk++;
        }
        else if(d<maxdistinlist)
        {
          result(posofmaxdistinlist,0) = i;
          result(posofmaxdistinlist,1) = j;
          result(posofmaxdistinlist,2) = d;
          posofmaxdistinlist = argmax(result.column(2));
          maxdistinlist = result(posofmaxdistinlist,2);
        }
      }
    }
  }
  sortRows(result, 2);
  return result;
}

%> // end of namespace PLearn

