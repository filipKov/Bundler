#pragma once

namespace Bundler {

	template < class CameraModel, template < class > class Preconditioner >
	class BundleOptimizerOpenMP
	{
	public:
	
		void Initialize( __in const OptimizerSettings& settings )
		{
			m_maxIterations = settings.maxIterations;
			m_errorTolerance = settings.errorTolerance;
	
			m_dampeningFactor = settings.initialDampeningFactor;
			m_dampeningUp = settings.dampeningUp;
			m_dampeningDown = settings.dampeningDown;
	
			m_linearSolver.Initialize( settings.linearSolverSettings );
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
		}
	
	protected:
	
		void InitializeJacobian(
			__in Bundle* pBundle,
			__out ProjectionProvider< CameraModel >* pJacobian )
		{
			const int cameraCount = ( int )pBundle->cameras.Length( );
			m_cameraModels.Allocate( cameraCount );

			#pragma omp parallel for
			for ( int i = 0; i < cameraCount; i++ )
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
			Vector< Scalar > updateVector( ( uint )parameterCount );
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
				m_linearSolver.SolveSystem( m_dampeningFactor, pJacobian, ( uint )parameterCount, pUpdateVector, NULL );
	
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
			Async::InterlockedVariable< double > error = 0;

			const int64 projectionCount = ( int64 )pJacobian->GetProjectionCount( );

			#pragma omp parallel
			{
				Scalar residuals[2];
				double localError = 0;

				#pragma omp for
				for ( int64 projIx = 0; projIx < projectionCount; projIx++ )
				{
					pJacobian->GetProjectionBlock< false, false, true >( projIx, NULL, NULL, residuals );

					localError += residuals[0] * residuals[0];
					localError += residuals[1] * residuals[1];
				}

				error += localError;
			}

			return error.GetValue( );
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

			const int64 cameraCount = ( int64 )m_cameraModels.Length( );
			const int64 pointCount = ( int64 )pBundle->points.Length( );

			#pragma omp parallel
			{
				#pragma omp for nowait
				for ( int64 cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
				{
					const Scalar* pParamSrc = Utils::GetCameraParamPtr< CameraModel >( ( uint )cameraIx, pUpdateVector );

					m_cameraModels[cameraIx].UpdateCamera( pParamSrc );
				}

				#pragma omp for
				for ( int64 pointIx = 0; pointIx < pointCount; pointIx++ )
				{
					const Scalar* pParamSrc = Utils::GetPointParamPtr< CameraModel >( ( size_t )pointIx, ( size_t )cameraCount, pUpdateVector );
					Scalar* pPoint = pBundle->points[pointIx].Elements( );

					MatrixAdd< Scalar, 1, POINT_PARAM_COUNT >( pPoint, pParamSrc, pPoint );
				}
			}
		}
	
		void ResetBundleParams(
			__in const size_t paramCount,
			__in_ecount( paramCount ) const Scalar* pUpdateVector,
			__out Bundle* pBundle )
		{
			UNREFERENCED_PARAMETER( paramCount );

			const int64 cameraCount = ( int64 )m_cameraModels.Length( );
			const int64 pointCount = ( int64 )pBundle->points.Length( );

			#pragma omp parallel
			{
				#pragma omp for nowait
				for ( int64 cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
				{
					const Scalar* pParamSrc = Utils::GetCameraParamPtr< CameraModel >( ( uint )cameraIx, pUpdateVector );

					m_cameraModels[cameraIx].ResetLastUpdate( pParamSrc );
				}

				#pragma omp for
				for ( int64 pointIx = 0; pointIx < pointCount; pointIx++ )
				{
					const Scalar* pParamSrc = Utils::GetPointParamPtr< CameraModel >( ( size_t )pointIx, ( size_t )cameraCount, pUpdateVector );
					Scalar* pPoint = pBundle->points[pointIx].Elements( );

					MatrixSub< Scalar, 1, POINT_PARAM_COUNT >( pPoint, pParamSrc, pPoint );
				}
			}
		}
	
		Scalar EstimateDampeningFactor( __in const ProjectionProvider< CameraModel >* pProjectionProvider )
		{
			HessianBlockProvider< CameraModel > hessian;
			hessian.Initialize( pProjectionProvider );

			Async::InterlockedVariable< Scalar > totalNorm = FLT_MAX;

			const int64 pointCount = ( int64 )hessian.GetPointCount( );

			#pragma omp parallel
			{
				Scalar tempNorm = FLT_MAX;
				Scalar pointBlock[POINT_PARAM_COUNT * POINT_PARAM_COUNT];

				#pragma omp for nowait
				for ( int64 pointIx = 0; pointIx < pointCount; pointIx++ )
				{
					hessian.GetPointBlock( pointIx, pointBlock );
					Scalar pointNorm = MatrixFrobeniusNorm< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT >( pointBlock );
					tempNorm = min( tempNorm, pointNorm );
				}

				totalNorm.SetMin( tempNorm );
			}

			return totalNorm.GetValue( ) * Scalar( 0.0000001 );
		}
	
	protected:
	
		uint m_maxIterations;
		double m_errorTolerance;
	
		Scalar m_dampeningFactor;
		Scalar m_dampeningUp;
		Scalar m_dampeningDown;
	
		Containers::Buffer< CameraModel > m_cameraModels;
	
		LinearSolver::PCGSolverOpenMP< CameraModel > m_linearSolver;
	};

}
