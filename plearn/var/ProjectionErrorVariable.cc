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
 * $Id: ProjectionErrorVariable.cc,v 1.12 2004/07/31 13:36:55 larocheh Exp $
 * This file is part of the PLearn library.
 ******************************************************* */

#include "ProjectionErrorVariable.h"
#include "Var_operators.h"
#include <plearn/math/plapack.h>
//#include "Var_utils.h"

namespace PLearn {
  using namespace std;


  /** ProjectionErrorVariable **/

  PLEARN_IMPLEMENT_OBJECT(ProjectionErrorVariable,
                          "Computes the projection error of a set of vectors on a non-orthogonal basis.\n",
                          "The first input is a set of n_dim vectors (possibly seen as a single vector of their concatenation) f_i, each in R^n\n"
                          "The second input is a set of T vectors (possibly seen as a single vector of their concatenation) t_j, each in R^n\n"
                          "If !use_subspace_distance, the output is the following:\n"
                          "    sum_j min_w || t_j - sum_i w_i f_i ||^2\n"
                          "where w is a local n_dim-vector that is optmized analytically. However, if use_subspace_distance, the output is\n"
                          "     min_{w,u}  || sum_i w_i f_i  -  sum_j u_j t_j ||^2.\n"
                          "In any case, if norm_penalization>0, an extra term is added:\n"
                          "    norm_penalization * sum_i (||f_i||^2 - 1)^2.\n");
  
    ProjectionErrorVariable::ProjectionErrorVariable(Variable* input1, Variable* input2, int n_, bool normalize_by_neighbor_distance_, bool use_subspace_distance_, real norm_penalization_, real epsilon_, real regularization_)
    : inherited(input1, input2, 1, 1), n(n_), use_subspace_distance(use_subspace_distance_), normalize_by_neighbor_distance(normalize_by_neighbor_distance_), norm_penalization(norm_penalization_), epsilon(epsilon_), regularization(regularization)
  {
    build_();
  }

  void
  ProjectionErrorVariable::build()
  {
    inherited::build();
    build_();
  }

  void
  ProjectionErrorVariable::build_()
  {
    if (input1 && input2) {
      if ((input1->length()==1 && input1->width()>1) || 
          (input1->width()==1 && input1->length()>1))
        {
          if (n<0) PLERROR("ProjectionErrorVariable: Either the input should be matrices or n should be specified\n");
          n_dim = input1->size()/n;
          if (n_dim*n != input1->size())
            PLERROR("ProjectErrorVariable: the first input size should be an integer multiple of n");
        }
      else 
        n_dim = input1->length();
      if ((input2->length()==1 && input2->width()>1) || 
          (input2->width()==1 && input2->length()>1))
        {
          if (n<0) PLERROR("ProjectionErrorVariable: Either the input should be matrices or n should be specified\n");
          T = input2->size()/n;
          if (T*n != input2->size())
            PLERROR("ProjectErrorVariable: the second input size should be an integer multiple of n");
        }
      else 
        T = input2->length();

      F = input1->value.toMat(n_dim,n);
      dF = input1->gradient.toMat(n_dim,n);
      TT = input2->value.toMat(T,n);
      if (n<0) n = input1->width();
      if (input2->width()!=n)
        PLERROR("ProjectErrorVariable: the two arguments have inconsistant sizes");
      if (n_dim>n)
        PLERROR("ProjectErrorVariable: n_dim should be less than data dimension n");
      if (!use_subspace_distance)
        {
          V.resize(n_dim,n_dim);
          Ut.resize(n,n);
          B.resize(n_dim,n);
          VVt.resize(n_dim,n_dim);
          fw_minus_t.resize(T,n);
          w.resize(T,n_dim);
          one_over_norm_T.resize(T);
        }
      else 
        {
          wwuu.resize(n_dim+T);
          ww = wwuu.subVec(0,n_dim);
          uu = wwuu.subVec(n_dim,T);
          wwuuM = wwuu.toMat(1,n_dim+T);
          rhs.resize(n_dim+T);
          rhs.subVec(0,n_dim).fill(-1.0);
          A.resize(n_dim+T,n_dim+T);
          A11 = A.subMat(0,0,n_dim,n_dim);
          A12 = A.subMat(0,n_dim,n_dim,T);
          A21 = A.subMat(n_dim,0,T,n_dim);
          A22 = A.subMat(n_dim,n_dim,T,T);
          Tu.resize(n);
          FT.resize(n_dim+T,n);
          FT1 = FT.subMat(0,0,n_dim,n);
          FT2 = FT.subMat(n_dim,0,T,n);
          Ut.resize(n,n);
          V.resize(n_dim+T,n_dim+T);
        }
      fw.resize(n);
      if (norm_penalization>0)
        norm_err.resize(n_dim);
    }
  }


