
#pragma once

#include <Windows.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <map>
#include <thread>
#include <mutex>
#include <atomic>

#include <stdint.h>

#include <amp.h>
#include <amp_math.h>
#include <omp.h>

#include "../Utils/UtilsLib.h"
using namespace Utils;

#define IMPORT_AMP_LINEAR_ALGEBRA
#define IMPORT_AMP_DIFFNUM
#include "..\LinearAlgebra\LinearAlgebraLib.h"
using namespace LinearAlgebra;
using namespace AutomaticDifferentiation;

#include "../Bundler/BundlerLib.h"
using namespace Bundler;
