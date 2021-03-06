#pragma once

namespace LinearAlgebra {

	// -------------------------------------------------------------------------------------------------------
	//    Unary Operators
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar >
	inline Matrix<Scalar, MatrixBase::Dynamic> operator- ( __in const Matrix<Scalar, MatrixBase::Dynamic>& A ) {
		Matrix<Scalar, MatrixBase::Dynamic> B( A );
		B *= Scalar( -1 );
		return B;
	};

	// -------------------------------------------------------------------------------------------------------
	//    Scalar Binary Operators
	// -------------------------------------------------------------------------------------------------------

	// -------------------------------------------------------------------------------------------------------
	//    Matrix Binary Operators
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar >
	inline Matrix<Scalar, MatrixBase::Dynamic> operator+ ( __in const Matrix<Scalar, MatrixBase::Dynamic>& A, __in const Matrix<Scalar, MatrixBase::Dynamic>& B ) {
		if ( !A.HasEqualDimensions( B ) ) {
			throw InvalidDimensionsException( GET_VARIABLE_NAME( B ) );
		}

		Matrix<Scalar, MatrixBase::Dynamic> C( A );
		C += B;
		return C;
	};

	template < typename Scalar >
	inline Matrix<Scalar, MatrixBase::Dynamic> operator- ( __in const Matrix<Scalar, MatrixBase::Dynamic>& A, __in const Matrix<Scalar, MatrixBase::Dynamic>& B ) {
		if ( !A.HasEqualDimensions( B ) ) {
			throw InvalidDimensionsException( GET_VARIABLE_NAME( B ) );
		}

		Matrix<Scalar, MatrixBase::Dynamic> C( A );
		C -= B;
		return C;
	};

	template < typename Scalar >
	inline Matrix<Scalar, MatrixBase::Dynamic> operator* ( __in const Matrix<Scalar, MatrixBase::Dynamic>& A, __in const Matrix<Scalar, MatrixBase::Dynamic>& B ) {

		uint Arows = 0, ACols = 0;
		uint BRows = 0, BCols = 0;

		A.GetDimensions( Arows, ACols );
		B.GetDimensions( BRows, BCols );

		if ( !A.CheckMultiplicationDimensions( B ) ) {
			throw InvalidDimensionsException( BRows, BCols, ACols, BCols );
		}

		Matrix<Scalar, MatrixBase::Dynamic> C( Arows, BCols );

		Scalar* pDst = C.Elements();
		Scalar* pASrc = A.Elements();
		Scalar* pBSrc = B.Elements();

		for ( uint i = 0; i < Arows; i++ ) {
			for ( uint j = 0; j < BCols; j++ ) {
				Scalar tmp = Scalar( 0 );
				for ( uint k = 0; k < ACols; k++ ) {
					tmp += ( *( pASrc + i * ACols + k ) ) * ( *( pBSrc + k * BCols + j ) );
				}
				*( pDst + i * BCols + j ) = tmp;
			}
		}

		return C;
	};

	template < typename Scalar >
	void MultiplyATB(
		__in const Matrix< Scalar, MatrixBase::Dynamic >& lhs,
		__in const Matrix< Scalar, MatrixBase::Dynamic >& rhs,
		__out Matrix< Scalar, MatrixBase::Dynamic >& res )
	{
		uint lRows = 0, lCols = 0, rRows = 0, rCols = 0, resRows = 0, resCols = 0;
		lhs.GetDimensions( lRows, lCols );
		rhs.GetDimensions( rRows, rCols );
		res.GetDimensions( resRows, resCols );

		if ( ( lRows != rRows ) || ( lCols != resRows ) || ( rCols != resCols ) ) {
			throw InvalidDimensionsException();
		}

		for ( uint row = 0; row < resRows; row++ ) {
			for ( uint col = 0; col < resCols; col++ ) {
				Scalar tmp = Scalar( 0 );
				for ( uint i = 0; i < lRows; i++ ) {
					tmp += lhs[i][row] * rhs[i][col];
				}

				res[row][col] = tmp;
			}
		}
	}

