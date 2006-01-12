// -*- C++ -*-

// PLearn (A C++ Machine Learning Library)
// Copyright (C) 2003,2006 Olivier Delalleau

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
 * This file is part of the PLearn library.
 ******************************************************* */

#include "ConjGradientOptimizer.h"
//#include <plearn/math/TMat_maths_impl.h>

namespace PLearn {
using namespace std;

///////////////////////////
// ConjGradientOptimizer //
///////////////////////////
ConjGradientOptimizer::ConjGradientOptimizer():
    constrain_limit(0.1),
    expected_red(1),
    max_eval_per_line_search(20),
    max_extrapolate(3),
    no_negative_gamma(true),
    rho(1e-2),
    sigma(0.5),
    slope_ratio(100),
    verbosity(0),
    line_search_failed(false),
    line_search_succeeded(false)
{}

PLEARN_IMPLEMENT_OBJECT(ConjGradientOptimizer,
    "Optimizer based on the conjugate gradient method.",
    "The conjugate gradient algorithm is basically the following :\n"
    "- 0: initialize the search direction d = -gradient\n"
    "- 1: perform a line search along direction d for the minimum of the\n"
    "     function value\n"
    "- 2: move to this minimum, update the search direction d and go to\n"
    "     step 1\n"
    "The line search algorithm is inspired by Carl Rasmussen's Matlab\n"
    "algorithm from:\n"
    "http://www.kyb.tuebingen.mpg.de/bs/people/carl/code/minimize/minimize.m\n"
    "\n"
    "Many options can be set, however the provided default values should\n"
    "be adequate in most cases.\n"
);

////////////////////
// declareOptions //
////////////////////
void ConjGradientOptimizer::declareOptions(OptionList& ol)
{
    declareOption(ol, "verbosity", &ConjGradientOptimizer::verbosity,
                                   OptionBase::buildoption, 
        "Controls the amount of output.");

    declareOption(ol, "expected_red", &ConjGradientOptimizer::expected_red,
                                      OptionBase::buildoption, 
        "Expected function reduction at first step.");

    declareOption(ol, "no_negative_gamma",
                  &ConjGradientOptimizer::no_negative_gamma,
                  OptionBase::buildoption,
        "If true, then a negative value for gamma in the Polak-Ribiere\n"
        "formula will trigger a restart.");

    declareOption(ol, "sigma", &ConjGradientOptimizer::sigma,
                               OptionBase::buildoption, 
        "Constant in the Wolfe-Powell stopping conditions.");

     declareOption(ol, "rho", &ConjGradientOptimizer::rho,
                              OptionBase::buildoption, 
        "Constant in the Wolfe-Powell stopping conditions.");

     declareOption(ol, "constrain_limit",
                   &ConjGradientOptimizer::constrain_limit,
                   OptionBase::buildoption, 
        "Multiplicative coefficient to constrain the evaluation bracket.");

     declareOption(ol, "max_extrapolate",
                   &ConjGradientOptimizer::max_extrapolate,
                   OptionBase::buildoption, 
        "Maximum coefficient for bracket extrapolation.");

     declareOption(ol, "max_eval_per_line_search",
                   &ConjGradientOptimizer::max_eval_per_line_search,
                  OptionBase::buildoption, 
        "Maximum number of function evalutions during line search.");

     declareOption(ol, "slope_ratio", &ConjGradientOptimizer::slope_ratio,
                                  OptionBase::buildoption, 
        "Maximum slope ratio.");

    inherited::declareOptions(ol);
}


////////////
// build_ //
////////////
void ConjGradientOptimizer::build_() {
    // Make sure the internal data have the right size.
    int n = params.nelems();
    current_opp_gradient.resize(n);
    search_direction.resize(n);
    tmp_storage.resize(n);
    delta.resize(n);
}

//////////////////////////////
// computeCostAndDerivative //
//////////////////////////////
void ConjGradientOptimizer::computeCostAndDerivative(
    real alpha, real& cost, real& derivative) {
    if (fast_exact_is_equal(alpha, 0)) {
        cost = this->current_cost;
        derivative = -dot(this->search_direction, this->current_opp_gradient);
    } else {
        this->params.copyTo(this->tmp_storage);
        this->params.update(alpha, this->search_direction);
        computeGradient(this->delta);
        cost = this->cost->value[0];
        derivative = dot(this->search_direction, this->delta);
        this->params.copyFrom(this->tmp_storage);
    }
}

//////////////////////
// computeCostValue //
//////////////////////
real ConjGradientOptimizer::computeCostValue(real alpha)
{
    if (fast_exact_is_equal(alpha, 0))
        return this->current_cost;
    this->params.copyTo(this->tmp_storage);
    this->params.update(alpha, this->search_direction);
    this->proppath.fprop();
    real c = this->cost->value[0];
    this->params.copyFrom(this->tmp_storage);
    return c;
}

///////////////////////
// computeDerivative //
///////////////////////
real ConjGradientOptimizer::computeDerivative(real alpha)
{
    if (fast_exact_is_equal(alpha, 0))
        return -dot(this->search_direction, this->current_opp_gradient);
    this->params.copyTo(this->tmp_storage);
    this->params.update(alpha, this->search_direction);
    computeGradient(this->delta);
    this->params.copyFrom(this->tmp_storage);
    return dot(this->search_direction, this->delta);
}

///////////////////
// findDirection //
///////////////////
void ConjGradientOptimizer::findDirection() {
    real gamma = polakRibiere();
    if (gamma < 0 && no_negative_gamma) {
        if (verbosity >= 2)
            pout << "gamma = " << gamma << " < 0 ==> Restarting" << endl;
        gamma = 0;
    }
    /*
    // Old code triggering restart.
    else {
        real dp = dot(delta, current_opp_gradient);
        real delta_n = pownorm(delta);
        if (abs(dp) > restart_coeff *delta_n ) {
            if (verbosity >= 5)
                cout << "Restart triggered !" << endl;
            gamma = 0;
        }
    }
    */
    updateSearchDirection(gamma);
}

////////////////////////
// minimizeLineSearch //
////////////////////////
real ConjGradientOptimizer::minimizeLineSearch()
{
    // We may need to perform two iterations of line search if the first one
    // fails.
    bool try_again = true;
    while (try_again) {
        try_again = false;
        real fun_val0 = fun_val1;
        computeCostAndDerivative(step1, fun_val2, fun_deriv2);
        real fun_val3 = fun_val1;
        real fun_deriv3 = fun_deriv1;
        real step3 = - step1;
        fun_eval_count = max_eval_per_line_search;
        line_search_succeeded = false;
        bracket_limit = -1;
        while (true) {
            while ( (fun_val2 > fun_val1 + step1 * rho * fun_deriv1 ||
                     fun_deriv2 > - sigma * fun_deriv1 ) &&
                    fun_eval_count > 0 )
            {
                // Tighten bracket.
                bracket_limit = step1;
                if (fun_val2 > fun_val1) {
                    // Quadratic fit.
                    step2 = step3 -
                        (0.5*fun_deriv3*step3*step3) / 
                        (fun_deriv3*step3+fun_val2-fun_val3);
                } else {
                    // Cubic fit.
                    cubic_a = 6*(fun_val2-fun_val3)/step3 +
                              3*(fun_deriv2+fun_deriv3);
                    cubic_b = 3*(fun_val3-fun_val2) -
                              step3*(fun_deriv3+2*fun_deriv2);
                    step2 =
                        (sqrt(cubic_b*cubic_b-cubic_a*fun_deriv2*step3*step3) -
                         cubic_b) / cubic_a;
                }
                if (isnan(step2) || isinf(step2))
                    // Shit happens => bisection.
                    step2 = step3/2;
                // Constrained range.
                step2 = max(min(step2, constrain_limit*step3),
                            (1-constrain_limit)*step3);
                // Increase step and update function value and derivative.
                step1 += step2;
                computeCostAndDerivative(step1, fun_val2, fun_deriv2);
                // Update point 3.
                step3 = step3 - step2;  
                fun_eval_count--;
            }
            if (fun_val2 > fun_val1+step1*rho*fun_deriv1 ||
                fun_deriv2 > -sigma*fun_deriv1)
                // Failure.
                break;
            else if (fun_deriv2 > sigma * fun_deriv1) {
                // Sucesss.
                line_search_succeeded = true;
                break;
            } else if (fun_eval_count == 0)
                // Failure.
                break;
            // Cubic fit.
            cubic_a = 6*(fun_val2-fun_val3)/step3+3*(fun_deriv2+fun_deriv3);
            cubic_b = 3*(fun_val3-fun_val2)-step3*(fun_deriv3+2*fun_deriv2);
            step2 = -fun_deriv2*step3*step3 /
                (cubic_b +
                 sqrt(cubic_b*cubic_b-cubic_a*fun_deriv2*step3*step3));
            if (isnan(step2) || isinf(step2) || step2 < 0) {
                // Numerical issue, or wrong sign.
                if (bracket_limit < -0.5)
                    // No upper limit.
                    step2 = step1 * (max_extrapolate - 1);
                else
                    step2 = (bracket_limit - step1) / 2;
            } else if (bracket_limit > -0.5 && (step2 + step1 > bracket_limit))
                // Extrapolation beyond maximum.
                step2 = (bracket_limit - step1) / 2;
            else if (bracket_limit < -0.5 &&
                     step2+step1 > step1 * max_extrapolate) {
                // Extrapolation beyond limit.
                step2 = step1 * (max_extrapolate - 1);
            } else if (step2 < - step3 * constrain_limit) {
                step2 = - step3 * constrain_limit;
                // % too close to limit?
            } else if (bracket_limit > -0.5 &&
                       step2 < (bracket_limit - step1) * (1 - constrain_limit))
                // Too close to limit.
                step2 = (bracket_limit - step1) * (1 - constrain_limit);
            // Point 3 = point 2.
            fun_val3 = fun_val2;
            fun_deriv3 = fun_deriv2;
            step3 = - step2;
            // Update step and function value and derivative.
            step1 += step2;
            computeCostAndDerivative(step1, fun_val2, fun_deriv2);
            fun_eval_count--;
        }

        if (line_search_succeeded) {
            fun_val1 = fun_val2;
            line_search_failed = false;
        } else {
            // Come back to initial point.
            fun_val1 = fun_val0;
            // If it is the second time it fails, then we cannot do better.
            if (line_search_failed)
                return 0;
            // Original code:
            // tmp = df1; df1 = df2; df2 = tmp; % swap derivatives
            // s = -df1; % try steepest
            // d1 = -s'*s;
            // We do not do that... it looks weird!
            // We will actually do s = -df0 as this seems more logical.
            // TODO See Carl Rasmussen's answer to email...
            fun_deriv1 = - pownorm(current_opp_gradient);
            step1 = 1 / (1 - fun_deriv1);
            line_search_failed = true;
            try_again = true;
        }
    }
    return step1;
}

////////////////
// lineSearch //
////////////////
bool ConjGradientOptimizer::lineSearch() {
    real step = minimizeLineSearch();
    if (step < 0)
        // Hopefully this will not happen.
        PLWARNING("Negative step!");
    bool no_improvement_possible = fast_exact_is_equal(step, 0);
    if (no_improvement_possible) {
        if (verbosity >= 2)
            pout << "No more progress made by the line search, stopping" << endl;
    } else
        params.update(step, search_direction);
    return !no_improvement_possible;
}

/////////////////////////////////
// makeDeepCopyFromShallowCopy //
/////////////////////////////////
void ConjGradientOptimizer::makeDeepCopyFromShallowCopy(CopiesMap& copies)
{
    inherited::makeDeepCopyFromShallowCopy(copies);
    PLERROR("In ConjGradientOptimizer::makeDeepCopyFromShallowCopy - Not "
            "implementented");
}

///////////////
// optimizeN //
///////////////
bool ConjGradientOptimizer::optimizeN(VecStatsCollector& stats_coll) {
    int stage_max = stage + nstages; // The stage to reach.

    if (stage == 0)
    {
        computeOppositeGradient(current_opp_gradient);
        // First search direction = - gradient.
        search_direction <<  current_opp_gradient;
        current_cost = cost->value[0];

        fun_val1 = current_cost;
        fun_deriv1 = - pownorm(search_direction);
        step1 = expected_red / ( 1 - fun_deriv1 );
    }

    if (early_stop) {
        // The 'early_stop' flag is already set: we must still update the stats
        // collector with the current cost value.
        this->proppath.fprop();
        stats_coll.update(cost->value);    
    }

    for (; !early_stop && stage<stage_max; stage++) {
        // Make a line search along the current search direction.
        early_stop = !lineSearch();
        // Ensure 'delta' contains the opposite gradient at the new point
        // reached after the line search.
        // Also update 'current_cost'.
        computeOppositeGradient(delta);
        current_cost = cost->value[0];
        // Display current cost value if required.
        if (verbosity >= 2)
            pout << "ConjGradientOptimizer - stage " << stage << ": "
                 << current_cost << endl;
        stats_coll.update(cost->value);
    
        // Find the new search direction if we need to continue.
        if (!early_stop)
            findDirection();
    }

    if (early_stop && verbosity >= 2)
        pout << "Early Stopping!" << endl;

    return early_stop;
}

//////////////////
// polakRibiere //
//////////////////
real ConjGradientOptimizer::polakRibiere()
{
    real normg = pownorm(this->current_opp_gradient);
    // At this point, delta = opposite gradient at new point.
    this->tmp_storage << this->delta;
    this->tmp_storage -= this->current_opp_gradient;
    return dot(this->tmp_storage, this->delta) / normg;
}

///////////
// reset //
///////////
void ConjGradientOptimizer::reset() {
    inherited::reset();
    line_search_failed = false;
    line_search_succeeded = false;
}

///////////////////////////
// updateSearchDirection //
///////////////////////////
void ConjGradientOptimizer::updateSearchDirection(real gamma) {
    if (fast_exact_is_equal(gamma, 0))
        search_direction << delta;
    else
        for (int i=0; i<search_direction.length(); i++)
            search_direction[i] = delta[i] + gamma * search_direction[i];

    // Update 'current_opp_gradient' for the new current point.
    current_opp_gradient << delta;
    fun_deriv2 = - dot(current_opp_gradient, search_direction);
    if (fun_deriv2 > 0) {
        search_direction << current_opp_gradient;
        fun_deriv2 = - pownorm(search_direction);
    }
    step1 = step1 * min(slope_ratio, fun_deriv1/(fun_deriv2-REAL_EPSILON));
    fun_deriv1 = fun_deriv2;
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
