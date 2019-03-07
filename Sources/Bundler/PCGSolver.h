#pragma once

namespace Bundler { namespace LinearSolver {

	struct PCGSolverStatistics
	{
		uint iterationCount;
		Scalar finalError;
	};

	namespace Internal {

		class PCGSolverTemp
		{
		public:

			Vector< Scalar > r;
			Vector< Scalar > d;
			Vector< Scalar > Ad;
			Vector< Scalar > MInvR;

			Scalar errSq;

			PCGSolverTemp( __in const uint paramCount ) :
				r( paramCount ),
				d( paramCount ),
				Ad( paramCount ),
				MInvR( paramCount )
			{
				errSq = Scalar( DBL_MAX );
			}
		};

		template < class CameraModel >
		class PcgHessianCameraBlock 
		{
		public:
			
			Containers::Buffer< size_t > pointIndices;
			Containers::Buffer< HessianCameraPointBlock< CameraModel > > cameraPointBlocks;

			HessianCameraBlock< CameraModel > cameraBlock;

			PcgHessianCameraBlock( __in const size_t cameraPointBlockCount )
			{
				pointIndices.Allocate( cameraPointBlockCount );
				cameraPointBlocks.Allocate( cameraPointBlockCount );
			}
		};

	}

	class PCGSolver 
	{
	public:
		
		void Initialize(
			__in const uint maxIterations,
			__in const Scalar errorTolerance ) 
		{
			m_maxIterations = maxIterations;
			m_errorToleranceSq = errorTolerance * errorTolerance;
		}

		template < class CameraModel >
		void SolveSystem(
			__in const IJacobianProvider< CameraModel >* pJacobian,
			__in const uint parameterVectorSize,
			__inout_ecount( parameterVectorSize ) Scalar* pX,
			__out_opt PCGSolverStatistics* pStatistics ) const
		{
			PCGSolverTemp temp( parameterVectorSize );
			Vector< Scalar > x( parameterVectorSize, pX );
			BasicHessianProvider< CameraModel > hessian;
			hessian.Initialize( pJacobian );
			
			InitSolve( pJacobian, &hessian, &x, &temp );
			uint finalIter = SolveLoop( pJacobian, &hessian, &x, &tmep );

			if ( pStatistics )
			{
				pStatistics->iterationCount = finalIter;
				pStatistics->finalError = sqrt( temp.errSq );
			}
		}

		template < class CameraModel >
		void InitSolve(
			__in const IJacobianProvider< CameraModel >* pJacobian,
			__in const IHessianProvider< CameraModel >* pHessian,
			__in const Vector< Scalar >* pX,
			__out Internal::PCGSolverTemp* pTemp )
		{
			GetInitialResidual( pHessian, pX->Elements(), pTemp->r.Elements() );
			// ApplyPreconditioner( pTemp->r, pTemp->d );
			pTemp->errSq = pTemp->r.Dot( pTemp->d );
		}

		template < class CameraModel >
		uint SolveLoop(
			__in const IJacobianProvider< CameraModel >* pJacobian,
			__in const IHessianProvider< CameraModel >* pHessian,
			__inout Vector< Scalar >* pX,
			__inout Internal::PCGSolverTemp* pTemp ) const
		{
			uint iteration = 0;
			while ( ( iteration < m_maxIterations ) && 
					( pTemp->m_errSq > m_errorToleranceSq ) )
			{
				// temp.Ad = A * d; -> from HessianProvider

				Scalar alpha = pTemp->errSq / ( pTemp->d.Dot( pTemp->Ad ) );
				(*pX) += alpha * pTemp->d;
				pTemp->r -= alpha * pTemp->Ad; // maybe do "r = b - Ax" every 50th iter or so

				// temp.MInvR = M * temp.r; -> from HessianProvider/Preconditioner

				Scalar errSqOld = pTemp->errSq;
				pTemp->errSq = pTemp->r.Dot( pTemp->MInvR );
				Scalar beta = temp.errSq / errSqOld;
				pTemp->d *= beta;
				pTemp->d += pTemp->MInvR;

				iteration++;
			}

			return iteration;
		}

	protected:

		template < class CameraModel >
		void GetInitialResidual(
			__in const IHessianProvider< CameraModel >* pHessian,
			__in const size_t totalParamCount,
			__in_ecount( totalParamCount ) const Scalar* pX,
			__out_ecount( totalParamCount ) Scalar* pResidual )
		{
			const size_t totalCameraParamCount = pHessian->GetCameraCount() * CameraModel::cameraParameterCount;
			constexpr const size_t cameraParamCount = CameraModel::cameraParameterCount;

			for ( size_t cameraIx = 0; cameraIx < pHessian->GetCameraCount(); cameraIx++ ) {
				GetInitialResidualCameraBlock( cameraIx, totalParamCount, pX, pResidual );
			}

			for ( size_t pointIx = 0; pointIx < pHessian->GetPointCount(); pointIx++ ) {
				GetInitialResidualPointBlock( pointIx, totalParamCount, pX, pResidual );
			}
		}

