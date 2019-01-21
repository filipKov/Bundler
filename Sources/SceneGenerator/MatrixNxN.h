#pragma once

#include "stdafx.h"

template < uint N > 
class MatrixNxN: public LinearAlgebra::Matrix< float, N, N > {
public:

	using LinearAlgebra::Matrix<float, N, N>::Matrix;

	inline MatrixNxN<N>& operator*=( __in const MatrixNxN<N>& o ) {
		auto tmp = (*this) * o;
		ShallowCopy( tmp.Elements(), N*N, m_pData );
		return *this;
	}

	inline MatrixNxN<N>& MultiplyLeft( __in const MatrixNxN<N>& o ) {
		auto tmp = o * ( *this );
		ShallowCopy( tmp.Elements(), N*N, m_pData );
		return *this;
	}

};

