// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>
#include <thread>
#include <atomic>
#include <cmath>
#include <amp.h>
using namespace Concurrency;

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include "..\Utils\UtilsLib.h"
using namespace Utils;

// #define IMPORT_AMP_LINEAR_ALGEBRA
#include "..\LinearAlgebra\LinearAlgebraLib.h"
using namespace LinearAlgebra;

// #define IMPORT_AMP_DIFFNUM
#include "..\NumericOptimization\DiffNum.h"
using namespace NumericOptimization::AutomaticDifferentiation;

#include "NumericTypes.h"

#include "BundlerDefs.h"

// TODO: reference additional headers your program requires here
