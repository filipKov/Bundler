#include "stdafx.h"

namespace Utils { namespace FileSystem {

	const char* GetFilenameStart( __in_z const char* pPath )
	{
		const char* pSlash1 = strrchr( pPath, '\\' );
		const char* pSlash2 = strrchr( pPath, '/' );
		
		if ( !pSlash1 )
		{
			if ( !pSlash2 )
			{
				return pPath;
			}
			return pSlash2 + 1;
		}
		else
		{
			if ( !pSlash2 )
			{
				return pSlash1 + 1;
			}
			return min( pSlash1, pSlash2 ) + 1;
		}
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