#include "stdafx.h"
#include "ApplicationSectionStrings.h"
#include "ProgressBar.h"

#define STOPWATCH_ID "progress"

// -----------------------------------------------
//  ProgressVisualiser
// -----------------------------------------------

#define VIS_CHAR_EMPTY ' '
#define VIS_CHAR_BAR '-'
#define CLEAR_BAR "                                                                "

void ProgressVisualiser::Initialize( __in const uint algId, __in const uint spaceCount ) {
	mPos = spaceCount;
	mSpaceCount = spaceCount;

	_ASSERT( mSpaceCount < 32 );

	if ( !LoadProgressString( algId, mProgressHeader ) ) {
		sprintf_s( mProgressHeader, "Working" );
	}

	mPrintChar = VIS_CHAR_EMPTY;
	ByteFill( VIS_CHAR_EMPTY, mSpaceCount, mProgressBarString );
	mProgressBarString[ mSpaceCount ] = 0;
}

void ProgressVisualiser::Update() {
	mPos++;
	if ( mPos >= mSpaceCount ) {
		mPos = 0;
		mPrintChar = ( mPrintChar == VIS_CHAR_BAR ) ? VIS_CHAR_EMPTY : VIS_CHAR_BAR;
	}

	mProgressBarString[ mPos ] = mPrintChar;
}

void ProgressVisualiser::Print( __in const float progress ) {
	printf_s( "%s: [%s](%.3f%%)\r", mProgressHeader, mProgressBarString, progress * 100 );
}

void ProgressVisualiser::PrintOnProgressEnd( __in bool success, __in const double timeElapsed ) {
	const char* pSuccessString = "Done";
	if ( !success ) {
		pSuccessString = "Failed";
	}

	printf_s( "%s: %s\r", mProgressHeader, CLEAR_BAR );
	printf_s( "%s: %s (%.3fms elapsed)\n", mProgressHeader, pSuccessString, timeElapsed );
}


// -----------------------------------------------
//  ProgressVisualisationThread
// -----------------------------------------------


void ProgressVisualisationThread::Initialize( __in const uint algId, __in IProgressProvider* pProgress ) {
	m_pProgress = pProgress;

	mVisualiser.Initialize( algId, 16 );
	mVisualiser.Print( 0.0f );

	mTimer.Start();

	mInProgress = true;
	mUpdateThread = std::thread( [ this ] { this->Update(); } );
}

void ProgressVisualisationThread::End( __in const HRESULT errorCode ) {
	mInProgress = false;
	
	if ( mUpdateThread.joinable() ) {
		mUpdateThread.join();
	}

	mTimer.Stop( );

	mVisualiser.PrintOnProgressEnd( SUCCEEDED( errorCode ), mTimer.GetTotalTime< TimeUnits::Milliseconds >( ) );
}

void ProgressVisualisationThread::Update() {
	while ( mInProgress ) {
		mVisualiser.Update();
		mVisualiser.Print( m_pProgress->GetProgress() );
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}


// -----------------------------------------------
//  ProgressBar
// -----------------------------------------------

ProgressBar::ProgressBar() {
	mLastSectionResult = S_OK;
}


ProgressBar::~ProgressBar() {
	// EndProgress();
}

float ProgressBar::GetProgress() {
	float totalProg = 0.0f;
	float currentWeight = 1.0f;

	mDataLock.lock();

	for ( size_t i = 0; i < mSections.Length(); i++ ) {
		const ProgressSection& section = mSections[ i ];
		currentWeight *= section.weight;
		totalProg += section.progress * currentWeight;
	}

	mDataLock.unlock();

	// _ASSERT( totalProg <= 1.0f );
	totalProg = min( 1.0f, totalProg );

	return totalProg;
}

void ProgressBar::BeginSection( __in uint algId, __in float sectionFactor ) {
	_ASSERT_EXPR( ( ( sectionFactor <= 1.0 ) && ( sectionFactor > 0.0 ) ), "sectionFactor not in valid range" );

	mSections.Add( ProgressSection{ algId, sectionFactor, 0.0f } );
	mLastSectionResult = S_OK;

	if ( mSections.Length() == 1 ) {
		mVisThread.Initialize( algId, this );
	}
}

HRESULT ProgressBar::OnProgress( __in UINT position, __in UINT totalSteps ) {
	float sectionProgress = (float)position / totalSteps;
	if ( sectionProgress <= 1.0 )
	{
		mDataLock.lock();
		mSections.GetLast().progress = sectionProgress;
		mDataLock.unlock();

		return S_OK;
	}

	// _ASSERT_EXPR( false, "Bad bad progress reporting..." );

	return S_OK;
}

bool ProgressBar::IsAborted() {
	return ( mLastSectionResult == E_ABORT );
}

void ProgressBar::EndSection( __in HRESULT hr, __in_opt void* reserved ) {
	UNREFERENCED_PARAMETER( reserved );
	mLastSectionResult = hr;
	
	mDataLock.lock();

	if ( SUCCEEDED( hr ) ) {
		mSections.GetLast().progress = 1.0f;
	}

	ProgressSection lastSection = mSections.RemoveLast();

	if ( ( mSections.Length() == 0 ) ) {
		mVisThread.End( hr );
	}
	else
	{
		mSections.GetLast().progress += lastSection.progress * lastSection.weight;
	}

	mDataLock.unlock();
}
