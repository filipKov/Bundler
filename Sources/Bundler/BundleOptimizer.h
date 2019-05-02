#pragma once

namespace Bundler {

	template < class CameraModel, template < class > class Preconditioner >
	class BundleOptimizer
	{
	public:

		void Initialize( __in const OptimizerSettings& settings )
		{
			m_maxIterations = settings.maxIterations;
			m_errorTolerance = settings.errorTolerance;
			
			m_dampeningFactor = settings.initialDampeningFactor;
			m_dampeningUp = settings.dampeningUp;
			m_dampeningDown = settings.dampeningDown;

			m_linearSolver.Initialize( settings.linearSolverSettings, &m_preconditioner );
		}

		void Optimize( __inout Bundle* pBundle, __out_opt OptimizerStatistics* pStats )
		{
			ProjectionProvider< CameraModel > jacobian;
			InitializeJacobian( pBundle, &jacobian );

			if ( m_dampeningFactor <= 0 )
			{
				m_dampeningFactor = EstimateDampeningFactor( &jacobian );
			}

			OptimizeLoop( &jacobian, pBundle, pStats );

			SetOptimizedCameras( pBundle );
		}

	protected:

		void InitializeJacobian( 
			__in Bundle* pBundle, 
			__out ProjectionProvider< CameraModel >* pJacobian )
		{
			const size_t cameraCount = pBundle->cameras.Length( );
			m_cameraModels.Allocate( cameraCount );
			for ( size_t i = 0; i < cameraCount; i++ )
			{
				m_cameraModels[i].Initialize( &pBundle->cameras[i] );
			}

			pJacobian->Initialize(
				cameraCount,
				m_cameraModels.Data( ),
				pBundle->points.Length( ),
				pBundle->points.Data( ),
				pBundle->projections.Length( ),
				pBundle->projections.Data( ) 
			);
		}

		void OptimizeLoop(
			__in const ProjectionProvider< CameraModel >* pJacobian,
			__inout Bundle* pBundle,
			__out_opt OptimizerStatistics* pStats )
		{
			const size_t parameterCount = GetTotalPrameterCount( pJacobian );
			Vector< Scalar > updateVector( (uint)parameterCount );
			Scalar* pUpdateVector = updateVector.Elements( );
			
			InitializeUpdateVector( parameterCount, pUpdateVector );

			double geometricError = GetGeometricError( pJacobian );
			if ( pStats )
			{
				pStats->initialGeometricError = geometricError;
			}

			printf( "Pre-optimization: error: %f | dampening: %f\n", geometricError, m_dampeningFactor );

			uint iteration = 0;
			uint acceptedSteps = 0;
			uint rejectecSteps = 0;
			while ( ( iteration < m_maxIterations ) && ( geometricError > m_errorTolerance ) )
			{
				m_linearSolver.SolveSystem( m_dampeningFactor, pJacobian, (uint)parameterCount, pUpdateVector, NULL );
				
				UpdateBundleParams( parameterCount, pUpdateVector, pBundle );

				double newGeometricError = GetGeometricError( pJacobian );
				if ( newGeometricError < geometricError )
				{
					geometricError = newGeometricError;
					acceptedSteps++;
					m_dampeningFactor *= m_dampeningDown;

					printf( "Iteration %u: step ACCEPTED | error: %f | dampening: %f\n", iteration, geometricError, m_dampeningFactor );
				}
				else
				{
					ResetBundleParams( parameterCount, pUpdateVector, pBundle );
					rejectecSteps++;
					m_dampeningFactor *= m_dampeningUp;

					printf( "Iteration %u: step REJECTED | error: %f | dampening: %f\n", iteration, geometricError, m_dampeningFactor );
				}

				iteration++;
			}

			if ( pStats )
			{
				pStats->iterationCount = iteration;
				pStats->finalGeometricError = geometricError;
				pStats->acceptedIterations = acceptedSteps;
				pStats->rejectedIterations = rejectecSteps;
			}
		}

		size_t GetTotalPrameterCount( __in const ProjectionProvider< CameraModel >* pJacobian ) const
		{
			return pJacobian->GetCameraCount( ) * CameraModel::cameraParameterCount + pJacobian->GetPointCount( ) * POINT_PARAM_COUNT;
		}

