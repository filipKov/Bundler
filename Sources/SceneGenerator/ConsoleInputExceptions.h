#pragma once

class InvalidConsoleArgumentException : public Exception {
public:
	
	InvalidConsoleArgumentException() : Exception( "Invalid console argument" ) {};

	InvalidConsoleArgumentException( __in const size_t i ) {
		sprintf_s( mMessage, "Console argument %zu is invalid", i );
	}

	InvalidConsoleArgumentException( __in const size_t i, __in const char* pArg ) {
		sprintf_s( mMessage, "Console argument \"%s\"(%zu) is invalid", pArg, i );
	}

};

class UnknownOptionException : public Exception {
public:

	UnknownOptionException( __in const char* pOption ) {
		sprintf_s( mMessage, "Unknown option \"%s\"", pOption );
	}
};

class InvalidConsoleInputException : public Exception {
	using Exception::Exception;
};
