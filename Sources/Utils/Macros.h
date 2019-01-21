#pragma once

#define GET_VARIABLE_NAME( variable ) ( __FUNCTION__ "::" #variable  )

// Environment defines
#ifdef _MSC_VER
	#ifdef _WIN64
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#elif __GNUG__
	#ifdef __x86_64__ || __ppc64__
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

// #define FORCE_DEBUG
#ifdef DEBUG
	undef DEBUG
#endif

#if defined( _DEBUG ) || defined( FORCE_DEBUG )
	#define DEBUG
#endif
