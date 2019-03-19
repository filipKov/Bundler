#include "stdafx.h"

namespace Utils { namespace FileSystem {

	const char* GetFilenameStart( __in_z const char* pPath )
	{
		const char* pSlash1 = strrchr( pPath, '\\' );
		const char* pSlash2 = strrchr( pPath, '/' );
		return min( pSlash1, pSlash2 );
	}

	const char* GetFileExtension( __in_z const char* pPath )
	{
		return strrchr( pPath, '.' );
	}

	bool FileExists( __in_z const char* pPath )
	{
		return GetFileAttributesA( pPath ) != INVALID_FILE_ATTRIBUTES;
	}

} }