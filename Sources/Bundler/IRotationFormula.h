#pragma once


namespace Bundler { namespace CameraModels {

	template < uint rotationParameterCount, uint totalParameterCount >
	class IRotationFormula {
	public:

		constexpr uint GetParameterCount() const {
			return rotationParameterCount;
		}

		// virtual void Initialize( __in_ecount( 9 ) const Scalar* pRotationMatrix ) = 0;

		virtual void Initialize( __in_ecount( rotationParameterCount ) const Scalar* pParams ) = 0;

		virtual void RotatePoint(
			__in_ecount( 3 ) DScalar< totalParameterCount >* pInputPoint,
			__out_ecount( 3 ) DScalar< totalParameterCount >* pOutputPoint ) const = 0;

	};

} }