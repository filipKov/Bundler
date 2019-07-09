// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>
#include <ctime>
#include <thread>
#include <atomic>
#include <amp.h>
#include <amp_math.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


// Headers for CppUnitTest
#include "CppUnitTest.h"

#include "..\Utils\UtilsLib.h"
using namespace Utils;

#define IMPORT_AMP_LINEAR_ALGEBRA
#define IMPORT_AMP_DIFFNUM
#include "..\LinearAlgebra\LinearAlgebraLib.h"
using namespace LinearAlgebra;
using namespace AutomaticDifferentiation;

#include "..\TestingUtils\TestingUtilsLib.h"
using namespace TestingUtils;
// TODO: reference additional headers your program requires here
