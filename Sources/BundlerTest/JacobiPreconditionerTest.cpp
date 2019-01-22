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

		TEST_METHOD( TestJacobi6x6_0 ) {
			Init();

			LinearAlgebra::Matrix<float> M( {
				{ -0.20147f, -0.20422f, 0.44640f, 0.95199f, -0.58429f, -0.71067f },
				{ -0.42966f, 0.03693f, 0.39886f, -0.65909f, -0.80962f, -0.62624f },
				{ -0.71290f, 0.29151f, 0.88934f, -0.06087f, 0.32349f, -0.09013f },
				{ 0.25960f, 0.27662f, -0.11375f, -0.08716f, -0.75011f, 0.07095f },
				{ -0.62184f, -0.14878f, 0.81946f, 0.63194f, -0.86569f, 0.85048f },
				{ 0.64738f, 0.84952f, -0.29961f, 0.50793f, -0.94794f, -0.64903f },
			} );
			LinearAlgebra::Vector<float> x( { 0.02297f, -0.28488f, 0.58424f, -0.57022f, 0.60740f, 0.70646f } );

			TestJacobi( M, x );
		}

		TEST_METHOD( TestJacobi15x15_0 ) {
			Init();

			LinearAlgebra::Matrix<float> M( {
				{ -0.19372f, -0.65561f, 0.07691f, -0.33818f, 0.27046f, -0.91034f, 0.59324f, 0.42169f, 0.98782f, -0.25706f, 0.07374f, -0.35824f, -0.92423f, 0.93280f, -0.58888f },
				{ 0.15852f, -0.79298f, -0.09088f, 0.52499f, -0.27564f, 0.85194f, -0.44759f, 0.03545f, 0.60382f, 0.42749f, 0.98578f, -0.30033f, 0.73020f, 0.83379f, 0.96584f },
				{ -0.59649f, 0.33147f, -0.50662f, -0.95672f, -0.04006f, -0.41111f, 0.68646f, 0.46644f, 0.77618f, -0.29185f, -0.56217f, 0.28412f, 0.17622f, 0.26975f, 0.46388f },
				{ -0.79266f, -0.92549f, -0.47571f, 0.99576f, 0.30886f, 0.36931f, 0.16837f, -0.60422f, -0.40549f, -0.73618f, 0.05850f, 0.31008f, -0.31294f, 0.58704f, -0.19128f },
				{ -0.45130f, -0.26937f, -0.80803f, -0.72510f, 0.29736f, -0.34520f, -0.67763f, 0.25546f, 0.01754f, 0.62004f, -0.92820f, -0.99727f, 0.49628f, -0.41395f, 0.19690f },
				{ 0.32142f, 0.08356f, 0.18310f, -0.28637f, -0.80226f, -0.70263f, 0.75244f, 0.71357f, 0.48557f, -0.01292f, 0.52165f, 0.72986f, 0.73606f, 0.30823f, 0.89874f },
				{ 0.26055f, 0.22550f, 0.50754f, 0.36757f, 0.51490f, 0.47340f, 0.17969f, 0.92609f, 0.07366f, 0.57395f, 0.17934f, 0.64450f, 0.80706f, -0.55004f, -0.36528f },
				{ -0.71697f, -0.52210f, -0.49764f, 0.88554f, -0.83674f, 0.95890f, -0.21157f, -0.12236f, -0.92572f, -0.15820f, -0.59010f, 0.11101f, 0.28835f, 0.43424f, 0.14289f },
				{ -0.56322f, 0.24630f, 0.67998f, -0.04699f, -0.65547f, -0.15725f, 0.06030f, 0.17672f, -0.88756f, -0.71098f, 0.50667f, -0.64446f, -0.67363f, 0.72285f, -0.52505f },
				{ -0.05902f, 0.38896f, 0.86083f, 0.26715f, -0.40249f, -0.40500f, 0.83779f, 0.08969f, 0.85208f, -0.49168f, 0.66963f, 0.56438f, 0.81937f, 0.42957f, 0.53574f },
				{ 0.93792f, -0.08440f, 0.86192f, 0.59398f, -0.92722f, -0.38640f, -0.34806f, -0.16108f, -0.24225f, -0.28581f, -0.43846f, 0.52232f, -0.77187f, 0.46183f, -0.94343f },
				{ 0.39814f, -0.65339f, -0.32518f, -0.81443f, 0.90740f, -0.43151f, 0.13805f, 0.09149f, -0.27165f, 0.26789f, -0.07172f, -0.13942f, -0.71288f, -0.08931f, -0.87798f },
				{ 0.11696f, -0.60209f, -0.50306f, -0.68335f, -0.86741f, 0.56222f, 0.71819f, 0.65413f, -0.60925f, 0.35558f, 0.99323f, -0.40249f, 0.24074f, 0.75066f, 0.03420f },
				{ 0.34579f, 0.32095f, -0.39605f, -0.16531f, -0.71849f, 0.61241f, -0.90206f, -0.60579f, -0.31441f, -0.93938f, -0.86847f, -0.34342f, -0.91632f, 0.93076f, -0.29333f },
				{ 0.23880f, 0.89177f, 0.21906f, 0.62764f, -0.78103f, -0.24333f, 0.97056f, 0.87367f, -0.73839f, 0.87634f, -0.37078f, 0.99685f, -0.62414f, -0.31179f, -0.61610f },
			} );
			LinearAlgebra::Vector<float> x( { -0.80435f, -0.99194f, -0.79799f, -0.94673f, 0.06679f, -0.37748f, -0.31553f, 0.85516f, 0.57400f, 0.46089f, 0.04404f, 0.60938f, -0.25878f, -0.32619f, -0.72513f } );

			TestJacobi( M, x );
		}

	protected:

		std::shared_ptr< IPreconditioner< float > > spPreconditioner;

	};

}