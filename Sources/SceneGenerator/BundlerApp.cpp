#include "stdafx.h"

#include "ConsoleInputExceptions.h"
#include "ApplicationContext.h"
#include "ConsoleArgumentParser.h"

#include "ViewerLauncher.h"
#include "ApplicationExceptions.h"

#include "ProgressBar.h"
#include "ApplicationSectionStrings.h"

#include <iostream>
#include <fstream>

#include "Optim.h"

#include "BundlerApp.h"
#include "BundlerAppCommands.h"

using namespace Bundler;

// ----------------------------------
//	Application methods
// ----------------------------------
BundlerApp* BundlerApp::m_pApp = nullptr;

BundlerApp* BundlerApp::GetApp( )
{
	if ( !BundlerApp::m_pApp )
	{
		m_pApp = new BundlerApp( );
	}
	return m_pApp;
}

void BundlerApp::Release( )
{
	if ( BundlerApp::m_pApp )
	{
		delete m_pApp;
	}
}

BundlerApp::BundlerApp( )
{
	SetDefaultContext( );
}

BundlerApp::~BundlerApp( )
{
	// Clear();
}

void AddDefaultParserCallbacks( __inout ConsoleArgumentParser& parser )
{
	parser.RegisterCommand( &setInputCommand );
	parser.RegisterCommand( &setFilterCommand );
	parser.RegisterCommand( &setNoiseCommand );
	parser.RegisterCommand( &setOptimizeCommand );
}

void BundlerApp::Start( __in const int argCount, __in char** ppArgs )
{
	ConsoleArgumentParser parser;
	AddDefaultParserCallbacks( parser );

	parser.ParseArgs( argCount, ppArgs );

	Bundle bundle;
	BundleAdditionalPayload bundleMetadata;

	Bundle preprocessedBundle;
	BundleAdditionalPayload preprocessedMeta;

	Bundle noisedBundle;

	Bundle optimizedBundle;

	HRESULT hr = Import::BundleImporter::Import( mContext.bundlePath, &bundle, &bundleMetadata );
	if ( SUCCEEDED( hr ) )
	{
		Scalar mean[3];
		Scalar stdev = 0;
		Bundle normalizedBundle;
		Preprocess::Normalize( &bundle, &bundle, mean, &stdev );

		if ( mContext.appStages & AppStages::FILTER )
		{
			PreprocessBundle( &bundle, &bundleMetadata, &preprocessedBundle, &preprocessedMeta );
			if ( mContext.appStages & AppStages::NOISE )
			{
				AddNoiseToBundle( &preprocessedBundle, &noisedBundle );
				if ( mContext.appStages & AppStages::OPTIMIZE )
				{
					Scalar targetError = Scalar( GetGeometricError< CameraModels::CameraModel6DoF< 1 > >( &preprocessedBundle ) );
					OptimizeBundle( &noisedBundle, targetError, &optimizedBundle );
				}
			}
		}
	}

	if ( SUCCEEDED( hr ) )
	{
		if ( mContext.appStages & AppStages::FILTER )
		{
			printf_s( "Post Filtering\n" );
			printf_s( "\tCamera Count: %zu\n", preprocessedBundle.cameras.Length( ) );
			printf_s( "\tPoint Count: %zu\n", preprocessedBundle.points.Length( ) );
			printf_s( "\tCorresp Count: %zu\n", preprocessedBundle.projections.Length( ) );

			printf_s( "\tModel Error: %f\n", GetGeometricError< CameraModels::CameraModel6DoF< 1 > >( &preprocessedBundle ) );
		}

		if ( mContext.appStages & AppStages::NOISE )
		{
			printf_s( "Post Noise\n" );
			printf_s( "\tModel Error: %f\n", GetGeometricError< CameraModels::CameraModel6DoF< 1 > >( &noisedBundle ) );
		}

		if ( mContext.appStages & AppStages::OPTIMIZE )
		{
			printf_s( "Post Optimization\n" );
			printf_s( "\tModel Error: %f\n", GetGeometricError< CameraModels::CameraModel6DoF< 1 > >( &optimizedBundle ) );
		}
	}

	if ( SUCCEEDED( hr ) )
	{
		ViewerLauncher::InitializeEmpty( );
		
		float bundlePivot[3] = { 0 };
		ViewerLauncher::ShowBundle( &bundle, &bundleMetadata, bundlePivot );

		if ( mContext.appStages & AppStages::FILTER )
		{
			ViewerLauncher::ShowBundle( &preprocessedBundle, &preprocessedMeta, bundlePivot );
		}

		if ( mContext.appStages & AppStages::NOISE )
		{
			ViewerLauncher::ShowBundle( &noisedBundle, &preprocessedMeta, bundlePivot );
		}

		if ( mContext.appStages & AppStages::OPTIMIZE )
		{
			ViewerLauncher::ShowBundle( &optimizedBundle, &preprocessedMeta, bundlePivot );
		}
		
		ViewerLauncher::LaunchViewer( );
	}
}

