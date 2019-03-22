#pragma once

namespace Bundler {

	struct OptimizerSettings
	{
		uint maxIterations;
		Scalar errorTolerance;

		Scalar initialDampeningFactor;
		Scalar dampeningUp;
		Scalar dampeningDown;

		LinearSolver::PCGSolverSettings linearSolverSettings;
	};

	struct OptimizerStatistics
	{
		Scalar finalGeometricError;
		uint iterationCount;
	};

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

			Scalar geometricError = 0;
			uint finalIteration = 0;
			OptimizeLoop( &jacobian, pBundle, &geometricError, &finalIteration ); // TODO: parameter vector

			if ( pStats )
			{
				pStats->finalGeometricError = geometricError;
				pStats->iterationCount = finalIteration;
			}
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
			__out Scalar* pGeometricError,
			__out uint* pFinalIteration )
		{
			const size_t parameterCount = GetTotalPrameterCount( pJacobian );
			Vector< Scalar > updateVector( (uint)parameterCount );
			Scalar* pUpdateVector = updateVector.Elements( );

			Scalar geometricError = GetGeometricError( pJacobian );

			uint iteration = 0;
			while ( ( iteration < m_maxIterations ) && ( geometricError > m_errorTolerance ) )
			{
				InitializeUpdateVector( parameterCount, pUpdateVector );

				m_linearSolver.SolveSystem( m_dampeningFactor, pJacobian, (uint)parameterCount, pUpdateVector, NULL );
				
				UpdateBundleParams( parameterCount, pUpdateVector, pBundle );

				Scalar newGeometricError = GetGeometricError( pJacobian );
				if ( newGeometricError < geometricError )
				{
					geometricError = newGeometricError;
					
					m_dampeningFactor *= m_dampeningDown;
				}
				else
				{
					
					ResetBundleParams( parameterCount, pUpdateVector, pBundle );

					m_dampeningFactor *= m_dampeningUp;
				}
			}

			*pFinalIteration = iteration;
			*pGeometricError = geometricError;
		}

		size_t GetTotalPrameterCount( __in const ProjectionProvider< CameraModel >* pJacobian ) const
		{
			return pJacobian->GetCameraCount( ) * CameraModel::cameraParameterCount + pJacobian->GetPointCount( ) * POINT_PARAM_COUNT;
		}

		Scalar GetGeometricError( __in const ProjectionProvider< CameraModel >* pJacobian ) const
		{
			Scalar error = 0;

			Scalar residuals[2];
			const size_t projectionCount = pJacobian->GetProjectionCount();
			for ( size_t projIx = 0; projIx < projectionCount; projIx++ )
			{
				pJacobian->GetProjectionBlock< false, false, false >( projIx, NULL, NULL, residuals );

				error += residuals[0] * residuals[0];
				error += residuals[1] * residuals[1];
			}
			
			return error;
		}

		void InitializeUpdateVector(
			__in const size_t paramCount,
			__out_ecount( paramCount ) Scalar* pVector )
		{
			for ( size_t i = 0; i < paramCount; i++ )
			{
				ELEMENT( pVector, i ) = Scalar( rand( ) ) / RAND_MAX;
			}
		}

		void UpdateBundleParams(
			__in const size_t paramCount,
			__in_ecount( paramCount ) const Scalar* pUpdateVector,
			__out Bundle* pBundle )
		{
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

	protected:

		uint m_maxIterations;
		Scalar m_errorTolerance;

		Scalar m_dampeningFactor;
		Scalar m_dampeningUp;
		Scalar m_dampeningDown;

		Containers::Buffer< CameraModel > m_cameraModels;

		Preconditioner< CameraModel > m_preconditioner;
		LinearSolver::PCGSolver< CameraModel > m_linearSolver;

	};

}