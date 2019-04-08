#pragma once

#include "NumericTypes.h"

#include "BundlerDefs.h"


// Bundle import
#include "IBundleImportWorker.h"
#include "SnavelyBundleImportWorker.h"
#include "BundleImporter.h"

// Rotation models
#include "RodriguesRotation.h"


// Camera models
#include "ICameraModel.h"
#include "BasicCameraModel.h"
#include "CameraModel3DoF.h"
#include "CameraModel6DoF.h"

#include "BundlerUtils.h" // TODO: move up when done( it's here because intellisense has a mental breakdown otherwise )


// Hessian 
#include "BundleStructureMapping.h"

#include "ProjectionProvider.h"
#include "HessianBlockProvider.h"
#include "HessianMultiplicationEngine.h"


// Local Hessian - CPU
#include "LocalProjectionProviderCPU.h"
#include "LocalHessianBlockProviderCPU.h"
#include "LocalHessianMultiplicationEngineCPU.h"


// Local Hessian - GPU
#include "AMPUtils.h"

#include "LocalProjectionProviderAMP.h"
#include "LocalHessianBlockProviderAMP.h"
#include "LocalHessianMultiplicationEngineAMP.h"

#include "LocalProjectionProvider.h"
#include "MemoryLimitPreciseStrategy.h"
#include "MemoryLimitPreciseFastStrategy.h"
#include "CountLimitStrategy.h"


// Async lib
#include "InterlockedVariable.h"

#include "ITask.h"
#include "HessianCameraRowMultiplicationTask.h"
#include "HessianPointRowMultiplicationTask.h"

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

#include "ParallelPCGLoopTasksCPU.h"

#include "HessianMulCamRowCPU.h"
#include "HessianMulPointRowCPU.h"

#include "ParallelPcgTaskFactory.h"

#include "ParallelPcgFunctions.h"
#include "ParallelPCGSolver.h"


// Optimizer 
#include "BundleOptimizer.h"


// Scene generator
#include "SceneGenerator.h"

