#pragma once

#include <Windows.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <map>
#include <thread>
#include <mutex>
#include <atomic>

#include <glew.h>
#include <freeglut.h>

#include <stdint.h>

#include <amp.h>
#include <amp_math.h>

#include "../Utils/UtilsLib.h"
using namespace Utils;

#define IMPORT_AMP_LINEAR_ALGEBRA
#include "..\LinearAlgebra\LinearAlgebraLib.h"
using namespace LinearAlgebra;

#define IMPORT_AMP_DIFFNUM
#include "..\NumericOptimization\DiffNum.h"
using namespace NumericOptimization::AutomaticDifferentiation;


#include "../NumericOptimization/DiffNum.h"
#include "../Bundler/BundlerLib.h"

#include "Macros.h"
#include "ViewerMath.h"