  void ProjectionErrorVariable::recomputeSize(int& len, int& wid) const
  {
    len = 1;
    wid = 1;
  }

  void ProjectionErrorVariable::fprop()
  {
    // Let F the input1 matrix with rows f_i.
    // IF use_subspace_distance THEN
    //  We need to solve the system
    //    | FF'  -FT'| |w|   | 1 |
    //    |          | | | = |   |
    //    |-TF'   TT'| |u|   | 0 |
    //  in (w,u), and then scale both down by ||w|| so as to enforce ||w||=1.
    //
    // ELSE
    //  We need to solve the system 
    //     F F' w_j = F t_j
    //  for each t_j in order to find the solution w of
    //    min_{w_j} || t_j - sum_i w_{ji} f_i ||^2
    //  for each j. Then sum over j the above square errors.
    //  Let F' = U S V' the SVD of F'. Then
    //    w_j = (F F')^{-1} F t_j = (V S U' U S V')^{-1} F t_j = V S^{-2} V' F t_j.
    //  Note that we can pre-compute
    //    B = V S^{-2} V' F = V S^{-1} U'
    //  and
    //    w_j = B t_j is our solution.
    // ENDIF
    //
    // if  norm_penalization>0 then also add the following term:
    //   norm_penalization * sum_i (||f_i||^2 - 1)^2
    //
    real cost = 0;
    if (use_subspace_distance)
      {
#if 0        
        productTranspose(A11,F,F);
        productTranspose(A12,F,TT);
        A12 *= -1.0;
        transpose(A12,A21);
        productTranspose(A22,TT,TT);
        if (epsilon>0)
          for (int i=0;i<A.length();i++)
            A[i][i] += epsilon;
        static TVec<int> pivots;
        wwuu << rhs;
        // note that A is symmetric, which is not exploited here
        lapackSolveLinearSystem(A, wwuuM, pivots);
        // note that A is destroyed here, but wwuu contains the solution
#else
        // use SVD of (F' -T') instead.
        FT1 << F;
        multiply(FT2,TT,-1.0);
        lapackSVD(FT, Ut, S, V);
        wwuu.clear();
        for (int k=0;k<S.length();k++)
          {
            real s_k = S[k];
            real sv = s_k+ regularization;
            real coef = 1/(sv * sv);
            if (s_k>epsilon) // ignore the components that have too small singular value (more robust solution)
              {
                real sum_first_elements = 0;
                for (int j=0;j<n_dim;j++) 
                  sum_first_elements += V(j,k);
                for (int i=0;i<n_dim+T;i++)
                  wwuu[i] += V(i,k) * sum_first_elements * coef;
              }
          }
#endif
        static bool debugging=false;
        if (debugging)
          {
            productTranspose(A11,F,F);
            productTranspose(A12,F,TT);
            A12 *= -1.0;
            Vec res(ww.length());
            product(res,A11,ww);
            productAcc(res,A12,uu);
            res -= 1.0;
            cout << "norm of error in w equations: " << norm(res) << endl;
            Vec res2(uu.length());
            transposeProduct(res2,A12,ww);
            productTranspose(A22,TT,TT);
            productAcc(res2,A22,uu);
            cout << "norm of error in u equations: " << norm(res2) << endl;
          }
        // scale w and u so that ||w|| = 1
        real wnorm = sum(ww); // norm(ww);
        wwuu *= 1.0/wnorm;

        // compute the cost = ||F'w - T'u||^2
        transposeProduct(fw,F,ww);
        transposeProduct(Tu,TT,uu);
        fw -= Tu;
        cost = pownorm(fw);
      }
    else // PART THAT IS REALLY USED STARTS HERE
      {
        static Mat F_copy;
        F_copy.resize(F.length(),F.width());
        F_copy << F;
        // N.B. this is the SVD of F'
        lapackSVD(F_copy, Ut, S, V);
        B.clear();
        for (int k=0;k<S.length();k++)
          {
            real s_k = S[k];
            if (s_k>epsilon) // ignore the components that have too small singular value (more robust solution)
              { 
                s_k += regularization;
                real coef = 1/s_k;
                for (int i=0;i<n_dim;i++)
                  {
                    real* Bi = B[i];
                    for (int j=0;j<n;j++)
                      Bi[j] += V(i,k)*Ut(k,j)*coef;
                  }
              }
          }
        //  now we have B, we can compute the w's and the cost
        for(int j=0; j<T;j++)
          {
            Vec tj = TT(j);
            Vec wj = w(j);
            product(wj, B, tj); // w_j = B * t_j = projection weights for neighbor j
            transposeProduct(fw, F, wj); // fw = sum_i w_ji f_i = z_m
            Vec fw_minus_tj = fw_minus_t(j);
            substract(fw,tj,fw_minus_tj); // -z_n = z_m - z
            if (normalize_by_neighbor_distance) // THAT'S THE ONE WHICH WORKS WELL:
              {
                one_over_norm_T[j] = 1.0/pownorm(tj); // = 1/||z||
                cost += sumsquare(fw_minus_tj)*one_over_norm_T[j]; // = ||z_n||^2 / ||z||^2
              }
            else
                cost += sumsquare(fw_minus_tj);
          }
      }
    if (norm_penalization>0)
    {
      real penalization=0;
      for (int i=0;i<n_dim;i++)
        {
          Vec f_i = F(i);
          norm_err[i] = pownorm(f_i)-1;
          penalization += norm_err[i]*norm_err[i];
        }
      cost += norm_penalization*penalization;
    }
    value[0] = cost;
  }


