#pragma once

namespace LinearAlgebra {

	// -------------------------------------------------------------------------------------------------------
	//    Matrix class, with runtime given dimensions
	//
	//		Add Operators and methods as needed
	//		
	//		Implementation Queue:
	//			1)
	// -------------------------------------------------------------------------------------------------------
	template < typename Scalar >
	class Matrix< Scalar, MatrixBase::Dynamic > {

		using Allocator = ::Utils::MemoryAllocator<Scalar>;

	public:

		// -------------------------------------------------------------------------------------------------------
		//    Constructors/Initializers/Destructors
		// -------------------------------------------------------------------------------------------------------
		Matrix( __in const uint rows, __in const uint columns ) {
			Initialize( rows, columns );
		};

		Matrix( 
			__in const uint rows, 
			__in const uint columns, 
			__in_ecount( rows * columns ) Scalar* pData )
		{
			Initialize( rows, columns, pData );
		}

		Matrix( __in const std::initializer_list< std::initializer_list<Scalar> > list ) {
			Initialize( static_cast<uint>( list.size() ), static_cast<uint>( list.begin()->size() ) );

			Scalar *pRowDst = m_pData;
			for ( const std::initializer_list<Scalar>* pRowSrc = list.begin(); pRowSrc < list.end(); pRowSrc++ ) {
				ShallowCopy<Scalar>( pRowSrc->begin(), m_columns, pRowDst );
				pRowDst += m_columns;
			}
		};

		Matrix( __in const Matrix< Scalar, MatrixBase::Dynamic >& src ) {
			Initialize( src.m_rows, src.m_columns );
			ShallowCopy<Scalar>( src.m_pData, m_rows * m_columns, m_pData );
		};

		Matrix( __in Matrix< Scalar, MatrixBase::Dynamic >&& moveSrc ) {
			m_rows = moveSrc.m_rows;
			m_columns = moveSrc.m_columns;
			m_pData = moveSrc.m_pData;
			m_externalDataAttached = moveSrc.m_externalDataAttached;

			moveSrc.m_pData = nullptr;
			moveSrc.m_rows = 0;
			moveSrc.m_columns = 0;
		};

		~Matrix() {
			Release();
		}

		// -------------------------------------------------------------------------------------------------------
		//    Accessors
		// -------------------------------------------------------------------------------------------------------
		inline Scalar* Elements() const {
			return m_pData;
		};

		inline Scalar& Get( __in const uint row, __in const uint column ) const {
			if ( ( row >= m_rows ) || ( column >= m_columns ) ) {
				throw IndexOutOfRangeException( row, column, m_rows - 1, m_columns - 1 );
			}

			return *( m_pData + row * m_columns + column );
		};

		inline void Set( __in const uint row, __in const uint column, __in const Scalar& value ) {
			if ( ( row >= m_rows ) || ( column >= m_columns ) ) {
				throw IndexOutOfRangeException( row, column, m_rows - 1, m_columns - 1 );
			}

			*( m_pData + row * m_columns + column ) = value;
		};

		inline Scalar* operator[] ( __in const uint row ) const {
			if ( row >= m_rows ) {
				throw IndexOutOfRangeException( row, m_rows - 1 );
			}

			return ( m_pData + row * m_columns );
		};

		// -------------------------------------------------------------------------------------------------------
		//    Utilities
		// -------------------------------------------------------------------------------------------------------
		inline bool HasEqualDimensions( __in const Matrix< Scalar, MatrixBase::Dynamic >& o ) const {
			return ( ( m_rows == o.m_rows ) && ( m_columns == o.m_columns ) );
		};

		inline bool CheckMultiplicationDimensions( __in const Matrix< Scalar, MatrixBase::Dynamic >& o ) const {
			return ( m_columns == o.m_rows );
		};

