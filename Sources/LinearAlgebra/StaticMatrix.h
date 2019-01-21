#pragma once

#include "stdafx.h"
#include "Matrix.h"

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
		Matrix() {
			Initialize();
		}

		Matrix( __in const std::initializer_list< std::initializer_list<Scalar> > list ) {
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

		Matrix( __in const Matrix<Scalar, rows, columns>& src ) {
			Initialize();
			ShallowCopy<Scalar>( src.m_pData, rows*columns, m_pData );
		}

		// -------------------------------------------------------------------------------------------------------
		//    Accessors
		// -------------------------------------------------------------------------------------------------------
		inline const Scalar* Elements() const {
			return m_pData;
		}

		inline Scalar* Elements() {
			return m_pData;
		}

		// Retrieves element at given "coordinates" in Matrix 
		inline const Scalar& Get( __in const uint row, __in const uint column ) const {
			if ( ( row >= rows ) || ( column >= columns ) ) {
				throw IndexOutOfRangeException( row, column, rows - 1, columns - 1 );
			}

			return *( m_pData + row * columns + column );
		}

		inline Scalar& Get( __in const uint row, __in const uint column ) {
			if ( ( row >= rows ) || ( column >= columns ) ) {
				throw IndexOutOfRangeException( row, column, rows - 1, columns - 1 );
			}

			return *( m_pData + row * columns + column );
		}

		// Sets element to given value
		inline void Set( __in const uint row, __in const uint column, __in const Scalar& value ) {
			if ( ( row >= rows ) || ( column >= columns ) ) {
				throw IndexOutOfRangeException( row, column, rows - 1, columns - 1 );
			}

			*( m_pData + row * columns + column ) = value;
		}

		inline const Scalar* operator[] ( __in const uint row ) const {
			if ( row >= rows ) {
				throw IndexOutOfRangeException( row, rows - 1 );
			}

			return ( m_pData + row * columns );
		}

		inline Scalar* operator[] ( __in const uint row ) {
			if ( row >= rows ) {
				throw IndexOutOfRangeException( row, rows - 1 );
			}

			return ( m_pData + row * columns );
		}

		// -------------------------------------------------------------------------------------------------------
		//    Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix<Scalar, rows, columns>& operator= ( __in const Matrix<Scalar, rows, columns>& src ) {
			ShallowCopy<Scalar>( src.m_pData, rows*columns, m_pData );
			return *this;
		}

		// -------------------------------------------------------------------------------------------------------
		//    Scalar Operators
		// -------------------------------------------------------------------------------------------------------	
		inline Matrix<Scalar, rows, columns>& operator*= ( __in const Scalar& o ) {
			Scalar* pDst = m_pData;
			for ( uint i = 0; i < ( rows * columns ); i++ ) {
				*pDst *= o;
				pDst++;
			}

			return *this;
		};

		// -------------------------------------------------------------------------------------------------------
		//    Matrix Operators
		// -------------------------------------------------------------------------------------------------------
		inline Matrix<Scalar, rows, columns>& operator+= ( __in const Matrix<Scalar, rows, columns>& o ) {
			const Scalar* pSrc = o.Elements();
			Scalar* pDst = m_pData;
			for ( uint i = 0; i < ( rows * columns ); i++ ) {
				*pDst += *pSrc;
				pSrc++;
				pDst++;
			}

			return *this;
		}

		inline Matrix<Scalar, rows, columns>& operator-= ( __in const Matrix<Scalar, rows, columns>& o ) {
			const Scalar* pSrc = o.Elements();
			Scalar* pDst = m_pData;
			for ( uint i = 0; i < ( rows * columns ); i++ ) {
				*pDst -= *pSrc;
				pSrc++;
				pDst++;
			}

			return *this;
		}

	protected:

		void Initialize() {
			ByteFill<Scalar>( 0, rows*columns, m_pData );
		}

	protected:

		Scalar m_pData[ rows * columns ];

	};

}
