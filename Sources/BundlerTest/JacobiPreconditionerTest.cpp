#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace NumericOptimization::LinearSolver;

namespace BundlerTest
{

	TEST_CLASS( JacobiPreconditionerTest )
	{
	public:

		void Init() {
			if ( spPreconditioner == nullptr ) {
				spPreconditioner = std::shared_ptr< IPreconditioner< float > >( new JacobiPreconditioner< float >() );
			}
		}

		~JacobiPreconditionerTest() {
			spPreconditioner = nullptr;
		}

		TEST_METHOD( TestSourceIdentity ) {
			Init();
		
			LinearAlgebra::Matrix< float > I( { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } );
		
			spPreconditioner->Initialize( I );
		
			LinearAlgebra::Vector< float > x( { 1, 5, 8 } );
			LinearAlgebra::Vector< float > res( 3 );
		
			spPreconditioner->Apply( x, res );
		
			AssertAreEqual( 3, x.Elements(), res.Elements() );
		}

		TEST_METHOD( TestZeros ) {
			Init();

			LinearAlgebra::Matrix<float> M( { { 5,0,0 }, { 0,0,0 }, { 0,0,0 } } );

			spPreconditioner->Initialize( M );

			LinearAlgebra::Vector< float > x( { 1, 5, 8 } );
			LinearAlgebra::Vector< float > res( 3 );
			LinearAlgebra::Vector< float > expected( { 0.2f, 5, 8 } );

			spPreconditioner->Apply( x, res );

			AssertAreEqual( 3, expected.Elements(), res.Elements() );
		}

		LinearAlgebra::Matrix< float > GetDiagonalInverse( __in const LinearAlgebra::Matrix< float >& A ) {		
			uint rows = 0, cols = 0;;
			A.GetDimensions( rows, cols );

			LinearAlgebra::Matrix< float > res( rows, cols );
			for ( uint i = 0; i < rows; i++ ) {
				res[ i ][ i ] = 1.0f / A[ i ][ i ];
			}

			return res;
		}

		void TestJacobi( __in LinearAlgebra::Matrix< float >& A, __in LinearAlgebra::Vector< float >& x ) {
			LinearAlgebra::Vector< float > actualResult( x.GetDimension() );
		
			spPreconditioner->Initialize( A );
			spPreconditioner->Apply( x, actualResult );

			auto M = GetDiagonalInverse( A );
			auto expectedResult = M * x;
		
			AssertAreEqual( x.GetDimension(), expectedResult.Elements(), actualResult.Elements(), 10e-4f );
		}

		TEST_METHOD( TestJacobi3x3_0 ) {
			Init();

			LinearAlgebra::Matrix<float> M( { 
				{ 1, 2, 3 },
				{ 4, 5, 6 },
				{ 7, 8, 9 } } 
			);

			LinearAlgebra::Vector< float > x( { 1, 5, 8 } );

			TestJacobi( M, x );
		}
		
		TEST_METHOD( TestJacobi3x3_1 ) {
			Init();

			LinearAlgebra::Matrix<float> M( {
				{ 43.0f, 1.342f, 0.0f },
				{ 6.0f, 1.432f, 99.0f },
				{ 123.0f, 456.0f, 9.433f } }
			);

			LinearAlgebra::Vector< float > x( { 12.98f, 5.22f, 23.211f } );

			TestJacobi( M, x );
		}

		TEST_METHOD( TestJacobi4x4_0 ) {
			Init();

			LinearAlgebra::Matrix<float> M( {
				{ 5.23f, 7.324f, 0.0f, 1.0f },
				{ 96.32f, 8.222f, 9.1123f, 19.98f },
				{ 0.0f, 0.0f, 17.3265f, 1.25f },
				{ 123.0f, 456.0f, 9.433f, 4.589f } }
			);

			LinearAlgebra::Vector< float > x( { 2.4f, 9.8f, 12.9004f, 5.107f } );

			TestJacobi( M, x );
		}

	protected:

		std::shared_ptr< IPreconditioner< float > > spPreconditioner;

	};

}