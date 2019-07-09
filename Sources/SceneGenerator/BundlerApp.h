#pragma once


class BundlerApp
{
public:

	static BundlerApp* GetApp();
	static void Release();

	void Start( __in const int argCount, __in char** ppArgs );

	BundlerAppContext& GetContext();

protected:

	BundlerApp();
	~BundlerApp();

	void SetDefaultContext();

	void PreprocessBundle( 
		__in const Bundler::Bundle* pBundle, 
		__in const Bundler::BundleAdditionalPayload* pBundleMetadata, 
		__out Bundler::Bundle* pPreprocessedBundle, 
		__out Bundler::BundleAdditionalPayload* pPreprocessedMeta);

	void AddNoiseToBundle(
		__in const Bundler::Bundle* pBundle,
		__out Bundler::Bundle* pNoisyBundle );

	void OptimizeBundle(
		__in const Bundler::Bundle* pBundle,
		__out Bundler::Bundle* pOptimizedBundle );

protected:

	static BundlerApp* m_pApp;

	BundlerAppContext mContext;

};

