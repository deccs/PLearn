// -*- C++ -*-

// plearn_inc.h
//
// Copyright (C) 2004 Olivier Delalleau 
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
   * $Id: plearn_inc.h,v 1.57 2005/04/12 15:02:51 tihocan Exp $ 
   ******************************************************* */

// Authors: Olivier Delalleau

/*! \file plearn_inc.h */

/*! Include here all classes available in the PLearn CVS repository 
    that do NOT depend upon fancy external libraries.
 */

#ifndef plearn_inc_INC
#define plearn_inc_INC

// Version number.
#define PLEARN_MAJOR_VERSION 0
#define PLEARN_MINOR_VERSION 92
#define PLEARN_FIXLEVEL 0

/*****************
 * Miscellaneous *
 *****************/
#include <plearn_learners/misc/Grapher.h>
#include <plearn_learners/misc/VariableSelectionWithDirectedGradientDescent.h>
#include <plearn/math/ManualBinner.h>
#include <plearn/math/SoftHistogramBinner.h>
#include <plearn/misc/ShellScript.h>
#include <plearn/misc/RunObject.h>
#include <plearn/db/UCISpecification.h>
#include <plearn_learners/testers/PTester.h>

/***********
 * Command *
 ***********/
#include <commands/PLearnCommands/AutoRunCommand.h>
#include <commands/PLearnCommands/FieldConvertCommand.h>
#include <commands/PLearnCommands/HelpCommand.h>
#include <commands/PLearnCommands/JulianDateCommand.h>
#include <commands/PLearnCommands/KolmogorovSmirnovCommand.h>
#include <commands/PLearnCommands/LearnerCommand.h>
#include <commands/PLearnCommands/ReadAndWriteCommand.h>
#include <commands/PLearnCommands/RunCommand.h>
#include <commands/PLearnCommands/ServerCommand.h>
#include <commands/PLearnCommands/TestDependenciesCommand.h>
#include <commands/PLearnCommands/TestDependencyCommand.h>
//#include <commands/PLearnCommands/TxtmatCommand.h>


/**************
 * Dictionary *
 **************/
#include <plearn_learners/language/Dictionary/Dictionary.h>
#include <plearn_learners/language/Dictionary/FileDictionary.h>
#include <plearn_learners/language/Dictionary/VecDictionary.h>

/****************
 * HyperCommand *
 ****************/
#include <plearn_learners/hyper/HyperOptimize.h>
#include <plearn_learners/hyper/HyperRetrain.h>

/**********
 * Kernel *
 **********/
#include <plearn/ker/AdditiveNormalizationKernel.h>
#include <plearn/ker/DistanceKernel.h>
#include <plearn/ker/DotProductKernel.h>
#include <plearn/ker/EpanechnikovKernel.h>
#include <plearn/ker/GaussianKernel.h>
#include <plearn/ker/GeodesicDistanceKernel.h>
#include <plearn/ker/LLEKernel.h>
#include <plearn/ker/NegOutputCostFunction.h>
#include <plearn/ker/ReconstructionWeightsKernel.h>

/*************
 * Optimizer *
 *************/
#include <plearn/opt/AdaptGradientOptimizer.h>
#include <plearn/opt/ConjGradientOptimizer.h>
#include <plearn/opt/GradientOptimizer.h>

/****************
 * OptionOracle *
 ****************/
#include <plearn_learners/hyper/CartesianProductOracle.h>
#include <plearn_learners/hyper/EarlyStoppingOracle.h>
#include <plearn_learners/hyper/ExplicitListOracle.h>
#include <plearn_learners/hyper/OptimizeOptionOracle.h>

/************
 * PLearner *
 ************/

// Classifiers
#include <plearn_learners/meta/AdaBoost.h>
#include <plearn_learners/classifiers/BinaryStump.h>
#include <plearn_learners/classifiers/ClassifierFromDensity.h>
#include <plearn_learners/classifiers/KNNClassifier.h>
#include <plearn_learners/classifiers/MultiInstanceNNet.h>
//#include <plearn_learners/classifiers/OverlappingAdaBoost.h> // Does not currently compile.

// Generic
#include <plearn_learners/generic/AddCostToLearner.h>
#include <plearn_learners/generic/AddLayersNNet.h>
#include <plearn_learners/generic/NNet.h>
#include <plearn_learners/generic/SelectInputSubsetLearner.h>
#include <plearn_learners/generic/StackedLearner.h>
#include <plearn_learners/generic/TestingLearner.h>

// Hyper
#include <plearn_learners/hyper/HyperLearner.h>

// Regressors
#include <plearn_learners/regressors/ConstantRegressor.h>
#include <plearn_learners/regressors/KNNRegressor.h>
#include <plearn_learners/regressors/LinearRegressor.h>
#include <plearn_learners/regressors/PLS.h>
#include <plearn_learners/regressors/RankLearner.h>

// Distribution (deprecated)
#include <plearn_learners/distributions/LocallyWeightedDistribution.h>

