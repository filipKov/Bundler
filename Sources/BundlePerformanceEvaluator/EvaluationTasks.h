#pragma once

using CameraModel = CameraModels::CameraModel6DoF< 10 >;

// template < class CameraModel >
struct TaskInput
{
	ProjectionProvider< CameraModel >* pJacobian;
	Scalar dampening;

	uint vectorSize;
	Scalar* pVectorX;

	int64 maxTaskSize;
};

struct TaskResult
{
	double totalTime;
	double initTime;
	double evalTime;
	
	double singleBlockAvgTime;
};

class IEvaluatorTask
{
public:

	virtual ~IEvaluatorTask( )
	{

	}

	virtual void PreInit( __in const TaskInput* pIn ) = 0;

	virtual int64 Initialize( __in const TaskInput* pIn ) = 0;

	virtual void Execute( __in const TaskInput* pIn ) = 0;

};

class TaskEvaluator
{
public:

	static void Evaluate( __in const TaskInput* pIn, __inout IEvaluatorTask* pTask, __out TaskResult* pResult )
	{
		HighResolutionClock stopwatch;
		stopwatch.Clear( );

		pTask->PreInit( pIn );

		stopwatch.Start( );
		int64 actualTaskSize = pTask->Initialize( pIn );
		stopwatch.Lap( );
		pTask->Execute( pIn );
		stopwatch.Lap( );

		stopwatch.Stop( );


		pResult->initTime = stopwatch.GetLap( 0 );
		pResult->evalTime = stopwatch.GetLap( 1 );
		pResult->totalTime = stopwatch.GetTotalTime( );

		pResult->singleBlockAvgTime = pResult->evalTime / actualTaskSize;
	}

};

