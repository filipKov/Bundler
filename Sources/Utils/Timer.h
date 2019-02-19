#pragma once

#include "stdafx.h"
#include <unordered_map>
#include <vector>
#include <numeric>

namespace Utils {

	// !!! DEPRECATED - USE "Utils::HighResolutionClock" INSTEAD !!!

	struct TimeGroup {
		LARGE_INTEGER lastRecordedPoint;
		LARGE_INTEGER tempPoint;

		std::vector< double > durations;

		inline void GetNow( LARGE_INTEGER& timePt ) {
			QueryPerformanceCounter( &timePt );
		};

		inline double ElpasedMilliseconds( LARGE_INTEGER& start, LARGE_INTEGER& end ) {
			LARGE_INTEGER freq;
			QueryPerformanceFrequency( &freq );
			return ( double( end.QuadPart ) - double( start.QuadPart ) ) * 1000.0 / double( freq.QuadPart );
		};

		inline void Start() {
			GetNow( lastRecordedPoint );
		};

		inline void SaveElapsed() {
			GetNow( tempPoint );
			double elapsed = ElpasedMilliseconds( lastRecordedPoint, tempPoint );
			durations.push_back( elapsed );
		};

		inline double GetAverage() {
			double sum = std::accumulate( durations.begin(), durations.end(), double( 0 ) );
			return sum / durations.size();
		};

		inline double GetLast() {
			return *durations.rbegin();
		};

	};

	class Timer {

	public:

		inline TimeGroup& GetTimeGroup( const char* pGroupName ) {
			return mGroups[ std::string( pGroupName ) ];
		};

		inline void Clear() {
			mGroups.clear();
		};

	protected:

		std::unordered_map < std::string, TimeGroup > mGroups;

	};

}