// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <Windows.h>
#include <ctime>

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#define WIN32_LEAN_AND_MEAN 

#include "..\Utils\UtilsLib.h"
using namespace Utils;

#include "..\LinearAlgebra\LinearAlgebraLib.h"
using namespace LinearAlgebra;

#include "..\TestingUtils\TestingUtilsLib.h"
using namespace TestingUtils;