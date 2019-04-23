#pragma once

namespace LinearAlgebra {

	// -------------------------------------------------------------------------------------------------------
	//    Dense Matrix class, with compile time given dimensions
	//
	//		Add Operators and methods as needed
	//		
	//		Implementation Queue:
	//			1)
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar, uint rows, uint columns >
	class Matrix< Scalar, rows, columns > {

	public:

		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Matrix() 
		{
			Initialize();
		}

		Matrix( __in const std::initializer_list< std::initializer_list<Scalar> > list ) 
		{
			if ( list.size() != rows || list.begin()->size() != columns ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( list ), ExceptionReasons::DIMENSIONS_MISMATCH );
			}

			Initialize();

			Scalar* pRowDst = m_pData;
			for ( const std::initializer_list<Scalar> *prowsrc = list.begin(); prowsrc != list.end(); prowsrc++ ) {
				ShallowCopy<Scalar>( prowsrc->begin(), columns, pRowDst );
				pRowDst += columns;
			}
		}

		Matrix( __in const Scalar (&values)[ rows * columns ] ) 
		{
			ShallowCopy< Scalar >( values, rows * columns, m_pData );
		}

		Matrix( __in const Matrix<Scalar, rows, columns>& src ) = default;

		// -------------------------------------------------------------------------------------------------------
		//    Accessors
		// -------------------------------------------------------------------------------------------------------
		inline const Scalar* Elements() const 
		{
			return m_pData;
		}

		inline Scalar* Elements() 
		{
			return m_pData;
		}

		// Retrieves element at given "coordinates" in Matrix 
		inline const Scalar& Get( __in const uint row, __in const uint column ) const 
		{
			_ASSERT_EXPR( ( row < rows ) && ( column < columns ), "Index out of range" );

			return *( m_pData + row * columns + column );
		}

		inline Scalar& Get( __in const uint row, __in const uint column ) 
		{
			_ASSERT_EXPR( ( row < rows ) && ( column < columns ), "Index out of range" );

			return *( m_pData + row * columns + column );
		}

		// Sets element to given value
		inline void Set( __in const uint row, __in const uint column, __in const Scalar& value ) 
		{
			_ASSERT_EXPR( ( row < rows ) && ( column < columns ), "Index out of range" );

			*( m_pData + row * columns + column ) = value;
		}

		inline const Scalar* operator[] ( __in const uint row ) const 
		{
			_ASSERT_EXPR( row < rows, "Index out of range" );

			return ( m_pData + row * columns );
		}

		inline Scalar* operator[] ( __in const uint row )
		{
			_ASSERT_EXPR( row < rows, "Index out of range" );

			return ( m_pData + row * columns );
		}

		// -------------------------------------------------------------------------------------------------------
		//    Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix<Scalar, rows, columns>& operator= ( __in const Matrix<Scalar, rows, columns>& src ) = default;