  void ProjectionErrorVariable::bprop()
  {
    // calcule dcost/F et incremente input1->matGadient avec cette valeur
    // keeping w fixed
    // 
    // IF use_subspace_distance
    //   dcost/dF = w (F'w - T'u)'
    //
    // ELSE
    //   dcost/dfw = 2 (fw - t_j)/||t_j||
    //   dfw/df_i = w_i 
    //  so 
    //   dcost/df_i = sum_j 2(fw - t_j) w_i/||t_j||
    //
    // IF norm_penalization>0
    //   add the following to the gradient of f_i:
    //     norm_penalization*2*(||f_i||^2 - 1)*f_i
    // N.B. WE CONSIDER THE input2 (t_j's) TO BE FIXED AND DO NOT 
    // COMPUTE THE GRADIENT WRT to input2. IF THE USE OF THIS
    // OBJECT CHANGES THIS MAY HAVE TO BE REVISED.
    //

    if (use_subspace_distance)
      {
        externalProductScaleAcc(dF,ww,fw,gradient[0]);
        if (norm_penalization>0)
          for (int i=0;i<n_dim;i++)
          {
            Vec df_i = dF(i); // n-vector
              multiplyAcc(df_i, F(i), gradient[0]*norm_penalization*2*norm_err[i]);
          }
      }
    else
      {
        for (int j=0;j<T;j++)
          {
            Vec fw_minus_tj = fw_minus_t(j); // n-vector
            Vec wj = w(j);
            for (int i=0;i<n_dim;i++)
              {
                Vec df_i = dF(i); // n-vector
                if (normalize_by_neighbor_distance)
                  multiplyAcc(df_i, fw_minus_tj, gradient[0] * wj[i]*2*one_over_norm_T[j]);
                else
                  multiplyAcc(df_i, fw_minus_tj, gradient[0] * wj[i]*2);
                if (norm_penalization>0)
                  multiplyAcc(df_i, F(i), gradient[0]*norm_penalization*2*norm_err[i]);
              }
          }
      }
  }


  void ProjectionErrorVariable::symbolicBprop()
  {
    PLERROR("Not implemented");
  }

} // end of namespace PLearn