	template < typename Scalar >
	void MultiplyABT(
		__in const Matrix< Scalar, MatrixBase::Dynamic >& lhs,
		__in const Matrix< Scalar, MatrixBase::Dynamic >& rhs,
		__out Matrix< Scalar, MatrixBase::Dynamic >& res )
	{
		uint lRows = 0, lCols = 0, rRows = 0, rCols = 0, resRows = 0, resCols = 0;
		lhs.GetDimensions( lRows, lCols );
		rhs.GetDimensions( rRows, rCols );
		res.GetDimensions( resRows, resCols );

		if ( ( lCols != rCols ) || ( lRows != resRows ) || ( rRows != resCols ) ) {
			throw InvalidDimensionsException();
		}

		for ( uint row = 0; row < resRows; row++ ) {
			for ( uint col = 0; col < resCols; col++ ) {
				Scalar tmp = Scalar( 0 );
				for ( uint i = 0; i < lCols; i++ ) {
					tmp += lhs[row][i] + rhs[col][i];
				}

				res[row][col] = tmp;
			}
		}
	}

	template < typename Scalar >
	void MatrixMultiplyAtB(
		__in const uint m,
		__in const uint n,
		__in const uint p,
		__in_ecount( m * n ) const Scalar* A,
		__in_ecount( m * p ) const Scalar* B,
		__out_ecount( n * p ) Scalar* C )
	{
		for ( uint rowIx = 0; rowIx < n; rowIx++ ) 
		{
			for ( uint colIx = 0; colIx < p; colIx++ ) 
			{
				Scalar tmp = Scalar( 0 );
				for ( uint i = 0; i < m; i++ )
				{
					tmp += ELEMENT( A, i * n + rowIx ) * ELEMENT( B, i * p + colIx );
				}

				ELEMENT( C, rowIx * p + colIx ) = tmp;
			}
		}
	}

	template < typename Scalar >
	void MatrixElementWiseMultiply(
		__in const uint m,
		__in const uint n,
		__in_ecount( m * n ) const Scalar* A,
		__in_ecount( m * n ) const Scalar* B,
		__out_ecount( m * n ) Scalar* C )
	{
		for ( uint rowIx = 0; rowIx < m; rowIx++ )
		{
			for ( uint colIx = 0; colIx < n; colIx++ )
			{
				ELEMENT( C, rowIx * n + colIx ) = ELEMENT( A, rowIx * n + colIx ) * ELEMENT( B, rowIx * n + colIx );
			}
		}
	}

	template < typename Scalar >
	void MatrixIdentity( __in const uint n, __out_ecount( n * n ) Scalar* A )
	{
		ByteFill( 0, n * n, A );
		for ( uint i = 0; i < n; i++ )
		{
			ELEMENT( A, i *n + i ) = Scalar( 1 );
		}
	}

	template < typename Scalar >
	void MatrixSubC(
		__in const uint n,
		__in const uint m,
		__in_ecount( m * n ) const Scalar* A,
		__in_ecount( m * n ) const Scalar* B,
		__in const Scalar cnst,
		__out_ecount( m * n ) Scalar* C )
	{
		for ( uint r = 0; r < m; r++ )
		{
			for ( uint c = 0; c < n; c++ )
			{
				ELEMENT( C, r * n + c ) = ELEMENT( A, r * n + c ) - ( cnst * ELEMENT( B, r * n + c ) );
			}
		}
	}

	template < typename Scalar >
	void MatrixMultiplyC(
		__in const uint n,
		__in const uint m,
		__in_ecount( m * n ) const Scalar* A,
		__in const Scalar cnst,
		__out_ecount( m * n ) Scalar* B )
	{
		for ( uint r = 0; r < m; r++ )
		{
			for ( uint c = 0; c < n; c++ )
			{
				ELEMENT( B, r * n + c ) = ELEMENT( A, r * n + c ) * cnst;
			}
		}
	}