		inline void GetDimensions( __out uint& rows, __out uint& cols ) const {
			rows = m_rows;
			cols = m_columns;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix< Scalar, MatrixBase::Dynamic >& operator= ( __in const Matrix< Scalar, MatrixBase::Dynamic >& src ) {
			if ( !HasEqualDimensions( src ) ) {
				ResizeTo( src.m_rows, src.m_columns );
			}

			ShallowCopy<Scalar>( src.m_pData, m_rows * m_columns, m_pData );
			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Scalar Operators
		// -------------------------------------------------------------------------------------------------------	
		inline Matrix< Scalar, MatrixBase::Dynamic >& operator*= ( __in const Scalar& cnst ) {
			Scalar* pDst = m_pData;
			for ( uint i = 0; i < ( m_rows * m_columns ); i++ ) {
				*pDst *= cnst;
				pDst++;
			}

			return *this;
		}

		// -------------------------------------------------------------------------------------------------------
		//    Matrix Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix< Scalar, MatrixBase::Dynamic >& operator+= ( __in const Matrix< Scalar, MatrixBase::Dynamic >& o ) {
			if ( !HasEqualDimensions( o ) ) {
				throw InvalidDimensionsException( GET_VARIABLE_NAME( o ), o.m_rows, o.m_columns, m_rows, m_columns );
			}

			Scalar* pDst = m_pData;
			Scalar* pElements = o.m_pData;
			for ( uint i = 0; i < ( m_rows * m_columns ); i++ ) {
				*pDst += *pElements;
				pDst++;
				pElements++;
			}

			return *this;
		};

		inline Matrix< Scalar, MatrixBase::Dynamic >& operator-= ( __in const Matrix< Scalar, MatrixBase::Dynamic >& o ) {
			if ( !HasEqualDimensions( o ) ) {
				throw InvalidDimensionsException( GET_VARIABLE_NAME( o ), o.m_rows, o.m_columns, m_rows, m_columns );
			}

			Scalar* pDst = m_pData;
			Scalar* pElements = o.m_pData;
			for ( uint i = 0; i < ( m_rows * m_columns ); i++ ) {
				*pDst -= *pElements;
				pDst++;
				pElements++;
			}

			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Other
		// -------------------------------------------------------------------------------------------------------
		inline Matrix< Scalar, MatrixBase::Dynamic > Transpose() const {
			Matrix< Scalar, MatrixBase::Dynamic > B( m_columns, m_rows );
			Scalar* pDst = B.m_pData;
			Scalar* pSrc = m_pData;

			for ( uint r = 0; r < m_rows; r++ ) {
				for ( uint c = 0; c < m_columns; c++ ) {
					*( pDst + c * m_rows + r ) = *( pSrc + r * m_columns + c );
				}
			}

			return B;
		};

		inline void Negate()
		{
			for ( uint i = 0; i < m_rows * m_columns; i++ )
			{
				ELEMENT( m_pData, i ) = -ELEMENT( m_pData, i );
			}
		}

		inline void MultiplyDiag( __in const Scalar& cnst ) {
			if ( !CheckSquare() ) {
				throw InvalidDimensionsException( GET_VARIABLE_NAME( this ) );
			}

			Scalar* pDiag = m_pData;

			for ( uint r = 0; r < m_rows; r++ ) {
				*pDiag *= cnst;
				pDiag += m_columns + 1;
			}
		};

		inline bool CheckSquare() const {
			return m_rows == m_columns;
		};

		inline void Reset() {
			ByteFill( 0, m_rows * m_columns, m_pData );
		};


	protected:

		// -------------------------------------------------------------------------------------------------------
		//    Utilities
		// -------------------------------------------------------------------------------------------------------

		void Initialize( __in const uint rows, __in const uint columns ) {
			m_rows = rows;
			m_columns = columns;

			m_externalDataAttached = false;
			m_pData = Allocator::Allocate( rows * columns );
			memset( m_pData, 0, rows * columns * sizeof( Scalar ) );
		};

		void Initialize( 
			__in const uint rows, 
			__in const uint columns, 
			__in_ecount( rows * columns ) Scalar* pData )
		{
			m_rows = rows;
			m_columns = columns;
			m_externalDataAttached = true;
			m_pData = pData;
		}

		void Release() {
			if ( m_pData && !m_externalDataAttached ) {
				Allocator::Release( m_pData );
			}

			m_pData = nullptr;
		};

		void ResizeTo( __in const uint rows, __in const uint columns ) {
			if ( ( rows != m_rows ) || ( columns != m_columns ) )
			{
				Release();
				Initialize( rows, columns );
			}
		}

	protected:

		uint m_rows;
		uint m_columns;

		bool m_externalDataAttached;
		Scalar* m_pData;

	};

}