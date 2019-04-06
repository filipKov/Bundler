#pragma once

namespace Bundler { namespace Async { namespace LinearSolver { namespace Task {

using CameraModel = CameraModels::CameraModel6DoF< 10 >;

	class ParallelPcgTaskFactory
	{
	public:

		static uint GetHessianMulCamRowTask( __in WorkerThread* pWorker, __out ITask* pTask )
		{
			uint taskSize = 0;
			const WorkerThreadInfo* pWorkerInfo = pWorker->GetInfo( );

			switch ( pWorkerInfo->type )
			{
			case WorkerThreadType::CPU:

				break;
			case WorkerThreadType::GPU:

				break;
			default:
				break;
			}

			return taskSize;
		}

	protected:

		static uint GetHessianMulCamRowTaskCPU( __out ITask* pTask )
		{

		}

	};

} } } }
