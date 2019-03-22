#include "stdafx.h"
#include "..\Bundler\BundlerLib.h"
#include "TestData.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Bundler;
using namespace Bundler::CameraModels;

namespace BundlerTest {
	
	using CamModel = CameraModel6DoF< 5 >;

	TEST_CLASS( HessianProviderTest )
	{
		void GetBundle( __in std::istream* pStream, __in std::istream* pImListStream, __out Bundle* pBundle )
		{
			Import::BundleImporter::Import( pStream, pImListStream, pBundle, NULL );
		}

		void InitializeProjectionProvider( __in Bundle* pBundle, __inout CamModel* pCameras, __out ProjectionProvider< CamModel >* pProj )
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

		uint GetTotalParameterCount( __in ProjectionProvider< CamModel >* pJacobian )
		{
			return ( uint )( pJacobian->GetCameraCount( ) * CamModel::cameraParameterCount + pJacobian->GetPointCount( ) * POINT_PARAM_COUNT );
		}

		uint GetTotalProjectionCount( __in ProjectionProvider< CamModel >* pJacobian )
		{
			const size_t camCount = pJacobian->GetCameraCount( );
			uint projCount = 0;

			for ( size_t i = 0; i < camCount; i++ )
			{
				projCount += (uint)pJacobian->GetCameraProjectionCount( i );
			}

			return projCount;
		}

		void PlaceBlocks(
			__in_ecount( 2 * CamModel::cameraParameterCount ) const Scalar* cameraBlock,
			__in_ecount( 2 * POINT_PARAM_COUNT ) const Scalar* pointBlock,
			__in const size_t camIx,
			__in const size_t pointIx,
			__in const size_t camCount,
			__in const size_t columns,
			__out Scalar* pJacobianRow )
		{
			Scalar* pCamRow1 = pJacobianRow + camIx * CamModel::cameraParameterCount;
			Scalar* pCamRow2 = pCamRow1 + columns;

			Scalar* pPtRow1 = pJacobianRow + camCount * CamModel::cameraParameterCount + pointIx * POINT_PARAM_COUNT;
			Scalar* pPtRow2 = pPtRow1 + columns;

			ShallowCopy< Scalar >( cameraBlock, CamModel::cameraParameterCount, pCamRow1 );
			ShallowCopy< Scalar >( cameraBlock + CamModel::cameraParameterCount, CamModel::cameraParameterCount, pCamRow2 );

			ShallowCopy< Scalar >( pointBlock, POINT_PARAM_COUNT, pPtRow1 );
			ShallowCopy< Scalar >( pointBlock + POINT_PARAM_COUNT, POINT_PARAM_COUNT, pPtRow2 );
		}

		void FillDenseHessian( __in ProjectionProvider< CamModel >* pJacobian, __out Matrix< Scalar >* pHessian )
		{
			const uint rows = ( uint )( 2 * GetTotalProjectionCount( pJacobian ) );
			const uint cols = ( uint )( GetTotalParameterCount( pJacobian ) );
			Matrix< Scalar > jacobianMatrix( rows, cols );

			Scalar tempCameraBlock[2 * CamModel::cameraParameterCount];
			Scalar tempPointBlock[2 * POINT_PARAM_COUNT];
			Scalar* pJ = jacobianMatrix.Elements( );

			const size_t camCount = pJacobian->GetCameraCount( );
			for ( size_t camIx = 0; camIx < camCount; camIx++ )
			{
				const size_t projCount = pJacobian->GetCameraProjectionCount( camIx );
				for ( size_t projI = 0; projI < projCount; projI++ )
				{
					const size_t projectionIx = pJacobian->GetCameraProjectionIndex( camIx, projI );
					const size_t pointIx = pJacobian->GetPointIndex( projectionIx );

					pJacobian->GetProjectionBlock< true, true, false >( projectionIx, tempCameraBlock, tempPointBlock, NULL );

					PlaceBlocks( tempCameraBlock, tempPointBlock, camIx, pointIx, camCount, cols, pJ );

					pJ += 2 * cols;
				}
			}
			
			MatrixMultiplyAtB( rows, cols, cols, jacobianMatrix.Elements( ), jacobianMatrix.Elements( ), pHessian->Elements( ) );
		}

		void FillCameraBlock(
			__in const size_t cameraIx,
			__in const size_t stride,
			__in_ecount( CamModel::cameraParameterCount * CamModel::cameraParameterCount ) const Scalar* pCamBlock,
			__out Scalar* pHessian )
		{
			Scalar* pDst = pHessian + ( cameraIx * stride * CamModel::cameraParameterCount ) + ( cameraIx * CamModel::cameraParameterCount );
			for ( size_t i = 0; i < CamModel::cameraParameterCount; i++ )
			{
				ShallowCopy< Scalar >( pCamBlock, CamModel::cameraParameterCount, pDst );
				pDst += stride;
				pCamBlock += CamModel::cameraParameterCount;
			}
		}

		void FillCameraPtBlockCam(
			__in const size_t cameraCount,
			__in const size_t cameraIx,
			__in const size_t pointIx,
			__in const size_t stride,
			__in_ecount( CamModel::cameraParameterCount * POINT_PARAM_COUNT ) const Scalar* pBlock,
			__out Scalar* pHessian )
		{
			Scalar* pDst = pHessian + ( cameraIx * stride * CamModel::cameraParameterCount ) + ( cameraCount * CamModel::cameraParameterCount ) + ( pointIx * POINT_PARAM_COUNT );
			for ( size_t i = 0; i < CamModel::cameraParameterCount; i++ )
			{
				ShallowCopy< Scalar >( pBlock, POINT_PARAM_COUNT, pDst );
				pDst += stride;
				pBlock += POINT_PARAM_COUNT;
			}
		}

		void FillCameraPtBlockPt(
			__in const size_t cameraCount,
			__in const size_t cameraIx,
			__in const size_t pointIx,
			__in const size_t stride,
			__in_ecount( CamModel::cameraParameterCount * POINT_PARAM_COUNT ) const Scalar* pBlock,
			__out Scalar* pHessian )
		{
			// we need to fill the block in a transposed-fashion

			Scalar* pDst = pHessian + ( pointIx * stride * POINT_PARAM_COUNT ) + ( cameraIx * CamModel::cameraParameterCount );

			for ( size_t i = 0; i < POINT_PARAM_COUNT; i++ )
			{
				for ( size_t j = 0; j < CamModel::cameraParameterCount; j++ )
				{
					ELEMENT( pDst, j ) = ELEMENT( pBlock, j * POINT_PARAM_COUNT + i );
				}
				pDst += stride;
			}
		}

		void FillPointBlock(
			__in const size_t cameraCount,
			__in const size_t pointIx,
			__in const size_t stride,
			__in_ecount( POINT_PARAM_COUNT * POINT_PARAM_COUNT ) const Scalar* pBlock,
			__out Scalar* pHessian )
		{
			Scalar* pDst = pHessian + ( pointIx * stride * POINT_PARAM_COUNT ) + ( cameraCount * CamModel::cameraParameterCount ) + ( pointIx * POINT_PARAM_COUNT );
			for ( size_t i = 0; i < POINT_PARAM_COUNT; i++ )
			{
				ShallowCopy< Scalar >( pBlock, POINT_PARAM_COUNT, pDst );
				pDst += stride;
				pBlock += POINT_PARAM_COUNT;
			}
		}

		void FillDenseHessian( __in HessianBlockProvider< CamModel >* pHessian, __out Matrix< Scalar >* pHessianMatrix )
		{
			Scalar camBlockTmp[CamModel::cameraParameterCount * CamModel::cameraParameterCount];
			Scalar camPtBlockTmp[CamModel::cameraParameterCount * POINT_PARAM_COUNT];
			Scalar pointBlockTmp[POINT_PARAM_COUNT * POINT_PARAM_COUNT];

			uint rows = 0, cols = 0;
			pHessianMatrix->GetDimensions( rows, cols );

			const size_t camCount = pHessian->GetCameraCount( );
			for ( size_t camIx = 0; camIx < camCount; camIx++ )
			{
				pHessian->GetCameraBlock( camIx, camBlockTmp );
				FillCameraBlock( camIx, cols, camBlockTmp, pHessianMatrix->Elements( ) );

				const size_t projCount = pHessian->GetCameraProjectionCount( camIx );
				for ( size_t projI = 0; projI < projCount; projI++ )
				{
					size_t pointIx = 0;
					pHessian->GetCameraPointBlockCam( camIx, projI, &pointIx, camPtBlockTmp );
					FillCameraPtBlockCam( camCount, camIx, pointIx, cols, camPtBlockTmp, pHessianMatrix->Elements( ) );
				}
			}

			Scalar* pPtHessian = pHessianMatrix->Elements( ) + ( camCount * cols * CamModel::cameraParameterCount );
			const size_t pointCount = pHessian->GetPointCount( );
			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ )
			{
				pHessian->GetPointBlock( pointIx, pointBlockTmp );
				FillPointBlock( camCount, pointIx, cols, pointBlockTmp, pPtHessian );

				const size_t projCount = pHessian->GetPointProjectionCount( pointIx );
				for ( size_t projI = 0; projI < projCount; projI++ )
				{
					size_t camIx = 0;
					pHessian->GetCameraPointBlockPt( pointIx, projI, &camIx, camPtBlockTmp );
					FillCameraPtBlockPt( camCount, camIx, pointIx, cols, camPtBlockTmp, pPtHessian );
				}
			}
		}

