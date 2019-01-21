#pragma once

#include "stdafx.h"
#include "StaticMatrix.h"

namespace LinearAlgebra {

	template< typename Scalar, int dimension = MatrixBase::Dynamic >
	class Vector : public Matrix<Scalar, dimension, 1> {

	public:

		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Vector() : Matrix<Scalar, dimension, 1>() {};

		Vector( __in const std::initializer_list<Scalar> elements ) : Matrix<Scalar, dimension, 1>( ) {
			if ( elements.size() != dimension ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( elements ), ExceptionReasons::DIMENSIONS_MISMATCH );
			}

			ShallowCopy<Scalar>( elements.begin(), static_cast<uint>( elements.size() ), m_pData );
		};

		Vector( __in const Matrix< Scalar, dimension, 1 >& src ) : Matrix< Scalar, dimension, 1 >( src ) {};

		virtual ~Vector() {};

		// -------------------------------------------------------------------------------------------------------
		//    Accessors overrides
		// -------------------------------------------------------------------------------------------------------
		inline const Scalar& operator[] ( __in const uint i ) const {
			if ( i >= static_cast<uint>( dimension ) ) {
				throw IndexOutOfRangeException( i, static_cast<uint>( dimension - 1 ) );
			}

			return *( m_pData + i );
		}

		inline Scalar& operator[] ( __in const uint i ) {
			if ( i >= static_cast<uint>( dimension ) ) {
				throw IndexOutOfRangeException( i, static_cast<uint>( dimension - 1 ) );
			}

			return *( m_pData + i );
		}


		inline Vector<Scalar, dimension>& operator= ( __in const Matrix<Scalar, dimension, 1>& src ) {
			ShallowCopy<Scalar>( src.Elements(), dimension, m_pData );
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Other
		// -------------------------------------------------------------------------------------------------------
		inline void Normalize() {
			Scalar norm = ( *this ) * ( *this );
			norm = 1 / sqrt( norm );
			for ( uint i = 0; i < dimension; i++ ) {
				m_pData[i] *= norm;
			}
		}

		inline Scalar DistanceSquared( __in const Vector< Scalar, dimension >& B ) const {
			Scalar dist = Scalar( 0 );
			for ( uint i = 0; i < dimension; i++ ) {
				dist += ( m_pData[i] - B.m_pData[i] ) * ( m_pData[i] - B.m_pData[i] );
			}

			return dist;
		}

		inline Scalar Distance( __in const Vector< Scalar, dimension >& B ) const {
			return sqrt( DistanceSquared( B ) );
		}

	};


}