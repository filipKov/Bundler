// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "..\Utils\UtilsLib.h"
using namespace Utils;

#include "..\LinearAlgebra\LinearAlgebraLib.h"

#include "..\NumericOptimization\DiffNum.h"
#include "..\NumericOptimization\ILinearSolver.h"
#include "..\NumericOptimization\ConjugateGradientsSolver.h"
using namespace NumericOptimization::AutomaticDifferentiation;
using namespace NumericOptimization::LinearSolver;

#include "BundlerDefs.h"

// TODO: reference additional headers your program requires here
