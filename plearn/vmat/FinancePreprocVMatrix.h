
// -*- C++ -*-

// FinancePreprocVMatrix.h
//
// Copyright (C) 2003  Rejean Ducharme 
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
   * $Id: FinancePreprocVMatrix.h,v 1.6 2004/03/17 16:06:32 ducharme Exp $ 
   ******************************************************* */

/*! \file FinancePreprocVMatrix.h */
#ifndef FinancePreprocVMatrix_INC
#define FinancePreprocVMatrix_INC

#include "RowBufferedVMatrix.h"
#include "VMat.h"

namespace PLearn {
using namespace std;

class FinancePreprocVMatrix: public RowBufferedVMatrix
{
public:

  VMat underlying; //! the underlying vmat
  TVec<string> asset_name; //! all the asset names
  bool add_tradable; //! do we include the information telling if this day is tradable or not
  bool add_last_day_of_month; //! do we include the information about the last tradable day of the month
  bool add_moving_average; //! do we include the moving average statistics on the price_tag indexes
  bool add_rollover_info; //! add a column with '1' when the rollover occur (new expiration date)
  int min_volume_threshold; //! tradable = 1 if volume>min_volume_threshold
  TVec<string> prices_tag; //! all the price tags on which we want to compute the moving average (e.g. close:level)
  TVec<int> moving_average_window; //! the window size of the moving average
  string volume_tag; //! "volume" by default
  string date_tag; //! "Date" by default.  Only used if add_last_day_of_month==true
  string expiration_tag; //! "expiration-date" by default.  Only used if add_rollover_info==true
  int last_day_cutoff; //! 0 by default (last day of month).  Set last_day=15 to simulate last_day_of_month as the 15 of each month
  bool last_date_is_last_day; //! is the last day a last day of month? (default=false)

protected:

  TVec<int> volume_index; //! the indexes (in the vmat) of all the volume columns
  TVec<int> price_index; //! the indexes of all the prices on which we want to compute some stats
  TVec<int> expiration_index; //! the index of the expiration-date (related to the expiration_tag)
  Vec last_day_of_month_index; //! the index of all the last tradable day of the month, base on the date column of the underlying matrix
  int max_moving_average_window; //! the maximum value of moving_average_window
  TVec< TVec<int> > rollover_date; //! the position (date) of all rollover
  Vec row_buffer; //! a row buffer for the getRow method

public:

  // ****************
  // * Constructors *
  // ****************

  // Default constructor, make sure the implementation in the .cc
  // initializes all fields to reasonable default values.
  FinancePreprocVMatrix();

  //! Simple constructor: takes as input only the underlying matrix,
  //! the number of assets and the threshold on the volume.
  FinancePreprocVMatrix(VMat vm, TVec<string> the_asset_names,
      bool add_tradable_info=true, bool add_last_day=false,
      bool add_moving_average_stats=false, bool add_roll_over_info=false,
      int threshold=20, TVec<string> the_price_tags=TVec<string>(),
      TVec<int> moving_average_window_length=TVec<int>(),
      string the_volume_tag="volume:level", string the_date_tag="Date",
      string the_expiration_tag="expiration-date", int the_last_day_cutoff=0,
      bool last_date_is_a_last_day=false);

  // ******************
  // * Object methods *
  // ******************

private: 
  //! This does the actual building. 
  // (Please implement in .cc)
  void build_();

protected: 
  //! Declares this class' options
  // (Please implement in .cc)
  static void declareOptions(OptionList& ol);

  void setVMFields();

public:
  virtual void getRow(int i, Vec v) const;

  // simply calls inherited::build() then build_() 
  virtual void build();

  //! Provides a help message describing this class
  static string help();

  //! Transforms a shallow copy into a deep copy
  virtual void makeDeepCopyFromShallowCopy(map<const void*, void*>& copies);

  //! Declares name and deepCopy methods
  PLEARN_DECLARE_OBJECT(FinancePreprocVMatrix);
  typedef RowBufferedVMatrix inherited;

};
DECLARE_OBJECT_PTR(FinancePreprocVMatrix);

} // end of namespace PLearn
#endif
