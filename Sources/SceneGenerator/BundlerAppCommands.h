#pragma once

template < size_t sz >
bool StringsEqual( __in_z const char( &str1 )[ sz ], __in_z const char* str2 ) {
	return strncmp( str1, str2, sz ) == 0;
}

bool IsDirectory( __in const char* pPath ) {
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if ( GetFileAttributesExA( pPath, GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &fileInfo ) ) {
		if ( fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			return true;
		}
	}

	return false;
}

bool IsBundleFile( __in const char* pPath ) {
	WIN32_FILE_ATTRIBUTE_DATA fileInfo;
	if ( GetFileAttributesExA( pPath, GET_FILEEX_INFO_LEVELS::GetFileExInfoStandard, &fileInfo ) ) {
		return StringsEqual( ".out", FileSystem::GetFileExtension( pPath ) );
	}

	return false;
}

static const ApplicationCommand setInputCommand{
	[]( const int argCount, char** ppArgs, int* pArgI ) -> void
	{
		( *pArgI )++;
		if ( *pArgI >= argCount ) {
			throw InvalidConsoleInputException( "Too few arguments, path expected" );
		}
	
		const char* pPath = ppArgs[ *pArgI ];

		if ( !IsBundleFile( pPath ) ) {
			throw InvalidConsoleInputException( "Invalid input(s) provided" );
		}
	
		BundlerApp::GetApp()->GetContext().SetInput( pPath );
	},

	[] ( const bool showShort ) -> void
	{
		if ( showShort ) {
			printf_s( " -i InputFile.out" );
		}
		else {
			printf_s( "\t-i inp, --input inp:\n" );
			printf_s( "\t\tSet the input path for the App\n" );
			printf_s( "\t\t\"inp\" should be a Bundle file with the .out extension\n" );
		}
	},

	2, 
	{ "i", "input" }
};

static const ApplicationCommand setFilterCommand {
	[ ]( const int argCount, char** ppArgs, int* pArgI ) -> void
	{
		( *pArgI )++;
		if ( *pArgI >= argCount )
		{
			throw InvalidConsoleInputException( "Too few arguments, strength expected" );
		}
	
		float strength = ( float )atof( ppArgs[*pArgI] );
	
		BundlerApp::GetApp( )->GetContext( ).appStages |= AppStages::FILTER;
		BundlerApp::GetApp( )->GetContext( ).filteringStrength = strength;
	},
	
	[ ] ( const bool showShort ) -> void
	{
		if ( showShort )
		{
			printf_s( " [-f strength]" );
		}
		else
		{
			printf_s( "\t-f str, --filter str:\n" );
			printf_s( "\t\tSet the input filtering strength\n" );
			printf_s( "\t\tShould be a floating point number\n" );
		}
	},
	
	2,
	{ "f", "filter" }
};

static const ApplicationCommand setNoiseCommand {
	[ ]( const int argCount, char** ppArgs, int* pArgI ) -> void
	{
		( *pArgI )++;
		if ( *pArgI >= argCount )
		{
			throw InvalidConsoleInputException( "Too few arguments, strength expected" );
		}
	
		float strength = ( float )atof( ppArgs[*pArgI] );
		BundlerApp::GetApp( )->GetContext( ).pointNoise = strength;

		( *pArgI )++;
		if ( *pArgI >= argCount )
		{
			throw InvalidConsoleInputException( "Too few arguments, strength expected" );
		}

		strength = ( float )atof( ppArgs[*pArgI] );
		BundlerApp::GetApp( )->GetContext( ).cameraNoise = strength;


		BundlerApp::GetApp( )->GetContext( ).appStages |= AppStages::NOISE;
	},
	
	[ ] ( const bool showShort ) -> void
	{
		if ( showShort )
		{
			printf_s( " [-n ptStrength camStrength]" );
		}
		else
		{
			printf_s( "\t-n strP strC, --noise strP strC:\n" );
			printf_s( "\t\tSet the noise strength(must be a number, shoudl be fairly small)\n" );
			printf_s( "\t\tstrP: noise for points\n" );
			printf_s( "\t\tstrC: noise for camera parameters\n" );
		}
	},
	
	2,
	{ "n", "noise" }
};

static const ApplicationCommand setOptimizeCommand {
	[ ]( const int argCount, char** ppArgs, int* pArgI ) -> void
	{
		( *pArgI )++;
		if ( *pArgI >= argCount )
		{
			throw InvalidConsoleInputException( "Too few arguments, type expected" );
		}
	
		const char* typeStr = ppArgs[*pArgI];
		BundlerType type = BundlerType::UNKNOWN;

		if ( StringsEqual( "seqcpu", typeStr ) )
		{
			type = BundlerType::SEQ_CPU;
		}
		else if ( StringsEqual( "parcpu", typeStr ) )
		{
			type = BundlerType::PARALLEL_CPU;
		}
		else if ( StringsEqual( "cpugpu", typeStr ) )
		{
			type = BundlerType::COMBINED;
		}
		else
		{
			throw InvalidConsoleInputException( "Invalid type provided" );
		}
	
		BundlerApp::GetApp( )->GetContext( ).bundlerType = type;
		BundlerApp::GetApp( )->GetContext( ).appStages |= AppStages::OPTIMIZE;
	},
	
	[ ] ( const bool showShort ) -> void
	{
		if ( showShort )
		{
			printf_s( " [-o seqcpu|parcpu|cpugpu]" );
		}
		else
		{
			printf_s( "\t-o type, --optimize type:\n" );
			printf_s( "\t\tSet the version of the Bundler\n" );
			printf_s( "\t\tseqcpu: sequential CPU\n" );
			printf_s( "\t\tparcpu: parallel CPU\n" );
			printf_s( "\t\tcpugpu: combined CPU-GPU\n" );
		}
	},
	
	2,
	{ "o", "optimize" }
};


// static const ArgumentCallback showHelpCallback{
// 	[]( const int argCount, char** ppArgs, int* pArgI ) -> void
// 	{
// 		printf( "Usage: \n" );
// 		printf( "\tCliExample.exe -i ImagesFolder -o OutputFolder -n Name [-p TempFolder] [-d preview|normal|high] [-e|l|f|s|c|t|k]\n" );
// 		printf( "\n" );
// 		printf( "Options:\n" );
// 		printf( "\t-i ImagesFolder \t Full path to the folder that contains images to be processed.\n" );
// 		printf( "\t-o OutputFolder \t Full path to the folder where results will be saved.\n" );
// 		printf( "\t-n Name \t\t The name will be used as a prefix to result names. Must be one word.\n" );
// 		printf( "\t-p TempFolder \t Optional parameter for full path to the temp folder .\n" );
// 		printf( "\t-e \t\t\t Save project (component and model) to output folder.\n" );
// 		printf( "\t-l \t\t\t Load project (component and model) from output folder.\n" );
// 		printf( "\t-d \t\t\t Compute model in certain detail. Normal is default.\n" );
// 		printf( "\t-f \t\t\t Perform filtering.\n" );
// 		printf( "\t-s \t\t\t Simplify to 10%% of the model triangles count.\n" );
// 		printf( "\t-c \t\t\t Colorize vertices of the model.\n" );
// 		printf( "\t-t \t\t\t Texture model.\n" );
// 		printf( "\t-k \t\t\t Calculate ortho projection and export to KMZ.\n" );
// 		printf( "\n" );
// 	},
// 	"-h OR --help"
// };
