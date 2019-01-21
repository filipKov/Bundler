#pragma once

namespace LinearAlgebra {

	template < typename Scalar >
	Matrix<Scalar> Diag( __in const Matrix<Scalar>& src ) {
		uint dim = 0;
		src.GetDimensions( dim, dim );

		Matrix<Scalar> res( dim, dim );
		for ( uint i = 0; i < dim; i++ ) {
			res[i][i] = src[i][i];
		}

		return res;
	}

	template < typename Scalar >
	Matrix<Scalar> Invert( __in const Matrix<Scalar>& diagonalMatrix ) {
		uint dim = 0;
		diagonalMatrix.GetDimensions( dim, dim );

		Matrix<Scalar> res( dim, dim );
		for ( uint i = 0; i < dim; i++ ) {
			res[i][i] = Scalar(1) / diagonalMatrix[i][i];
		}

		return res;
	}

	template < typename Scalar >
	Matrix<Scalar> LowerTriangle( __in const Matrix<Scalar>& src ) {
		uint dim = 0;
		src.GetDimensions( dim, dim );

		Matrix<Scalar> res( dim, dim );
		for ( uint r = 0; r < dim; r++ ) {
			for ( uint c = 0; c < r; c++ ) {
				res[r][c] = src[r][c];
			}
		}

		return res;
	}

}