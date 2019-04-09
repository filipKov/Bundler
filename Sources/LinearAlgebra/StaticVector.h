#pragma once

namespace LinearAlgebra {

	template< typename Scalar, int dimension = MatrixBase::Dynamic >
	class Vector : public Matrix<Scalar, dimension, 1> {

	public:

		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Vector() 
			: Matrix<Scalar, dimension, 1>() 
		{};

		Vector( __in const std::initializer_list<Scalar> elements ) : Matrix<Scalar, dimension, 1>( ) 
		{
			if ( elements.size() != dimension ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( elements ), ExceptionReasons::DIMENSIONS_MISMATCH );
			}

			ShallowCopy<Scalar>( elements.begin(), static_cast<uint>( elements.size() ), m_pData );
		};

		Vector( __in const Scalar (&values)[ dimension ] ) 
		{
			ShallowCopy< Scalar >( values, dimension, m_pData );
		}

		Vector( __in const Matrix< Scalar, dimension, 1 >& src ) 
			: Matrix< Scalar, dimension, 1 >( src ) 
		{};

		// -------------------------------------------------------------------------------------------------------
		//    Accessors overrides
		// -------------------------------------------------------------------------------------------------------
		inline const Scalar& operator[] ( __in const uint i ) const 
		{
			_ASSERT_EXPR( i < ( uint )dimension, "Index out of range" );

			return *( m_pData + i );
		}

		inline Scalar& operator[] ( __in const uint i ) 
		{
			_ASSERT_EXPR( i < ( uint )dimension, "Index out of range" );

			return *( m_pData + i );
		}


		inline Vector<Scalar, dimension>& operator= ( __in const Matrix<Scalar, dimension, 1>& src ) 
		{
			ShallowCopy<Scalar>( src.Elements(), dimension, m_pData );
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Other
		// -------------------------------------------------------------------------------------------------------
		inline void Normalize() 
		{
			Scalar norm = Scalar( 1 ) / sqrt( VectorDot< Scalar, dimension >( m_pData, m_pData ) );
			MatrixMultiplyC< Scalar, dimension, 1 >( m_pData, norm, m_pData );
		}

		inline Scalar DistanceSquared( __in const Vector< Scalar, dimension >& B ) const 
		{
			Scalar dist = Scalar( 0 );
			for ( uint i = 0; i < dimension; i++ ) {
				dist += ( m_pData[i] - B.m_pData[i] ) * ( m_pData[i] - B.m_pData[i] );
			}

			return dist;
		}

		inline Scalar Distance( __in const Vector< Scalar, dimension >& B ) const 
		{
			return sqrt( DistanceSquared( B ) );
		}

	#ifdef IMPORT_AMP_LINEAR_ALGEBRA

		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Vector( ) restrict( amp )
			: Matrix<Scalar, dimension, 1>( )
		{};

		Vector( __in const Scalar ( &values )[dimension] ) restrict( amp )
		{
			Containers::ArrayUtils< Scalar >::Copy< dimension >( values, m_pData );
		}

		Vector( __in const Matrix< Scalar, dimension, 1 >& src ) restrict( amp )
			: Matrix< Scalar, dimension, 1 >( src )
		{
		};

		// -------------------------------------------------------------------------------------------------------
		//    Accessors overrides
		// -------------------------------------------------------------------------------------------------------
		inline const Scalar& operator[] ( __in const uint i ) const restrict( amp )
		{
			return *( m_pData + i );
		}

		inline Scalar& operator[] ( __in const uint i ) restrict( amp )
		{
			return *( m_pData + i );
		}

		inline Vector<Scalar, dimension>& operator= ( __in const Matrix<Scalar, dimension, 1>& src ) restrict( amp )
		{
			Containers::ArrayUtils< Scalar >::Copy< dimension >( src.Elements(), m_pData );
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Other
		// -------------------------------------------------------------------------------------------------------
		inline void Normalize( ) restrict( amp )
		{
			Scalar norm = Scalar( 1 ) / concurrency::precise_math::sqrt( VectorDot< Scalar, dimension >( m_pData, m_pData ) );
			MatrixMultiplyC< Scalar, dimension, 1 >( m_pData, norm, m_pData );
		}

		inline Scalar DistanceSquared( __in const Vector< Scalar, dimension >& B ) const restrict( amp )
		{
			Scalar dist = Scalar( 0 );
			for ( uint i = 0; i < dimension; i++ )
			{
				dist += ( m_pData[i] - B.m_pData[i] ) * ( m_pData[i] - B.m_pData[i] );
			}

			return dist;
		}

		inline Scalar Distance( __in const Vector< Scalar, dimension >& B ) const restrict( amp )
		{
			return concurrency::precise_math::sqrt( DistanceSquared( B ) );
		}

	#endif

	};

}