BundlerAppContext& BundlerApp::GetContext() {
	return mContext;
}

void BundlerApp::SetDefaultContext()
{
	mContext.filteringStrength = 1.0f;
	mContext.appStages = 0;
}

void BundlerApp::PreprocessBundle(
	__in const Bundler::Bundle* pBundle,
	__in const Bundler::BundleAdditionalPayload* pBundleMetadata,
	__out Bundler::Bundle* pPreprocessedBundle,
	__out Bundler::BundleAdditionalPayload* pPreprocessedMeta )
{
	double center[3];
	double normalizedStdev;
	Preprocess::GetMean( pBundle, center );
	Preprocess::GetStdev( pBundle, center, &normalizedStdev );
	
	Scalar filterCenter[3];
	Scalar filterRadius = Scalar( mContext.filteringStrength * normalizedStdev );
	MatrixCast< double, Scalar, 3, 1 >( center, filterCenter );
	
	Preprocess::FilterAroundCenter( pBundle, filterCenter, filterRadius, pPreprocessedBundle, pBundleMetadata, pPreprocessedMeta, NULL );
}

void BundlerApp::AddNoiseToBundle(
	__in const Bundler::Bundle* pBundle,
	__out Bundler::Bundle* pNoisyBundle )
{
	// uint noiseMask = SceneGenerator::SceneGenAutoNoiseMask::POINTS;
	
	const Scalar noiseStrength = mContext.pointNoise;
	SceneGenerator::SceneGenNoiseSettings noise = { 0 };
	noise.pointSettings.minDelta = -noiseStrength;
	noise.pointSettings.maxDelta = noiseStrength;

	const Scalar cameraStrength = mContext.cameraNoise;
	noise.cameraSettings.translationNoise.minDelta = -cameraStrength;
	noise.cameraSettings.translationNoise.maxDelta = cameraStrength;
	noise.cameraSettings.rotationNoise.minDelta = -cameraStrength;
	noise.cameraSettings.rotationNoise.maxDelta = cameraStrength;

	SceneGenerator::SceneGen::AddNoise( &noise, pBundle, pNoisyBundle );
}

void BundlerApp::OptimizeBundle(
	__in const Bundler::Bundle* pBundle,
	__in const Bundler::Scalar targetError,
	__out Bundler::Bundle* pOptimizedBundle )
{
	constexpr const uint maxIterations = 16;
	constexpr const uint maxIterationsSolver = 16;

	OptimizerSettings settings;
	settings.errorTolerance = targetError; // Scalar( 10e-6 );
	settings.dampeningUp = Scalar( 100 );
	settings.dampeningDown = Scalar( 0.1 );
	settings.initialDampeningFactor = 0;
	settings.maxIterations = maxIterations;
	settings.linearSolverSettings.errorTolerance = Scalar( 10e-6 );
	settings.linearSolverSettings.maxIterations = maxIterationsSolver;

	OptimizerStatistics stats = { 0 };

	HighResolutionClock stopwatch;
	stopwatch.Start( );

	switch ( mContext.bundlerType )
	{
	case BundlerType::SEQ_CPU:
		OptimizeBundleSeq< CameraModels::CameraModel6DoF< maxIterations + 1 > >( pBundle, &settings, &stats, pOptimizedBundle );
		break;
	case BundlerType::PARALLEL_CPU:
		OptimizeBundleParallelCPU< CameraModels::CameraModel6DoF< maxIterations + 1 > >( pBundle, &settings, &stats, pOptimizedBundle );
		break;
	case BundlerType::COMBINED:
		OptimizeBundleParallelCombined< CameraModels::CameraModel6DoF< maxIterations + 1 > >( pBundle, &settings, &stats, pOptimizedBundle );
		break;
	default:
		break;
	}

	stopwatch.Stop( );
	printf_s( "Optimization ended in %d iterations (took %fs)\n", stats.iterationCount, stopwatch.GetTotalTime<TimeUnits::Seconds>( ) );
}
