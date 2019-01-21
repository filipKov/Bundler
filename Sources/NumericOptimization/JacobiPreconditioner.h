#pragma once

namespace NumericOptimization { namespace LinearSolver {

	template < typename Scalar >
	class JacobiPreconditioner: public IPreconditioner< Scalar > {
	public:

		virtual void Initialize( __in const LinearAlgebra::Matrix< Scalar >& sourceMatrix ) {
			m_pSourceMatrix = &sourceMatrix;
		}

		virtual void Apply( __in const LinearAlgebra::Vector< Scalar >& x, __out LinearAlgebra::Vector< Scalar >& p ) {
			for ( uint rowI = 0; rowI < x.GetDimension(); rowI++ ) {
				p[ rowI ] = x[ rowI ];

				Scalar& diagElement = ( *m_pSourceMatrix )[ rowI ][ rowI ];
				if ( diagElement != 0 ) {
					p[ rowI ] /= diagElement;
				}
			}
		}

	protected:

		const LinearAlgebra::Matrix< Scalar >* m_pSourceMatrix;

	};

} }