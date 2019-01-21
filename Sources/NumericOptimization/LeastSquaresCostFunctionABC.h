#pragma once

#include "stdafx.h"
#include <vector>
#include "ILeastSquaresCostFunction.h"

namespace NumericOptimization {

	template < typename Scalar, class Residual >
	class LeastSquaresCostFunctionABC : public ILeastSquaresCostFunction< Scalar, Residual > {

	public:

		LeastSquaresCostFunctionABC() {};

		inline void AddResidual( __in const Residual& residual ) override final {
			m_residuals.push_back( residual );
		};

		inline void Reset() override final {
			m_residuals.clear();
		};

		inline uint GetResidualCount() const override {
			return static_cast<uint>( m_residuals.size() );
		};

		inline const Residual& GetResidual( __in const uint index ) const override {
			if ( index >= m_residuals.size() ) {
				throw IndexOutOfRangeException( index, static_cast<uint>( m_residuals.size() - 1 ) );
			}

			return m_residuals[index];
		};

	protected:

		std::vector< Residual > m_residuals;

	};

}