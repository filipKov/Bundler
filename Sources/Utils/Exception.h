#pragma once

#include <string>
#include <sstream>

namespace Utils {

	// probably do something with this
	namespace ExceptionReasons {

		static const char* DIMENSIONS_MISMATCH = "Dimensions do not match";
		
		static const char* DIVISION_BY_ZERO = "Division by zero";

		static const char* BAD_LOGARITHM_INPUT = "log' can't be calculated for x <= 0";

		static const char* BAD_SQRT_INPUT = "sqrt' can't be calculated for x < 0";

		static const char* BAD_CBRT_INPUT = "cbrt' can't be calculated for x == 0";

		static const char* NULLPTR = "Was nullptr";

		static const char* INVALID_FILE = "Invalid filename given";

	};

	class Exception {
	public:

		Exception() noexcept {};

		explicit Exception( __in_z const char8* pMessage ) {
			strcpy_s( mMessage, pMessage );
		}

		Exception( __in const Exception& src ) {
			strcpy_s( mMessage, src.mMessage );
		}

		virtual ~Exception() {};

		const char8* GetMessage() const {
			return mMessage;
		}

	protected:

		char8 mMessage[256];

	};

	class OutOfMemoryException : public Exception {
	public:

		OutOfMemoryException() : Exception( "Application ran out of memory" ) {};

		explicit OutOfMemoryException( __in const size_t byteCount ) {
			sprintf_s( mMessage, "Application failed to allocate %zu bytes", byteCount );
		};

	};

	class InvalidArgumentException : public Exception {
	public:

		InvalidArgumentException() : Exception( "Argument is invalid" ) {};

		explicit InvalidArgumentException( __in_z const char* pVariableName ) {
			sprintf_s( mMessage, "Argument [%s] is invalid", pVariableName );
		}

		explicit InvalidArgumentException( __in_z const char* pVariableName, __in_z const char* pReason ) {
			sprintf_s( mMessage, "Argument [%s] is invalid. Reason: %s", pVariableName, pReason );
		};

		template < typename T >
		explicit InvalidArgumentException( __in_z const char* pVariableName, __in const T* pVariable ) {
			sprintf_s( mMessage, "Argument[%s %s ( at %p ) ] is invalid", typeid( T ).name(), pVariableName, pVariable );
		}

		template < typename T >
		explicit InvalidArgumentException( __in_z const char* pVariableName, __in const T* pVariable, __in const char* pReason ) {
			sprintf_s( mMessage, "Argument[%s %s ( at %p ) ] is invalid. Reason: %s", typeid( T ).name(), pVariableName, pVariable, pReason );
		}

	};

	class IndexOutOfRangeException : public Exception {
	public:

		IndexOutOfRangeException() : Exception( "Index is out of range " ) {};

		explicit IndexOutOfRangeException( __in const size_t ixActual, __in const size_t ixMax ) {
			sprintf_s( mMessage, "Index %zu is out of range. Greatest valid index is %zu", ixActual, ixMax );
		}

		explicit IndexOutOfRangeException( __in const unsigned int ixActual, __in const unsigned int ixMax ) {
			sprintf_s( mMessage, "Index %u is out of range. Greatest valid index is %u", ixActual, ixMax );
		}

		explicit IndexOutOfRangeException( __in const int ixActual, __in const int ixMax ) {
			sprintf_s( mMessage, "Index %d is out of range. Greatest valid index is %d", ixActual, ixMax );
		}

		explicit IndexOutOfRangeException( __in const size_t ix1, __in const size_t ix2, __in const size_t ixMax1, __in const size_t ixMax2 ) {
			sprintf_s( mMessage, "Index [%zu, %zu] is out of range. Greatest valid index is [%zu, %zu]", ix1, ix2, ixMax1, ixMax2 );
		}

		explicit IndexOutOfRangeException( __in const unsigned int ix1, __in const unsigned int ix2, __in const unsigned int ixMax1, __in const unsigned int ixMax2 ) {
			sprintf_s( mMessage, "Index [%u, %u] is out of range. Greatest valid index is [%u, %u]", ix1, ix2, ixMax1, ixMax2 );
		}

		explicit IndexOutOfRangeException( __in const int ix1, __in const int ix2, __in const int ixMax1, __in const int ixMax2 ) {
			sprintf_s( mMessage, "Index [%d, %d] is out of range. Greatest valid index is [%d, %d]", ix1, ix2, ixMax1, ixMax2 );
		}

	};

	class WrongStateException : public Exception {

	public:

		WrongStateException() : Exception( "Object is in wrong state" ) {};

		explicit WrongStateException( __in const char* pVarString ) {
			sprintf_s( mMessage, "%s is in wrong state", pVarString );
		}

		explicit WrongStateException( __in const char* pVarString, __in const char* pReasonStr ) {
			sprintf_s( mMessage, "%s is in wrong state. Reason: %s", pVarString, pReasonStr );
		}

		template < typename T >
		explicit WrongStateException( __in T* pVar, __in const char* pVarString ) {
			sprintf_s( mMessage, "%s %s is in wrong state", typeid( T ).name(), pVarString );
			UNREFERENCED_PARAMETER( pVar );
		}

	};

}