		template < class CameraModel >
		void GetInitialResidualCameraBlock(
			__in const size_t cameraIx,
			__in const IHessianProvider< CameraModel >* pHessian,
			__in const size_t totalParamCount,
			__in_ecount( totalParamCount ) const Scalar* pX,
			__out_ecount( totalParamCount ) Scalar* pResidual )
		{
			constexpr const size_t cameraParamCount = CameraModel::cameraParameterCount;
			size_t cameraPointBlockCount = pHessian->GetCameraPointBlockCount( cameraIx );

			Internal::PcgHessianCameraBlock< CameraModel > cameraBlock( cameraIx, cameraPointBlockCount, pResidual );

			Vector< Scalar, cameraParamCount > residual;

			pHessian->GetCameraBlocks(
				cameraIx, &cameraBlock.cameraBlock, cameraPointBlockCount,
				cameraBlock.pointIndices.Data(), cameraBlock.cameraPointBlocks.Data(), 
				residual.Elements() );

			Vector< Scalar, cameraParamCount > blockAccumulator;
			AccumulateResidualCameraBlock< CameraModel >(
				pHessian->GetCameraCount(), cameraIx, totalParamCount, pX,
				&cameraBlock, blockAccumulator.Elements() );

			AccumulateResidualPointBlocks< CameraModel >(
				cameraIx, pHessian->GetCameraCount(), cameraBlock.pointIndices.Length(),
				cameraBlock.pointIndices.Data(), cameraBlock.cameraPointBlocks.Data(), 
				totalParamCount, pX, pResidual );

			residual -= blockAccumulator;
			ShallowCopy< Scalar >( residual.Elements, cameraParamCount, pResidual + cameraIx * cameraParamCount );
		}

		template < class CameraModel >
		void AccumulateResidualCameraBlock(
			__in const size_t cameraCount,
			__in const size_t cameraIx,
			__in const size_t totalParamCount,
			__in_ecount( totalParamCount ) const Scalar* pX,
			__in const Internal::PcgHessianCameraBlock< CameraModel >* pCameraBlock,
			__out_ecount( CameraModel::cameraParameterCount ) Scalar* pAccumulator )
		{
			constexpr const size_t cameraParamCount = CameraModel::cameraParameterCount;

			Vector< Scalar, cameraParamCount > accumulatorTmp;

			MatrixMultiply< Scalar, cameraParamCount, cameraParamCount, 1 >(
				pCameraBlock->cameraBlock.Elements(),
				pX + cameraIx * cameraParamCount,
				accumulatorTmp.Elements() );

			Vector< Scalar, cameraParamCount > camPtAccumulator;
			for ( uint cameraPointBlockIx = 0; cameraPointBlockIx < (uint)pCameraBlock->pointIndices.Length(); cameraPointBlockIx++ )
			{
				MatrixMultiply< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >(
					pCameraBlock->cameraPointBlocks[ cameraPointBlockIx ].Elements(),
					pX + cameraCount * cameraParamCount + pCameraBlock->pointIndices[ cameraPointBlockIx ] * POINT_PARAM_COUNT,
					camPtAccumulator.Elements() );
			
				cameraAccumulator += camPtAccumulator;
			}

			ShallowCopy< Scalar >( accumulatorTmp.Elements(), cameraParamCount, pAccumulator );
		}

		template < class CameraModel >
		void AccumulateResidualPointBlocks(
			__in const size_t cameraIx,
			__in const size_t cameraCount,
			__in const size_t pointCount,
			__in_ecount( pointCount ) const size_t* pPointIndices,
			__in_ecount( pointCount ) const HessianCameraPointBlock< CameraModel >* pCameraPointBlocks,
			__in const size_t totalParameterCount,
			__in_ecount( totalParameterCount ) const Scalar* pX,
			__out_ecount( totalParameterCount ) Scalar* pResidual )
		{
			constexpr const uint cameraParamCount = CameraModel::cameraParameterCount;
			const Scalar* pXSource = pX + cameraIx * cameraParamCount;

			Vector< Scalar, POINT_PARAM_COUNT > tempAccumulator;

			for ( size_t pointIx = 0; pointIx < pointCount; pointIx++ ) 
			{
				MatrixMultiplyAtB< Scalar, cameraParamCount, POINT_PARAM_COUNT, 1 >(
					ELEMENT( pCameraPointBlocks, pointIx ).Elements(),
					pXSource,
					tempAccumulator.Elements() );

				Scalar* pResidualDst = pResidual + cameraCount * cameraParamCount + ELEMENT( pPointIndices, pointIx ) * POINT_PARAM_COUNT;
				for ( uint i = 0; i < POINT_PARAM_COUNT; i++ )
				{
					ELEMENT( pResidualDst, i ) += tempAccumulator[ i ];
				}
			}
		}

		template < class CameraModel >
		void GetInitialResidualPointBlock(
			__in const size_t pointIx,
			__in const IHessianProvider< CameraModel >* pHessian,
			__in const size_t totalParamCount,
			__in_ecount( totalParamCount ) const Scalar* pX,
			__out_ecount( totalParamCount ) Scalar* pResidual )
		{
			const size_t totalCameraParameters = pHessian->GetCameraCount() * CameraModel::cameraParameterCount;

			HessianPointBlock pointBlock;
			Vector< Scalar, POINT_PARAM_COUNT > jtfBlock;

			pHessian->GetPointBlock( pointIx, &pointBlock.Elements(), jtfBlock.Elements() );


			Vector< Scalar, POINT_PARAM_COUNT > accumulator;
			MatrixMultiply< Scalar, POINT_PARAM_COUNT, POINT_PARAM_COUNT, 1 >(
				pointBlock.Elements(),
				pX + totalCameraParameters + pointIx * POINT_PARAM_COUNT,
				accumulator.Elements() );

			accumulator -= jtfBlock;
			Scalar* pBlockDst = pResidual + totalCameraParameters + pointIx * POINT_PARAM_COUNT;

			for ( uint i = 0; i < POINT_PARAM_COUNT; i++ ) 
			{
				ELEMENT( pBlockDst, i ) += accumulator[ i ];
			}
		}

	protected:
		
		uint m_maxIterations;
		Scalar m_errorToleranceSq;

	};

} }