		void PrintHessian( __in const size_t n, __in_ecount( n * n ) const Scalar* H )
		{
			wchar* line = new wchar[n + 1];
			line[n] = 0;

			for ( size_t i = 0; i < n; i++ )
			{
				for ( size_t j = 0; j < n; j++ )
				{
					line[j] = L'x';// L'\u2591';
					if ( abs( ELEMENT( H, i * n + j ) ) < Scalar( 1e-5 ) )
					{
						line[j] = L'.'; // L'\u2593';
					}
				}
				Logger::WriteMessage( line );
			}
		}

		void PrintTime( __in_z const char* pMsg, __in const double elapsedMillis )
		{
			char messageBuffer[256];
			sprintf_s( messageBuffer, "\t%s: %fms", pMsg, elapsedMillis );

			Logger::WriteMessage( messageBuffer );
		}

		TEST_METHOD( CompareWithDense0 )
		{
			Logger::WriteMessage( "---- " __FUNCTION__ " Start ----" );

			HighResolutionClock stopwatch;

			std::istringstream inputStream( parseTestBundle );
			std::istringstream imListStream( parseTestImageList );
			Bundle bundle;
		
			GetBundle( &inputStream, &imListStream, &bundle );
		
			Containers::Buffer< CamModel > cameraModels;
			ProjectionProvider< CamModel > jacobian;
		
			cameraModels.Allocate( bundle.cameras.Length( ) );
			InitializeProjectionProvider( &bundle, cameraModels.Data( ), &jacobian );
		
		
			const size_t hessianSize = GetTotalParameterCount( &jacobian );
		
			Matrix< Scalar > denseHessian( (uint)hessianSize, (uint)hessianSize );
			Matrix< Scalar > denseHessian2 = denseHessian;
		
			stopwatch.Start( );
			FillDenseHessian( &jacobian, &denseHessian );
			stopwatch.Stop( );
			PrintTime( "Dense Hessian", stopwatch.GetTotalTime<TimeUnits::Milliseconds>( ) );

			// PrintHessian( hessianSize, denseHessian.Elements( ) );
			
		
			HessianBlockProvider< CamModel > hessian;
			hessian.Initialize( &jacobian );
			
			stopwatch.Clear( );
			stopwatch.Start( );
			FillDenseHessian( &hessian, &denseHessian2 );
			stopwatch.Stop( );
			PrintTime( "Sparse Hessian", stopwatch.GetTotalTime<TimeUnits::Milliseconds>( ) );

			// PrintHessian( hessianSize, denseHessian2.Elements( ) );
		
			AssertAreEqual( hessianSize * hessianSize, denseHessian.Elements( ), denseHessian2.Elements( ), Scalar(10e-4) );
		}

		
	};

}