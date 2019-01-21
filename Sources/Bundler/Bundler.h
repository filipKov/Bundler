#pragma once

namespace Bundler {

	struct LevMarSettings {
		ScalarType lambdaInitial;
		ScalarType lambdaUp;
		ScalarType lambdaDown;
		ScalarType errorTolerance;
		uint maxIterations;
	};

	struct OptimizationData {
		LinearAlgebra::Matrix< ScalarType > jacobian;
		LinearAlgebra::Vector< ScalarType > residualVector;
		
		LinearAlgebra::Matrix< ScalarType > hessian;
		LinearAlgebra::Vector< ScalarType > jtfNeg;
		bool hessianValid;

		ScalarType error;
		ScalarType lambda;

		OptimizationData( __in const uint residualCnt, __in const uint paramCnt ) : 
			jacobian( residualCnt, paramCnt ),
			residualVector( residualCnt ),
			hessian( paramCnt, paramCnt ),
			jtfNeg( paramCnt ) 
		{
			hessianValid = false;
			error = ScalarType( 0 );
			lambda = ScalarType( 0.001 );
		}
	};

	class Bundler
	{
	public:
		Bundler();
		
		void Optimize( __inout Bundle& bundle );

	protected:

		void Iterate( __inout Bundle& bundle, __in const LevMarSettings& settings );

		ScalarType ComputeError( 
			__in const Bundle& bundle, 
			__out LinearAlgebra::Matrix< ScalarType >& jacobian, 
			__out LinearAlgebra::Vector< ScalarType >& residualVector );

		ScalarType ComputeError(
			__in const Bundle& bundle,
			__in const LinearAlgebra::Vector< ScalarType >& paramDelta,
			__out LinearAlgebra::Matrix< ScalarType >& jacobian,
			__out LinearAlgebra::Vector< ScalarType >& residualVector );

		void SaveDelta( __in const LinearAlgebra::Vector< ScalarType > delta, __out Bundle& bundle );

	protected:

		std::shared_ptr< ILinearSolver< ScalarType > > m_linearSolver;

	};

}