// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


// Headers for CppUnitTest
#include "CppUnitTest.h"

#include "..\Utils\UtilsLib.h"
using namespace Utils;

#include "..\LinearAlgebra\LinearAlgebraLib.h"

#include "..\NumericOptimization\NumericOptimizationLib.h"
using namespace NumericOptimization::AutomaticDifferentiation;
using namespace NumericOptimization::LinearSolver;

#include "TestingUtils.h"
#include "MatrixUtils.h"

// TODO: reference additional headers your program requires here
