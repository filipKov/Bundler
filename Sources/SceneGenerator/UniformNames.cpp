#include "stdafx.h"

#include "UniformNames.h"

const char* GetUniformArrayName( char* pBuffer, const char* pTemplate, uint ix ) {
	sprintf_s( pBuffer, MAX_UNIFORM_NAME_LENGTH + 1, pTemplate, ix );
	return pBuffer;
}