// Unsupervised/KernelProjection
#include <plearn_learners/unsupervised/Isomap.h>
#include <plearn_learners/unsupervised/KernelPCA.h>
#include <plearn_learners/unsupervised/LLE.h>
#include <plearn_learners/unsupervised/PCA.h>
#include <plearn_learners/unsupervised/SpectralClustering.h>
#include <plearn_learners/unsupervised/UniformizeLearner.h>

// PDistribution
#include <plearn_learners/distributions/ConditionalDensityNet.h>
#include <plearn_learners/distributions/GaussianDistribution.h>
#include <plearn_learners/distributions/GaussianContinuumDistribution.h>
#include <plearn_learners/distributions/ManifoldParzen2.h>
#include <plearn_learners/distributions/ParzenWindow.h>
#include <plearn_learners/distributions/SpiralDistribution.h>
#include <plearn_learners/distributions/UniformDistribution.h>

// Nearest-Neighbors
#include <plearn_learners/nearest_neighbors/ExhaustiveNearestNeighbors.h>
#include <plearn_learners/nearest_neighbors/GenericNearestNeighbors.h>

/************
 * Splitter *
 ************/
#include <plearn/vmat/BinSplitter.h>
#include <plearn/vmat/ConcatSetsSplitter.h>
#include <plearn/vmat/DBSplitter.h>
#include <plearn/vmat/ExplicitSplitter.h>
#include <plearn/vmat/FilterSplitter.h>
#include <plearn/vmat/FractionSplitter.h>
#include <plearn/vmat/KFoldSplitter.h>
#include <plearn/vmat/NoSplitSplitter.h>
#include <plearn/vmat/RepeatSplitter.h>
#include <plearn/vmat/SourceVMatrixSplitter.h>
#include <plearn/vmat/StackedSplitter.h>
#include <plearn/vmat/TestInTrainSplitter.h>
#include <plearn/vmat/ToBagSplitter.h>
#include <plearn/vmat/TrainTestSplitter.h>
#include <plearn/vmat/TrainValidTestSplitter.h>

/************
 * Variable *
 ************/
#include <plearn/var/MatrixElementsVariable.h>

/*********************
 * VecStatsCollector *
 *********************/
#include <plearn/math/LiftStatsCollector.h>

/***********
 * VMatrix *
 ***********/
#include <plearn/vmat/AsciiVMatrix.h>
#include <plearn/vmat/AutoVMatrix.h>
#include <plearn/vmat/BatchVMatrix.h>
#include <plearn/vmat/BootstrapVMatrix.h>
#include <plearn/vmat/CenteredVMatrix.h>
#include <plearn/vmat/CompactVMatrix.h>
#include <plearn/vmat/CompressedVMatrix.h>
#include <plearn/vmat/CumVMatrix.h>
#include <plearn/vmat/DatedJoinVMatrix.h>
#include <plearn/vmat/DictionaryVMatrix.h>
#include <plearn/vmat/ExtractNNetParamsVMatrix.h>
#include <plearn/vmat/FilteredVMatrix.h>
#include <plearn/vmat/FinancePreprocVMatrix.h>
#include <plearn/vmat/GeneralizedOneHotVMatrix.h>
#include <plearn/vmat/GetInputVMatrix.h>
#include <plearn/vmat/GramVMatrix.h>
#include <plearn/vmat/IndexedVMatrix.h>
#include <plearn/vmat/JulianizeVMatrix.h>
#include <plearn/vmat/KNNVMatrix.h>
#include <plearn/vmat/LocalNeighborsDifferencesVMatrix.h>
#include <plearn/vmat/LocallyPrecomputedVMatrix.h>
#include <plearn/vmat/MultiInstanceVMatrix.h>
#include <plearn/vmat/MultiToUniInstanceSelectRandomVMatrix.h>
#include <plearn/vmat/OneHotVMatrix.h>
#include <plearn/vmat/PLearnerOutputVMatrix.h>
#include <plearn/vmat/PairsVMatrix.h>
#include <plearn/vmat/PrecomputedVMatrix.h>
#include <plearn/vmat/ProcessingVMatrix.h>
#include <plearn/vmat/RankedVMatrix.h>
#include <plearn/vmat/RegularGridVMatrix.h>
#include <plearn/vmat/RemoveDuplicateVMatrix.h>
#include <plearn/vmat/SelectAttributsSequenceVMatrix.h>
#include <plearn/vmat/ShuffleColumnsVMatrix.h>
#include <plearn/vmat/SortRowsVMatrix.h>
#include <plearn/vmat/SparseVMatrix.h>
#include <plearn/vmat/SubInputVMatrix.h>
#include <plearn/vmat/TextFilesVMatrix.h>
#include <plearn/vmat/ThresholdVMatrix.h>
#include <plearn/vmat/TransposeVMatrix.h>
#include <plearn/vmat/VMatrixFromDistribution.h>
#include <plearn/vmat/ViewSplitterVMatrix.h>
#endif

