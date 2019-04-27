#pragma once

#include "NumericTypes.h"

#include "BundlerDefs.h"


// Bundle import
#include "IBundleImportWorker.h"
#include "SnavelyBundleImportWorker.h"
#include "BundleImporter.h"


// Pre-process
#include "BundleNormalization.h"


// Rotation models
#include "RodriguesRotation.h"


// Camera models
#include "ICameraModel.h"
#include "BasicCameraModel.h"
#include "CameraModel3DoF.h"
#include "CameraModel6DoF.h"
#include "CameraModelAMP6DoF.h"

#include "BundlerUtils.h" // TODO: move up when done( it's here because intellisense has a mental breakdown otherwise )


// Hessian 
#include "BundleStructureMapping.h"

#include "IProjectionProviderData.h"
#include "ProjectionProvider.h"
#include "HessianBlockProvider.h"
#include "HessianMultiplicationEngine.h"


// Local Hessian - CPU
#include "LocalProjectionProviderCPU.h"
#include "LocalHessianBlockProviderCPU.h"
#include "LocalHessianMultiplicationEngineCPU.h"


// Local Hessian - GPU
#include "AMPUtils.h"

#include "LocalProjectionProviderDataAMP.h"
#include "LocalProjectionProviderAMP.h"
#include "LocalHessianBlockProviderAMP.h"
#include "LocalHessianMultiplicationEngineAMP.h"


// Async lib
#include "InterlockedVariable.h"

#include "ITask.h"

#include "IWorkerThreadCallback.h"
#include "WorkerThreadDefs.h"
#include "WorkerThread.h"
#include "WorkerThreadGPU.h"
#include "WorkerPool.h"

#include "ITaskSizeEstimator.h"


// PCG
#include "Preconditioner.h"
#include "JacobiPreconditioner.h"
#include "CachedJacobiPreconditioner.h"
#include "BlockJacobiPreconditioner.h"

#include "PCGStructs.h"
#include "PCGSolver.h"


// Parallel PCG
#include "LocalBlockJacobiPreconditioner.h"

#include "ParallelPCGDefs.h" 

#include "ParallelPCGInitTasksCPU.h"
#include "ParallelPCGLoopTasksCPU.h"
// #include "ParallelPCGInitTasksAMP.h" // cannot get it to compile...
#include "ParallelPCGLoopTasksAMP.h"

#include "ParallelPCGTaskFactory.h"

#include "ParallelPCGSolver.h"


// Optimizer 
#include "BundleOptimizerDefs.h"
#include "BundleOptimizer.h"
#include "ParallelBundleOptimizer.h"


// Scene generator
#include "SceneGenerator.h"

