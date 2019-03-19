#pragma once

namespace Utils { namespace FileSystem {

	const char* GetFilenameStart( __in_z const char* pPath );

	const char* GetFileExtension( __in_z const char* pPath );

	bool FileExists( __in_z const char* pPath );

} }