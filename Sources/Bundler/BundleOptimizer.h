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
		Scalar iterationCount;
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
			Scalar finalIteration = 0;
			OptimizeLoop( &jacobian, pBundle, &geometricError, &finalIteration ); // TODO: parameter vector

			if ( pStats )
			{
				pStats->finalGeometricError = geometricError;
				pStats->iterationCount = finalIteration;
			}
		}

	protected:

		void InitializeJacobian( 
			__in const Bundle* pBundle, 
			__out ProjectionProvider< CameraModel >* pJacobian )
		{
			const size_t cameraCount = pBundle->cameras.Length( );
			m_cameraModels.Allocate( cameraCount );
			for ( size_t i = 0; i < cameraCount; i++ )
			{
				m_cameraModels[i].Initialize( pBundle->cameras[i] );
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
			// 0. fill paramVector from bundle
			// 1. solve for new params
			// 2. replace params in bundle
			// 3. get new error
			// if accept step
			//	---
			// else
			//  replace params back

			size_t parameterCount = GetTotalPrameterCount( pJacobian );
			Vector< Scalar > parameterVector( parameterCount );

			Scalar* pParameterVector = parameterVector.Elements( );
			GetParameterVector( pBundle, parameterCount, pParameterVector );

			Scalar geometricError = GetGeometricError( pJacobian );
			uint iteration = 0;

			while ( ( iteration < m_maxIterations ) && ( geometricError > m_errorTolerance ) )
			{
				m_linearSolver.SolveSystem( m_dampeningFactor, pJacobian, parameterCount, pTempParams, NULL );
				
				// replace bundle params

				Scalar newGeometricError = GetGeometricError( pJacobian );
				if ( newGeometricError < geometricError )
				{
					geometricError = newGeometricError;
					ShallowCopy< Scalar >( pTempParams, parameterCount, pParameterVector );

					m_dampeningFactor *= m_dampeningDown;
				}
				else
				{
					ShallowCopy< Scalar >( pParameterVector, parameterCount, pTempParams );
					// replace bundle params back

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

		void GetParameterVector(
			__in const Bundle* pBundle,
			__in const size_t parameterCount,
			__out_ecount( parameterCount ) Scalar* pParameterVector )
		{
			// TODO: exctract parameters from bundle
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