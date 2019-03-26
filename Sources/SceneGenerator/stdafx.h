#pragma once

#include <Windows.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <map>
#include <thread>
#include <mutex>

#include <glew.h>
#include <freeglut.h>

#include <stdint.h>

#include "../Utils/UtilsLib.h"
using namespace Utils;

#include "../LinearAlgebra/LinearAlgebraLib.h"
using namespace LinearAlgebra;

#include "../NumericOptimization/DiffNum.h"
#include "../Bundler/BundlerLib.h"

#include "Macros.h"
#include "ViewerMath.h"
