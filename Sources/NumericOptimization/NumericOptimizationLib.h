#pragma once

// Automatic differentiation
#include "DiffNum.h"

// Linear solvers
#include "ILinearSolver.h"
#include "ConjugateGradientsSolver.h"
#include "GaussJordanElimination.h"

#include "IPreconditioner.h"
#include "JacobiPreconditioner.h"

// Residuals, Cost functions
#include "IResidual.h"
#include "ILeastSquaresCostFunction.h"
#include "LeastSquaresCostFunctionABC.h"
#include "LeastSquaresCostFunctionSimple.h"

// Optimization methods
#include "IOptimizationMethod.h"
#include "GaussNewtonMethod.h"
#include "LevenbergMarquardtMethod.h"


namespace NumericOptimization {

}