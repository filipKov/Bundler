#pragma once

namespace LinearAlgebra {

	class InvalidDimensionsException : public Utils::Exception {
	public:

		InvalidDimensionsException() {
			sprintf_s( mMessage, "Matrix has invalid dimensions" );
		}

		explicit InvalidDimensionsException( __in const size_t actualX, __in const size_t actualY, __in const size_t expectedX, __in const size_t expectedY ) {
			sprintf_s( mMessage, "Dimensions [%zu, %zu] are invalid, expected [%zu, %zu]", actualX, actualY, expectedX, expectedY );
		}

		explicit InvalidDimensionsException( __in_z const char* pVarString, __in const size_t actualX, __in const size_t actualY, __in const size_t expectedX, __in const size_t expectedY ) {
			sprintf_s( mMessage, "%s has invalid dimensions [%zu, %zu], expected [%zu, %zu]", pVarString, actualX, actualY, expectedX, expectedY );
		}

		explicit InvalidDimensionsException( __in_z const char* pVarString ) {
			sprintf_s( mMessage, "%s has invalid dimensions", pVarString );
		};
	};

}