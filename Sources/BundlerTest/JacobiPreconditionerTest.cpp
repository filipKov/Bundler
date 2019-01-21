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
		

	protected:

		std::shared_ptr< IPreconditioner< float > > spPreconditioner;

	};

}