		// -------------------------------------------------------------------------------------------------------
		//    Scalar Operators
		// -------------------------------------------------------------------------------------------------------	
		inline Matrix<Scalar, rows, columns>& operator*= ( __in const Scalar& o ) 
		{
			MatrixMultiplyC< Scalar, rows, columns >( m_pData, o, m_pData );
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Matrix Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix<Scalar, rows, columns>& operator+= ( __in const Matrix<Scalar, rows, columns>& o ) {
			MatrixAdd< Scalar, rows, columns >( m_pData, o.m_pData, m_pData );
			return *this;
		}

		inline Matrix<Scalar, rows, columns>& operator-= ( __in const Matrix<Scalar, rows, columns>& o ) {
			MatrixSub< Scalar, rows, columns >( m_pData, o.m_pData, m_pData );
			return *this;
		}

		// -------------------------------------------------------------------------------------------------------
		//    Other
		// -------------------------------------------------------------------------------------------------------
		inline void SetIdentity() {
			MatrixIdentity< Scalar, rows >( m_pData );
		}

		inline Matrix< Scalar, columns, rows > Transpose() {
			Matrix< Scalar, columns, rows > T;
			for ( uint i = 0; i < rows; i++ ) {
				for ( uint j = 0; j < columns; j++ ) {
					T[ j ][ i ] = m_pData[ i * columns + j ];
				}
			}

			return T;
		}

	protected:

		void Initialize() 
		{
			ByteFill<Scalar>( 0, rows*columns, m_pData );
		}


	#ifdef IMPORT_AMP_LINEAR_ALGEBRA

	public:
		
		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Matrix( ) restrict( amp )
		{
			Initialize( );
		}

		Matrix( __in const Scalar ( &values )[rows * columns] ) restrict( amp )
		{
			Containers::ArrayUtils< Scalar >::Copy< rows * columns >( values, m_pData );
		}

		// Matrix( __in const Matrix<Scalar, rows, columns>& src ) restrict( amp ) = default;

		// -------------------------------------------------------------------------------------------------------
		//    Accessors
		// -------------------------------------------------------------------------------------------------------
		inline const Scalar* Elements( ) const restrict( amp )
		{
			return m_pData;
		}

		inline Scalar* Elements( ) restrict( amp )
		{
			return m_pData;
		}

		// Retrieves element at given "coordinates" in Matrix 
		inline const Scalar& Get( __in const uint row, __in const uint column ) const restrict( amp )
		{
			return *( m_pData + row * columns + column );
		}

		inline Scalar& Get( __in const uint row, __in const uint column ) restrict( amp )
		{
			return *( m_pData + row * columns + column );
		}

		// Sets element to given value
		inline void Set( __in const uint row, __in const uint column, __in const DefaultItemType< Scalar > value ) restrict( amp )
		{
			*( m_pData + row * columns + column ) = value;
		}

		inline const Scalar* operator[] ( __in const uint row ) const restrict( amp )
		{
			return ( m_pData + row * columns );
		}

		inline Scalar* operator[] ( __in const uint row ) restrict( amp )
		{
			return ( m_pData + row * columns );
		}

		// -------------------------------------------------------------------------------------------------------
		//    Operators
		// -------------------------------------------------------------------------------------------------------
		// inline Matrix<Scalar, rows, columns>& operator= ( __in const Matrix<Scalar, rows, columns>& src ) restrict( amp ) = default;

		// -------------------------------------------------------------------------------------------------------
		//    Scalar Operators
		// -------------------------------------------------------------------------------------------------------	
		inline Matrix<Scalar, rows, columns>& operator*= ( __in const Scalar& o ) restrict( amp )
		{
			MatrixMultiplyC< Scalar, rows, columns >( m_pData, o, m_pData );
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Matrix Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix<Scalar, rows, columns>& operator+= ( __in const Matrix<Scalar, rows, columns>& o ) restrict( amp )
		{
			MatrixAdd< Scalar, rows, columns >( m_pData, o.m_pData, m_pData );
			return *this;
		}

		inline Matrix<Scalar, rows, columns>& operator-= ( __in const Matrix<Scalar, rows, columns>& o ) restrict( amp )
		{
			MatrixSub< Scalar, rows, columns >( m_pData, o.m_pData, m_pData );
			return *this;
		}

		// -------------------------------------------------------------------------------------------------------
		//    Other
		// -------------------------------------------------------------------------------------------------------
		inline void SetIdentity( ) restrict( amp )
		{
			MatrixIdentity< Scalar, rows >( m_pData );
		}

		inline Matrix< Scalar, columns, rows > Transpose( ) restrict( amp )
		{
			Matrix< Scalar, columns, rows > T;
			for ( uint i = 0; i < rows; i++ )
			{
				for ( uint j = 0; j < columns; j++ )
				{
					T[j][i] = m_pData[i * columns + j];
				}
			}

			return T;
		}

	protected:

		void Initialize( ) restrict( amp )
		{
			Containers::ArrayUtils< Scalar >::Fill< rows * columns >( Scalar( 0 ), m_pData );
		}

	#endif

	protected:

		Scalar m_pData[ rows * columns ];

	};

}
