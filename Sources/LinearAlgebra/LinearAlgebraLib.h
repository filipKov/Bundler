#pragma once

#include "HelperMacros.h"
#include "Exceptions.h"

#include "DynamicMatrix.h"
#include "DynamicMatrixOperators.h"

#include "StaticMatrix.h"
#include "StaticMatrixOperators.h"

#include "DynamicVector.h"
#include "DynamicVectorOperators.h"

#include "StaticVector.h"
#include "StaticVectorOperators.h"

#include "FastVector3.h"
#include "FastVector4.h"
#include "FastMatrix33.h"
#include "FastMatrix44.h"
#include "FastLinAlgFunctions.h"

#include "MatrixInvertGJUnrolled.h"
#include "MatrixInvertGJ.h"

#ifdef IMPORT_AMP_LINEAR_ALGEBRA
	
	#include "FastVector3AMP.h"
	#include "FastVector4AMP.h"
	#include "FastMatrix33AMP.h"
	#include "FastMatrix44AMP.h"
	#include "FastLinAlgFunctionsAMP.h"
	#include "MatrixInvertGaussJordanAMP.h"

#endif