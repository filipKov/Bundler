#pragma once

#include "stdafx.h"
#include "ILinearSolver.h"
#include "IOptimizationMethod.h"

namespace NumericOptimization {

	using namespace LinearSolver;

	template < typename Scalar >
	struct LevMarSettings : public OptimizationMethodSettings< Scalar > {

		Scalar lambdaInitial;
		Scalar lambdaStepUp;
		Scalar lambdaStepDown;

	};

	template < typename Scalar, class Residual >
	class LevenbergMarquardtMethod : public IOptimizationMethod< Scalar, Residual > {

	protected:

		struct LevMarData {

			std::shared_ptr< Matrix< Scalar > > spJacobian;
			std::shared_ptr< Vector< Scalar > > spResidualVector;

			std::shared_ptr< Matrix< Scalar > > spJacobianTemp;
			std::shared_ptr< Vector< Scalar > > spResidualVectorTemp;
			std::shared_ptr< Vector< Scalar > > spParameterVectorTemp;

			Scalar lambda;
			Scalar cost;
			Scalar costTemp;
			Scalar delta;

			LevMarData( __in const uint parameterCount, __in const uint residualCount ) {
				spJacobian = std::shared_ptr< Matrix< Scalar > >( new Matrix< Scalar >( residualCount, parameterCount ) );
				spResidualVector = std::shared_ptr< Vector< Scalar > >( new Vector< Scalar >( residualCount ) );

				spJacobianTemp = std::shared_ptr< Matrix< Scalar > >( new Matrix< Scalar >( residualCount, parameterCount ) );
				spResidualVectorTemp = std::shared_ptr< Vector< Scalar > >( new Vector< Scalar >( residualCount ) );
				spParameterVectorTemp = std::shared_ptr< Vector< Scalar > >( new Vector< Scalar >( parameterCount ) );

				lambda = Scalar( 0 );
				cost = Scalar( 0 );
				costTemp = Scalar( 0 );
				delta = Scalar( 0 );
			};

		};

	public:

		LevenbergMarquardtMethod( __in std::shared_ptr< ILinearSolver<Scalar> > spSolver ) {	
			m_spLinearSolver = spSolver;
			m_settings.lambdaInitial = Scalar( 0.001 );
			m_settings.lambdaStepUp = Scalar( 10 );
			m_settings.lambdaStepDown = Scalar( 0.01 );
			m_settings.tolerance = Scalar( 1e-6 );
			m_settings.maxIterations = 100;
		};

		LevenbergMarquardtMethod( __in std::shared_ptr< ILinearSolver<Scalar> > spSolver, __in const OptimizationMethodSettings< Scalar >& settings ) {
			m_spLinearSolver = spSolver;
			m_settings.maxIterations = settings.maxIterations;
			m_settings.tolerance = settings.tolerance;
			m_settings.lambdaInitial = Scalar( 0.001 );
			m_settings.lambdaStepUp = Scalar( 10 );
			m_settings.lambdaStepDown = Scalar( 0.01 );
		};

		LevenbergMarquardtMethod( __in std::shared_ptr< ILinearSolver<Scalar> > spSolver, __in const LevMarSettings< Scalar >& settings ) {
			m_spLinearSolver = spSolver;
			m_settings = settings;
		};

		virtual void Solve(
			__in std::shared_ptr< ILeastSquaresCostFunction< Scalar, Residual > > spCostFunction,
			__inout LinearAlgebra::Vector<Scalar>& parameterVector,
			__out_opt OptimizationStatistics<Scalar>* pResult ) const override
		{
			uint residualCount = spCostFunction->GetResidualCount();
			uint parameterCount = parameterVector.GetDimension();

			LevMarData compData( parameterCount, residualCount );
			compData.lambda = m_settings.lambdaInitial;

			( *spCostFunction )( parameterVector, *compData.spResidualVector, *compData.spJacobian );
			GetCost( compData.spResidualVector, compData.cost );

			for ( uint it = 0; it < m_settings.maxIterations; it++ ) {
				ComputeDeltaParams( compData );
				compData.spParameterVectorTemp->operator+= ( parameterVector );

				if ( AcceptStep( compData, spCostFunction ) ) {
					DecreaseLambda( compData.lambda );
					AcceptTempParameters( compData, parameterVector );

					if ( FoundSolution( compData ) ) {
						WriteResult( it, compData.cost, compData.delta, pResult );
						return;
					}
				}
				else {
					IncreaseLambda( compData.lambda );
				}
			}

			WriteResult( m_settings.maxIterations, compData.cost, compData.delta, pResult );
		};

	protected:

		inline void GetCost( __in const std::shared_ptr< Vector< Scalar > > spResidualVector, __out Scalar& cost ) const {
			cost = spResidualVector->Dot( *spResidualVector );
		};

		inline bool FoundSolution( __in const LevMarData& compData ) const {
			// TODO: add "abs( min( residualVector ) ) < m_settings.tolerance";
			//			or other stopping criteria
			return ( compData.delta < m_settings.tolerance );
		};

		inline void ComputeDeltaParams( __in LevMarData& compData ) const {
			Matrix< Scalar > jacobianT = compData.spJacobian->Transpose();

			Matrix< Scalar > JTJ = jacobianT * ( *compData.spJacobian );
			JTJ.MultiplyDiag( compData.lambda + 1 );

			LinearAlgebra::Vector< Scalar > JTfNeg = -( jacobianT * ( *compData.spResidualVector ) );

			m_spLinearSolver->Solve( JTJ, JTfNeg, *compData.spParameterVectorTemp );
		};

		virtual inline bool AcceptStep( __in LevMarData& compData, __in std::shared_ptr< ILeastSquaresCostFunction< Scalar, Residual > > spCostFunction ) const {
			( *spCostFunction )( *compData.spParameterVectorTemp, *compData.spResidualVectorTemp, *compData.spJacobianTemp );
			GetCost( compData.spResidualVectorTemp, compData.costTemp );

			return ( compData.costTemp <= compData.cost );
		};

		inline void DecreaseLambda( __inout Scalar& lambda ) const {
			lambda *= m_settings.lambdaStepDown;
		};

		inline void IncreaseLambda( __inout Scalar& lambda ) const {
			lambda *= m_settings.lambdaStepUp;
		};

		inline void AcceptTempParameters( __inout LevMarData& compData, __inout Vector< Scalar >& parameterVector ) const {
			auto tmpJacobian = compData.spJacobian;
			compData.spJacobian = compData.spJacobianTemp;
			compData.spJacobianTemp = tmpJacobian;

			auto tmpReisudalVect = compData.spResidualVector;
			compData.spResidualVector = compData.spResidualVectorTemp;
			compData.spResidualVectorTemp = tmpReisudalVect;

			compData.delta = abs( compData.cost - compData.costTemp );
			compData.cost = compData.costTemp;

			parameterVector = *compData.spParameterVectorTemp;
			compData.spParameterVectorTemp->Reset();
		};

	protected:

		std::shared_ptr< ILinearSolver<Scalar> > m_spLinearSolver;
		LevMarSettings< Scalar > m_settings;

	};

}