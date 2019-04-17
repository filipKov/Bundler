#pragma once

namespace LinearAlgebra {

	namespace MatrixBase {
		const int Dynamic = -1;
	}

	template < typename Scalar, int rows = MatrixBase::Dynamic, int... > 
	class Matrix {
	
	public:
		Matrix() = delete;
		Matrix( __in const Matrix& src ) = delete;
		Matrix( __in Matrix&& src ) = delete;
	};	

}