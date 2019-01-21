#pragma once

#include "stdafx.h"
#include "LeastSquaresCostFunctionABC.h"

namespace NumericOptimization {

	template < typename Scalar, class Residual >
	class LeastSquaresCostFunction : public LeastSquaresCostFunctionABC < Scalar, Residual > {

	public:

		void operator() ( 
			__in const LinearAlgebra::Vector<Scalar>& parameters, 
			__inout LinearAlgebra::Vector<Scalar>& residualVector,
			__inout Matrix<Scalar>& jacobian ) const override final 
		{
			CheckInput( parameters, residualVector, jacobian );

			//#pragma omp parallel for schedule(static, 1024)
			//for ( int i = 0; i < static_cast<int>( m_residuals.size() ); i++ ) {
			for ( uint i = 0; i < m_residuals.size(); i++ ) {
				auto result = m_residuals[i].Evaluate( parameters );
				
				residualVector[i] = result.GetFx();
				ShallowCopy<Scalar>( result.GetDiff().Elements(), result.GetDiffCount(), jacobian[i] );
			}
		};

		Scalar operator() ( __in const LinearAlgebra::Vector<Scalar>& parameters ) const override final {
			Scalar cost = Scalar( 0 );

			for ( auto& residual : m_residuals ) {
				auto result = residual.Evaluate( parameters );
				cost += result.GetFx() * result.GetFx();
			}

			return cost;
		}

	protected:

		void CheckInput( 
			__in const LinearAlgebra::Vector<Scalar>& parameters,
			__in const LinearAlgebra::Vector<Scalar>& residualVector,
			__in const Matrix<Scalar>& jacobian ) const
		{
			if ( residualVector.GetDimension() != m_residuals.size() ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( residualVector ), ExceptionReasons::DIMENSIONS_MISMATCH );
			}

			uint JRows = 0, JCols = 0;
			jacobian.GetDimensions( JRows, JCols );

			if ( ( JRows != residualVector.GetDimension() ) || ( JCols != parameters.GetDimension() ) ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( jacobian ), ExceptionReasons::DIMENSIONS_MISMATCH );
			}
		};

	};


}