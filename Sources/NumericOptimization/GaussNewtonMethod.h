#pragma once

#include "stdafx.h"
#include "ILinearSolver.h"
#include "IOptimizationMethod.h"

namespace NumericOptimization {

	using namespace LinearSolver;

	template < typename Scalar, class Residual >
	class GaussNewtonMethod : public IOptimizationMethod< Scalar, Residual > {

	public:

		// default settings are: maxIterations = 100; tolerance = 1e-6
		GaussNewtonMethod( __in std::shared_ptr< ILinearSolver<Scalar> > spSolver ) {
			m_spLinearSolver = spSolver;
			m_settings.maxIterations = 100;
			m_settings.tolerance = Scalar( 1e-6 );
		};

		GaussNewtonMethod( __in std::shared_ptr< ILinearSolver<Scalar> > spSolver, __in const OptimizationMethodSettings< Scalar >& settings ) {
			m_spLinearSolver = spSolver;
			m_settings = settings;
		};

		virtual void Solve(
			__in std::shared_ptr< ILeastSquaresCostFunction< Scalar, Residual > > spCostFunction,
			__inout LinearAlgebra::Vector<Scalar>& parameterVector,
			__out_opt OptimizationStatistics<Scalar>* pResult ) const override
		{
			uint residualCnt = spCostFunction->GetResidualCount();
			uint parameterCnt = parameterVector.GetDimension();
			
			Matrix< Scalar > jacobian( residualCnt, parameterCnt);
			LinearAlgebra::Vector< Scalar > f( residualCnt );
			Matrix< Scalar > JTJ( parameterCnt, parameterCnt );
			LinearAlgebra::Vector< Scalar > JTfNeg( parameterCnt );

			Scalar cost0 = Scalar( 0 );
			Scalar delta = Scalar( 0 );

			for ( uint i = 0; i < m_settings.maxIterations; i++ ) {
				(*spCostFunction)( parameterVector, f, jacobian );
				
				Scalar cost = f.Dot( f );
				delta = abs( cost0 - cost );
				if ( delta < m_settings.tolerance ) {
					WriteResult( i, cost, delta, pResult );
					return;
				}
				cost0 = cost;

				Matrix< Scalar > jacobianT = jacobian.Transpose();
				JTJ = jacobianT * jacobian;
				JTfNeg = -(jacobianT * f);

				LinearAlgebra::Vector< Scalar > deltaX( parameterCnt );
 				m_spLinearSolver->Solve( JTJ, JTfNeg, deltaX );
				parameterVector += deltaX;
			}

			WriteResult( m_settings.maxIterations, cost0, delta, pResult );
		};

	protected:

		std::shared_ptr< ILinearSolver<Scalar> > m_spLinearSolver;
		
		OptimizationMethodSettings< Scalar > m_settings;

	};

}