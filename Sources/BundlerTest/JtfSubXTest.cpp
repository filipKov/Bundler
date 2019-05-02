#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include "TestData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {
	
	using CameraModel = CameraModels::CameraModel6DoF< 10 >;

	void JtfNegSubtractX_Local_Cams(
		__in const uint localCameraIx,
		__in const LocalProjectionProviderCPU< CameraModel >* pJacobian,
		__out_ecount( CameraModel::cameraPrameterCount ) Scalar* pDst )
	{
		Scalar camTemp[2 * CameraModel::cameraParameterCount];
		Scalar residualTemp[2];
	
		Scalar camXTemp[CameraModel::cameraParameterCount];
	
		const uint projectionCount = pJacobian->GetCameraProjectionCount( localCameraIx );
		for ( uint projI = 0; projI < projectionCount; projI++ )
		{
			const uint projectionIx = pJacobian->GetCameraProjectionIndex( localCameraIx, projI );
	
			pJacobian->GetProjectionBlock< true, false, true >( projectionIx, camTemp, NULL, residualTemp );
	
			MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, 1 >( camTemp, residualTemp, camXTemp );
			MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( camXTemp, pDst, pDst );
		}
	
		MatrixNegate< Scalar, CameraModel::cameraParameterCount, 1 >( pDst, pDst );
	}

	void JtfNegSubtractX_Local_Points(
		__in const uint localPointIx,
		__in const LocalProjectionProviderCPU< CameraModel >* pJacobian,
		__out_ecount( POINT_PARAM_COUNT ) Scalar* pDst )
	{
		Scalar pointTemp[2 * POINT_PARAM_COUNT];
		Scalar residualTemp[2];

		Scalar pointXTemp[POINT_PARAM_COUNT];

		const uint projectionCount = pJacobian->GetPointProjectionCount( localPointIx );
		for ( uint projI = 0; projI < projectionCount; projI++ )
		{
			const uint projectionIx = pJacobian->GetPointProjectionIndex( localPointIx, projI );

			pJacobian->GetProjectionBlock< false, true, true >( projectionIx, NULL, pointTemp, residualTemp );

			MatrixMultiplyAtB< Scalar, 2, POINT_PARAM_COUNT, 1 >( pointTemp, residualTemp, pointXTemp );
			MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pointXTemp, pDst, pDst );
		}

		MatrixNegate< Scalar, POINT_PARAM_COUNT, 1 >( pDst, pDst );
	}

	void JtfNegSubtractX_Global(
		__in const ProjectionProvider< CameraModel >* pJacobian,
		__in const uint vectorSize,
		__inout_ecount( vectorSize ) Scalar* pX )
	{
		const size_t cameraCount = pJacobian->GetCameraCount( );

		Scalar camTemp[2 * CameraModel::cameraParameterCount];
		Scalar pointTemp[2 * POINT_PARAM_COUNT];
		Scalar residualTemp[2];

		Scalar camXTemp[CameraModel::cameraParameterCount];
		Scalar pointXTemp[POINT_PARAM_COUNT];

		for ( size_t cameraIx = 0; cameraIx < cameraCount; cameraIx++ )
		{
			const size_t projectionCount = pJacobian->GetCameraProjectionCount( cameraIx );
			for ( size_t projI = 0; projI < projectionCount; projI++ )
			{
				const size_t projectionIx = pJacobian->GetCameraProjectionIndex( cameraIx, projI );
				const size_t pointIx = pJacobian->GetPointIndex( projectionIx );

				pJacobian->GetProjectionBlock< true, true, true >( projectionIx, camTemp, pointTemp, residualTemp );

				MatrixMultiplyAtB< Scalar, 2, CameraModel::cameraParameterCount, 1 >( camTemp, residualTemp, camXTemp );
				MatrixMultiplyAtB< Scalar, 2, POINT_PARAM_COUNT, 1 >( pointTemp, residualTemp, pointXTemp );

				Scalar* pCamXDestination = Bundler::Utils::GetCameraParamPtr< CameraModel >( cameraIx, pX );
				Scalar* pPtXDestination = Bundler::Utils::GetPointParamPtr< CameraModel >( pointIx, cameraCount, pX );

				MatrixAdd< Scalar, CameraModel::cameraParameterCount, 1 >( camXTemp, pCamXDestination, pCamXDestination );
				MatrixAdd< Scalar, POINT_PARAM_COUNT, 1 >( pointXTemp, pPtXDestination, pPtXDestination );
			}
		}

		Vector< Scalar > x( vectorSize, pX );
		x.Negate( );
	}

	TEST_CLASS( JtfSubXTest )
	{
		void GetTestBundle( __out Bundle* pBundle, __out_opt BundleAdditionalPayload* pMetadata )
		{
			// HRESULT hr = Import::BundleImporter::Import( "Resources/gargoyle.out", pBundle, pMetadata );
			HRESULT hr = E_FAIL;
			if ( FAILED( hr ) )
			{
				std::istringstream bundleStream( cubesMaskedBundle );
				std::istringstream imListStream( cubesMaskedImageList );

				hr = Import::BundleImporter::Import( &bundleStream, &imListStream, pBundle, pMetadata );
			}

			Assert::IsTrue( SUCCEEDED( hr ) );
		}

		void GetGlobalProjectionProvider( __in Bundle* pBundle, __inout CameraModel* pCameras, __out ProjectionProvider< CameraModel >* pProj )
		{
			for ( size_t i = 0; i < pBundle->cameras.Length( ); i++ )
			{
				pCameras[i].Initialize( &pBundle->cameras[i] );
			}

			pProj->Initialize(
				pBundle->cameras.Length( ),
				pCameras,
				pBundle->points.Length( ),
				pBundle->points.Data( ),
				pBundle->projections.Length( ),
				pBundle->projections.Data( )
			);
		}

		uint GetTotalParameterCount( __in ProjectionProvider< CameraModel >* pJacobian )
		{
			return ( uint )( pJacobian->GetCameraCount( ) * CameraModel::cameraParameterCount + pJacobian->GetPointCount( ) * POINT_PARAM_COUNT );
		}

		TEST_METHOD( Correctness0 )
		{
			Bundle bundle;
			BundleAdditionalPayload metadata;

			GetTestBundle( &bundle, &metadata );

			Containers::Buffer< CameraModel > cameras;
			cameras.Allocate( bundle.cameras.Length( ) );

			ProjectionProvider< CameraModel > globalJacobian;
			GetGlobalProjectionProvider( &bundle, cameras.Data( ), &globalJacobian );

			const uint totalParamCount = GetTotalParameterCount( &globalJacobian );
			Vector< Scalar > x( totalParamCount );
			Random< Scalar >::Fill( Scalar( -0.5 ), Scalar( 0.5 ), totalParamCount, x.Elements( ) );

			Vector< Scalar > expected = x;
			JtfNegSubtractX_Global( &globalJacobian, totalParamCount, expected.Elements( ) );

			LocalProjectionProviderCPU< CameraModel > localJacobian;
			localJacobian.InitializeForCameras( &globalJacobian, 0, (uint)globalJacobian.GetCameraCount( ) );

			Scalar* pDst = x.Elements( );
			for ( uint camIx = 0; camIx < localJacobian.GetCameraCount( ); camIx++ )
			{
				JtfNegSubtractX_Local_Cams( camIx, &localJacobian, pDst );
				pDst += CameraModel::cameraParameterCount;
			}

			localJacobian.InitializeForPoints( &globalJacobian, 0, ( uint )globalJacobian.GetPointCount( ) );
			for ( uint ptIx = 0; ptIx < localJacobian.GetPointCount( ); ptIx++ )
			{
				JtfNegSubtractX_Local_Points( ptIx, &localJacobian, pDst );
				pDst += POINT_PARAM_COUNT;
			}

			AssertAreEqual( totalParamCount, expected.Elements( ), x.Elements( ), Scalar( 10e-6 ) );
		}

	};

}