		double GetGeometricError( __in const ProjectionProvider< CameraModel >* pJacobian ) const
		{
			double error = 0;

			Scalar residuals[2];
			const size_t projectionCount = pJacobian->GetProjectionCount();
			for ( size_t projIx = 0; projIx < projectionCount; projIx++ )
			{
				pJacobian->GetProjectionBlock< false, false, true >( projIx, NULL, NULL, residuals );

				error += residuals[0] * residuals[0];
				error += residuals[1] * residuals[1];
			}
			
			// printf_s( "GeometricError: Total: %f | Average: %f\n", error, error / projectionCount );
			return error;
		}

		void InitializeUpdateVector(
			__in const size_t paramCount,
			__out_ecount( paramCount ) Scalar* pVector )
		{
			Random< Scalar >::Fill( -m_dampeningFactor, m_dampeningFactor, paramCount, pVector );
		}

		void UpdateBundleParams(
			__in const size_t paramCount,
			__in_ecount( paramCount ) const Scalar* pUpdateVector,
			__out Bundle* pBundle )
		{
			UNREFERENCED_PARAMETER( paramCount );

			const size_t cameraCount = m_cameraModels.Length( );
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				m_cameraModels[cameraIx].UpdateCamera( pUpdateVector );
				pUpdateVector += CameraModel::cameraParameterCount;
			}

			const size_t pointCount = pBundle->points.Length( );
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				Scalar* pPoint = pBundle->points[pointIx].Elements( );
				MatrixAdd< Scalar, 1, POINT_PARAM_COUNT >( pPoint, pUpdateVector, pPoint );
				pUpdateVector += POINT_PARAM_COUNT;
			}
		}

		void ResetBundleParams(
			__in const size_t paramCount,
			__in_ecount( paramCount ) const Scalar* pUpdateVector,
			__out Bundle* pBundle )
		{
			UNREFERENCED_PARAMETER( paramCount );

			const size_t cameraCount = m_cameraModels.Length( );
			for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
			{
				m_cameraModels[cameraIx].ResetLastUpdate( pUpdateVector );
				pUpdateVector += CameraModel::cameraParameterCount;
			}

			const size_t pointCount = pBundle->points.Length( );
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				Scalar* pPoint = pBundle->points[pointIx].Elements();
				MatrixSub< Scalar, 1, POINT_PARAM_COUNT >( pPoint, pUpdateVector, pPoint );
				pUpdateVector += POINT_PARAM_COUNT;
			}
		}

		Scalar EstimateDampeningFactor( __in const ProjectionProvider< CameraModel >* pProjectionProvider )
		{
			HessianBlockProvider< CameraModel > hessian;
			hessian.Initialize( pProjectionProvider );

			Scalar totalNorm = FLT_MAX;

			Scalar pointBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];

			const size_t pointCount = hessian.GetPointCount( );
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				hessian.GetPointBlock( pointIx, pointBlock );
				Scalar ptNorm = MatrixFrobeniusNorm< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT >( pointBlock );
				totalNorm = min( totalNorm, ptNorm );
			}

			return totalNorm * Scalar( 0.0000001 );
		}

		void SetOptimizedCameras( __out Bundle* pBundle )
		{
			const size_t cameraCount = pBundle->cameras.Length( );

			CameraModel* pSrcCamera = m_cameraModels.Data( );
			Camera* pDstCamera = pBundle->cameras.Data( );

			for ( size_t i = 0; i < cameraCount; i++ )
			{
				pSrcCamera->CopyToCamera( pDstCamera );
				pSrcCamera++;
				pDstCamera++;
			}
		}

	protected:

		uint m_maxIterations;
		double m_errorTolerance;

		Scalar m_dampeningFactor;
		Scalar m_dampeningUp;
		Scalar m_dampeningDown;

		Containers::Buffer< CameraModel > m_cameraModels;

		Preconditioner< CameraModel > m_preconditioner;
		LinearSolver::PCGSolver< CameraModel > m_linearSolver;

	};

}