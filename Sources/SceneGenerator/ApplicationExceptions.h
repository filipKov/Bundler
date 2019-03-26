#pragma once

class ExceptionFromHresult : public Utils::Exception {
public:

	ExceptionFromHresult( __in HRESULT hr ) {
		sprintf_s( mMessage, "Processing exited with HRESULT code 0x%x", hr );
	}

	ExceptionFromHresult() = delete;

};