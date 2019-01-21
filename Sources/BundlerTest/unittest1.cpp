#include "stdafx.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace NumericOptimization::LinearSolver;

namespace BundlerTest
{		
	TEST_CLASS( SsorPreconditionerTest )
	{
	public:
		
		void Init() {
			if ( spPreconditioner == nullptr ) {
				spPreconditioner = std::shared_ptr< IPreconditioner< float > >( new SsorPreconditioner< float >( 1.0f ) );
			}
		}

		~SsorPreconditionerTest() {
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

		TEST_METHOD( TestSourceOnes ) {
			Init();

			LinearAlgebra::Matrix< float > A( { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } } );

			spPreconditioner->Initialize( A );

			LinearAlgebra::Vector< float > x( { 4, 2, 3 } );
			LinearAlgebra::Vector< float > res( 3 );

			spPreconditioner->Apply( x, res );
			
			LinearAlgebra::Vector< float > expected( { 9, 14, 17 } );

			AssertAreEqual( 3, expected.Elements(), res.Elements() );
		}

		void TestSSOR( __in LinearAlgebra::Matrix< float >& A, __in LinearAlgebra::Vector< float >& x ) {
			LinearAlgebra::Vector< float > actualResult( x.GetDimension() );

			spPreconditioner->Initialize( A );
			spPreconditioner->Apply( x, actualResult );

			auto M = ( Diag( A ) + LowerTriangle( A ) ) * Invert( Diag( A ) ) * ( Diag( A ) + LowerTriangle( A ) ).Transpose();
			auto expectedResult = M * x;

			AssertAreEqual( x.GetDimension(), expectedResult.Elements(), actualResult.Elements(), 10e-4f );
		}

		TEST_METHOD( TestPreconditioner_3x3_0 )
		{
			Init();

			LinearAlgebra::Matrix< float > A( { { 1, 2, 3 }, { 2, 4, 5 }, { 3, 5, 6 } } );
			LinearAlgebra::Vector< float > x( { 1, 1, 1 } );

			TestSSOR( A, x );
		}

		TEST_METHOD( TestPreconditioner_3x3_1 )
		{
			Init();

			LinearAlgebra::Matrix< float > A( { {4,2,5}, {2,3,1}, {5,1,7} } );
			LinearAlgebra::Vector< float > x( { 1, 5, 8 } );

			TestSSOR( A, x );
		}

		TEST_METHOD( TestPreconditioner_4x4_0 )
		{
			Init();

			LinearAlgebra::Matrix< float > A( { { 8, 102, 1, 7 }, { 102, 14, 5, 31 }, { 1, 5, 23, 0 }, { 7, 31, 0, 9 } } );
			LinearAlgebra::Vector< float > x( { 4, 13, 8, 21 } );

			TestSSOR( A, x );
		}

	protected:

		std::shared_ptr< IPreconditioner< float > > spPreconditioner;

	};
}