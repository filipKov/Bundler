#pragma once

class ProgressVisualiser {
public:

	void Initialize( __in const uint algId, __in const uint spaceCount );

	void Update();

	void Print( __in const float progress );

	void PrintOnProgressEnd( __in bool success, __in const double timeElapsed );

protected:

	uint mPos;
	uint mSpaceCount;

	char mPrintChar;

	char mProgressHeader[ 256 ];

	char mProgressBarString[ 32 ];
};

class IProgressProvider {
public:
	virtual float GetProgress() = 0;
};

class ProgressVisualisationThread {
public:

	void Initialize( __in const uint algId, __in IProgressProvider* pProgress );

	void End( __in const HRESULT errorCode );

protected:

	void Update();

protected:

	std::thread mUpdateThread;
	bool mInProgress;

	IProgressProvider* m_pProgress;
	ProgressVisualiser mVisualiser;

	HighResolutionClock mTimer;

};

struct ProgressSection {
	uint algorithmId;
	float weight;
	float progress;
};

class ProgressBar : public IProgressProvider
{
public:
	ProgressBar();
	~ProgressBar();

	float GetProgress() override;

	// -----------------------------------------------
	//  IProgressReportCallback 
	// -----------------------------------------------
	
	void    BeginSection( __in uint algId, __in float sectionFactor );

	HRESULT OnProgress( __in UINT position, __in UINT totalSteps );

	void    EndSection( __in HRESULT hr, __in_opt void* reserved );

	bool    IsAborted();

protected:

	std::mutex mDataLock;

	Containers::PagedVector< ProgressSection, 6 > mSections;
	HRESULT mLastSectionResult;

	ProgressVisualisationThread mVisThread;

};

