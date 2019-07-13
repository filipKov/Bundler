// BundlePerformanceEvaluator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "EvaluationTasks.h"
#include "OpenMpTasks.h"
#include "CpuTasks.h"
#include "GpuTasks.h"

#define OMP_THREADS 8
#define STRINGIFY2( x ) #x
#define STRINGIFY( x ) STRINGIFY2( x )

#define ADD_OMP_THREAD_COUNT( strBefore, strAfter ) strBefore STRINGIFY( OMP_THREADS ) strAfter

void PrintResults( __in const char* pHeader, __in const int64 batchSize, __in const TaskResult* pResult )
{
	printf_s( "\t-- %s --\n", pHeader );
	printf_s( "\t\tBatch Size: %lld\n", batchSize );
	printf_s( "\t\tTotal: %fms\n", pResult->totalTime );
	printf_s( "\t\tInitialization: %fms\n", pResult->initTime );
	printf_s( "\t\tEvaluation: %fms\n", pResult->evalTime );
	printf_s( "\t\tEvaluation of single block: %fms\n", pResult->singleBlockAvgTime );
}

int main( int argc, char **argv )
{
	omp_set_num_threads( OMP_THREADS );

	accelerator gpu;
	uint accelCount = Bundler::Async::GetAcceleratorCount( Bundler::Async::IsGPU );
	
	if ( accelCount > 0 )
	{
		Containers::Buffer< accelerator > accels;
		accels.Allocate( accelCount );
		Bundler::Async::GetAccelerators( accelCount, accels.Data(), Bundler::Async::IsGPU );
		
		gpu = accels[0];
		for ( uint i = 1; i < accelCount; i++ )
		{
			if ( accels[i].get_dedicated_memory( ) > gpu.get_dedicated_memory( ) )
			{
				gpu = accels[i];
			}
		}
	}
	else
	{
		printf_s( "No suitable accelerator found, cannot proceed with evaluation\n" );
		return 1;
	}

	if ( argc < 3 )
	{
		printf_s( "Not enough parameters provided\n" );
		printf_s( "Usage: > BundlerPerformanceEvaluator.exe bundleFile maxBatchSize" );
		return 1;
	}
	
	accelerator_view gpuView = gpu.get_default_view( );

	
	
	Bundle bundle;
	BundleAdditionalPayload metadata;

	HRESULT hr = Bundler::Import::BundleImporter::Import( argv[1], &bundle, &metadata );
	if ( SUCCEEDED( hr ) )
	{
		int64 batchSize = atoi( argv[2] );
		batchSize = batchSize > 0 ? batchSize : INT64_MAX;

		const uint cameraCount = ( uint )bundle.cameras.Length( );
		const uint pointCount = ( uint )bundle.points.Length( );

		Containers::Buffer< CameraModel > camModels;
		camModels.Allocate( cameraCount );

		for ( uint i = 0; i < cameraCount; i++ )
		{
			camModels[i].Initialize( &bundle.cameras[i] );
		}

		ProjectionProvider< CameraModel > jacobian;
		jacobian.Initialize(
			cameraCount,
			camModels.Data(),
			bundle.points.Length( ),
			bundle.points.Data( ),
			bundle.projections.Length( ),
			bundle.projections.Data( )
		);

		uint xSize = ( uint )( jacobian.GetCameraCount( ) * CameraModel::cameraParameterCount + jacobian.GetPointCount( ) * POINT_PARAM_COUNT );
		Vector< Scalar > x( xSize );

		TaskInput input;
		input.pJacobian = &jacobian;
		input.maxTaskSize = batchSize;
		input.dampening = Scalar( 1.0 );
		input.vectorSize = xSize;
		input.pVectorX = x.Elements( );

		Random< Scalar >::Fill( -1, 1, xSize, x.Elements( ) );
		
		IEvaluatorTask* pTaskCamCPU = new HxCamerasCPUTask( );
		TaskResult resultCamCPU;

		IEvaluatorTask* pTaskPtCPU = new HxPointsCPUTask( );
		TaskResult resultPtCPU;

		IEvaluatorTask* pTaskCamGPU = new HxCamerasGPUTask( gpuView );
		TaskResult resultCamGPU;

		IEvaluatorTask* pTaskPtGPU = new HxPointsGPUTask( gpuView );
		TaskResult resultPtGPU;

		IEvaluatorTask* pTaskCamOMP = new HxCamerasOMPTask( );
		TaskResult resultCamOMP;

		IEvaluatorTask* pTaskPtOMP = new HxPointsOMPTask( );
		TaskResult resultPtOMP;

		TaskEvaluator::Evaluate( &input, pTaskCamCPU, &resultCamCPU );
		TaskEvaluator::Evaluate( &input, pTaskPtCPU, &resultPtCPU );
		TaskEvaluator::Evaluate( &input, pTaskCamGPU, &resultCamGPU );
		TaskEvaluator::Evaluate( &input, pTaskPtGPU, &resultPtGPU );
		TaskEvaluator::Evaluate( &input, pTaskCamOMP, &resultCamOMP );
		TaskEvaluator::Evaluate( &input, pTaskPtOMP, &resultPtOMP );

		delete pTaskCamCPU;
		delete pTaskCamGPU;
		delete pTaskCamOMP;
		delete pTaskPtCPU;
		delete pTaskPtGPU;
		delete pTaskPtOMP;

		printf_s( "---- Evaluation statistics ----\n" );
		PrintResults( "Hessian Multiplication, Upper(Camera) part, CPU", min( batchSize, cameraCount ), &resultCamCPU );
		PrintResults( "Hessian Multiplication, Upper(Camera) part, GPU", min( batchSize, cameraCount ), &resultCamGPU );
		PrintResults( ADD_OMP_THREAD_COUNT( "Hessian Multiplication, Upper(Camera) part, OMP (", " threads)" ), min( batchSize, cameraCount ), &resultCamOMP );
		PrintResults( "Hessian Multiplication, Lower(Point) part, CPU", min( batchSize, pointCount ), &resultPtCPU );
		PrintResults( "Hessian Multiplication, Lower(Point) part, GPU", min( batchSize, pointCount ), &resultPtGPU );
		PrintResults( ADD_OMP_THREAD_COUNT( "Hessian Multiplication, Lower(Point) part, OMP (", " threads)" ), min( batchSize, pointCount ), &resultPtOMP );
	}

    return 0;
}

