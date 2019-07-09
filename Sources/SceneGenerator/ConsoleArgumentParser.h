#pragma once

struct ApplicationCommand {
	void ( *callbackFnc )( __in const int argCount, __in_ecount( argCount ) char** pArgs, __inout int* pArgI );

	void( *helpFnc ) ( __in const bool showShort );
	
	uint argumentCount;
	const char* registeredArgs[ 32 ];
};

class ConsoleArgumentParser
{
public:
	ConsoleArgumentParser();
	~ConsoleArgumentParser();

	void RegisterCommand( __in const ApplicationCommand* pCommand );

	void ParseArgs( __in const int argCount, __in_ecount( argCount ) char** ppArgs );

	void ShowHelp( );

protected:

	void ParseArgument( __in_z const char* pArg, __in const int argCount, __in_ecount( argCount ) char** ppArgs, __inout int* pArgI );

	void ParseShortArgument( __in_z const char* pArg, __in const int argCount, __in_ecount( argCount ) char** ppArgs, __inout int* pArgI );

	bool FindCommand( __in_z const char* consoleArgument, __deref_out const ApplicationCommand** ppCommand );

protected:

	Containers::PagedVector< const ApplicationCommand*, 5 > mCommands;

};

