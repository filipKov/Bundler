#pragma once

#include "stdafx.h"
#include "StaticVector.h"
#include "DynamicMatrix.h"

namespace LinearAlgebra {

	template < typename Scalar >
	class Vector< Scalar, MatrixBase::Dynamic > : public Matrix< Scalar, MatrixBase::Dynamic > {
	
	public:
	
		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Vector( __in const uint dimension ) : Matrix< Scalar, MatrixBase::Dynamic >( dimension, 1 ) {};
	
		Vector( __in const uint dimension, __in Scalar* pData ) : Matrix< Scalar, MatrixBase::Dynamic >( dimension, 1, pData ) {};

		Vector( __in const std::initializer_list<Scalar> elements ) : Matrix< Scalar, MatrixBase::Dynamic >( static_cast<uint>( elements.size() ), 1 ) {
			ShallowCopy<Scalar>( elements.begin(), elements.size(), m_pData );
		};

		Vector( __in const Vector< Scalar, MatrixBase::Dynamic >& src ) : Matrix< Scalar, MatrixBase::Dynamic >( src ) {};
	
		Vector( __in Vector< Scalar, MatrixBase::Dynamic >&& src ) : Matrix< Scalar, MatrixBase::Dynamic >( std::move( src ) ) {};
	
		virtual ~Vector() {};
	
	
		// -------------------------------------------------------------------------------------------------------
		//    Accessors overrides
		// -------------------------------------------------------------------------------------------------------
		inline Scalar& operator[] ( __in const uint i ) const {
			if ( i >= m_rows ) {
				throw IndexOutOfRangeException( i, m_rows - 1 );
			}
	
			return *( m_pData + i );
		}
	
		// -------------------------------------------------------------------------------------------------------
		//    Operators
		// -------------------------------------------------------------------------------------------------------
		inline Vector< Scalar, MatrixBase::Dynamic >& operator= ( __in const Vector< Scalar, MatrixBase::Dynamic >& src ) {
			SetTo( src.m_rows, src.m_pData );
			return *this;
		};
	
		// -------------------------------------------------------------------------------------------------------
		//    Ultilities + Other
		// -------------------------------------------------------------------------------------------------------
		inline Scalar Dot( __in const Vector< Scalar, MatrixBase::Dynamic >& B ) const {
			if ( !HasEqualDimensions( B.m_rows ) ) {
				throw InvalidDimensionsException( B.m_rows, B.m_columns, m_rows, m_columns );
			}
	
			Scalar dotProd = Scalar( 0 );
			Scalar* pA = m_pData;
			Scalar* pB = B.m_pData;
	
			for ( uint i = 0; i < m_rows; i++ ) {
				dotProd += ( *pA ) * ( *pB );
				pA++;
				pB++;
			}
	
			return dotProd;
		};
	
		inline Scalar Length() const {
			return static_cast<Scalar>( sqrt( Dot( *this ) ) );
		};
	
		inline uint GetDimension() const {
			return m_rows;
		};
	
		inline void SetTo( __in const uint n, __in const Scalar* pElements ) {
			if ( !HasEqualDimensions( n ) ) {
				ResizeTo( n, 1 );
			}
	
			ShallowCopy<Scalar>( pElements, n, m_pData );
		};
	
		inline bool HasEqualDimensions( __in const uint n ) const {
			return ( m_rows == n );
		};
	
	};

}