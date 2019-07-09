#include "stdafx.h"
#include "ApplicationContext.h"
#include "ConsoleInputExceptions.h"
#include "ConsoleArgumentParser.h"

ConsoleArgumentParser::ConsoleArgumentParser()
{
}


ConsoleArgumentParser::~ConsoleArgumentParser()
{
}

void ConsoleArgumentParser::RegisterCommand( __in const ApplicationCommand* pCommand ) 
{
	mCommands.Add( pCommand );
}

void ConsoleArgumentParser::ParseArgs( __in const int argCount, __in char** ppArgs ) 
{	
	for ( int i = 1; i < argCount; i++ ) {
		const char* pArgument = ppArgs[i];

		if ( *pArgument == '-' ) {
			pArgument++;

			if ( *pArgument == '-' ) {
				ParseArgument( ++pArgument, argCount, ppArgs, &i );
			}
			else {
				ParseShortArgument( pArgument, argCount, ppArgs, &i );
			}
		}
		else
		{
			ShowHelp( );
			throw InvalidConsoleArgumentException( i, pArgument );
		}
	}
}

void ConsoleArgumentParser::ParseArgument( __in_z const char* pArg, __in const int argCount, __in_ecount( argCount ) char** ppArgs, __inout int* pArgI )
{
	const ApplicationCommand* pCommand = NULL;
	if ( FindCommand( pArg, &pCommand ) ) {
		try {
			pCommand->callbackFnc( argCount, ppArgs, pArgI );
		}
		catch ( InvalidConsoleInputException err ) {
			printf_s( "Error: %s\n", err.GetMessage() );
			printf_s( "\tUsage: BundlerApp" );
			pCommand->helpFnc( true );
			throw Exception();
		}
	}
	else {
		ShowHelp();
		throw Exception();
	}
}

void ConsoleArgumentParser::ParseShortArgument( __in_z const char* pArg, __in const int argCount, __in_ecount( argCount ) char** ppArgs, __inout int* pArgI )
{
	if ( pArg[1] != 0 ) {
		throw InvalidConsoleArgumentException( *pArgI, ppArgs[*pArgI] );
	}

	ParseArgument( pArg, argCount, ppArgs, pArgI );
}

bool ConsoleArgumentParser::FindCommand( __in_z const char* consoleArgument, __deref_out const ApplicationCommand** ppCommand )
{
	for ( size_t cmdI = 0; cmdI < mCommands.Length(); cmdI++ )
	{
		const ApplicationCommand* pCommand = mCommands[ cmdI ];
		for ( size_t argI = 0; argI < pCommand->argumentCount; argI++ )
		{
			if ( strncmp( pCommand->registeredArgs[ argI ], consoleArgument, 256 ) == 0 ) {
				*ppCommand = pCommand;
				return true;
			}
		}
	}

	return false;
}

void ConsoleArgumentParser::ShowHelp()
{
	printf_s( "Usage:\n" );
	printf_s( "\tBundlerApp" );
	for ( size_t i = 0; i < mCommands.Length(); i++ ) {
		mCommands[ i ]->helpFnc( true );
	}
	printf_s( "\n" );

	printf_s( "Options:\n" );
	for ( size_t i = 0; i < mCommands.Length(); i++ ) {
		mCommands[ i ]->helpFnc( false );
		printf_s( "\n" );
	}
	printf_s( "\n" );
}
