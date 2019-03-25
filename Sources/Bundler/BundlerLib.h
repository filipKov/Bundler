#pragma once

#include "NumericTypes.h"

#include "BundlerDefs.h"

#include "IBundleImportWorker.h"
#include "SnavelyBundleImportWorker.h"
#include "BundleImporter.h"

#include "RodriguesRotation.h"

#include "ICameraModel.h"
#include "BasicCameraModel.h"
#include "CameraModel3DoF.h"
#include "CameraModel6DoF.h"

#include "BundlerUtils.h" // TODO: move up when done( it's here because intellisense has a mental breakdown otherwise )

#include "BundleStructureMapping.h"

#include "ProjectionProvider.h"
#include "HessianBlockProvider.h"
#include "HessianMultiplicationEngine.h"

#include "Preconditioner.h"
#include "JacobiPreconditioner.h"
#include "CachedJacobiPreconditioner.h"
#include "BlockJacobiPreconditioner.h"

#include "PCGStructs.h"
#include "PCGSolver.h"

#include "BundleOptimizer.h"

#include "SceneGenerator.h"

