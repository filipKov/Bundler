#pragma once

enum AppStages
{
	FILTER = 1,
	NOISE = 2,
	OPTIMIZE = 4,
};

enum class BundlerType 
{
	SEQ_CPU,
	PARALLEL_CPU,
	COMBINED,
	UNKNOWN
};

struct BundlerAppContext {

	char bundlePath[ MAX_PATH ];

	float filteringStrength;
	float pointNoise;
	float cameraNoise;

	BundlerType bundlerType;

	uint appStages;

	void SetInput( __in const char* pPath ) {
		sprintf_s( bundlePath, "%s", pPath );
	}

};