	template < typename Scalar >
	Scalar FrobeniusNorm(
		__in const uint m,
		__in const uint n,
		__in_ecount( m * n ) const Scalar* A )
	{
		Scalar norm = 0;

		Scalar tmp = 0;
		for ( uint i = 0; r < m * n; r++ )
		{
			tmp = abs( ELEMENT( A, i ) );
			norm += tmp * tmp;
		}

		return sqrt( norm );
	}

#ifdef IMPORT_AMP_LINEAR_ALGEBRA

	template < typename Scalar >
	void MatrixMultiplyAtB(
		__in const uint m,
		__in const uint n,
		__in const uint p,
		__in_ecount( m * n ) const Scalar* A,
		__in_ecount( m * p ) const Scalar* B,
		__out_ecount( n * p ) Scalar* C ) restrict( amp )
	{
		for ( uint rowIx = 0; rowIx < n; rowIx++ )
		{
			for ( uint colIx = 0; colIx < p; colIx++ )
			{
				Scalar tmp = Scalar( 0 );
				for ( uint i = 0; i < m; i++ )
				{
					tmp += ELEMENT( A, i * n + rowIx ) * ELEMENT( B, i * p + colIx );
				}

				ELEMENT( C, rowIx * p + colIx ) = tmp;
			}
		}
	}

	template < typename Scalar >
	void MatrixElementWiseMultiply(
		__in const uint m,
		__in const uint n,
		__in_ecount( m * n ) const Scalar* A,
		__in_ecount( m * n ) const Scalar* B,
		__out_ecount( m * n ) Scalar* C )  restrict( amp )
	{
		for ( uint rowIx = 0; rowIx < m; rowIx++ )
		{
			for ( uint colIx = 0; colIx < n; colIx++ )
			{
				ELEMENT( C, rowIx * n + colIx ) = ELEMENT( A, rowIx * n + colIx ) * ELEMENT( B, rowIx * n + colIx );
			}
		}
	}

	template < typename Scalar >
	void MatrixIdentity( __in const uint n, __out_ecount( n * n ) Scalar* A )  restrict( amp )
	{
		Containers::ArrayUtils< Scalar >::Fill( T( 0 ), n * n, A );
		for ( uint i = 0; i < n; i++ )
		{
			ELEMENT( A, i *n + i ) = Scalar( 1 );
		}
	}

	template < typename Scalar >
	void MatrixSubC(
		__in const uint n,
		__in const uint m,
		__in_ecount( m * n ) const Scalar* A,
		__in_ecount( m * n ) const Scalar* B,
		__in const Scalar cnst,
		__out_ecount( m * n ) Scalar* C )  restrict( amp )
	{
		for ( uint r = 0; r < m; r++ )
		{
			for ( uint c = 0; c < n; c++ )
			{
				ELEMENT( C, r * n + c ) = ELEMENT( A, r * n + c ) - ( cnst * ELEMENT( B, r * n + c ) );
			}
		}
	}

	template < typename Scalar >
	void MatrixMultiplyC(
		__in const uint n,
		__in const uint m,
		__in_ecount( m * n ) const Scalar* A,
		__in const Scalar cnst,
		__out_ecount( m * n ) Scalar* B )  restrict( amp )
	{
		for ( uint r = 0; r < m; r++ )
		{
			for ( uint c = 0; c < n; c++ )
			{
				ELEMENT( B, r * n + c ) = ELEMENT( A, r * n + c ) * cnst;
			}
		}
	}

	template < typename Scalar >
	Scalar FrobeniusNorm(
		__in const uint m,
		__in const uint n,
		__in_ecount( m * n ) const Scalar* A )  restrict( amp )
	{
		Scalar norm = 0;

		for ( uint i = 0; r < m * n; r++ )
		{
			norm += ELEMENT( A, i ) * ELEMENT( A, i );
		}

		return concurrency::precise_math::sqrt( norm );
	}

#endif

}
