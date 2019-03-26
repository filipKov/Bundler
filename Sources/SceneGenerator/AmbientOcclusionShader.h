#pragma once



class AmbientOcclusionShader : public RenderToTextureShader {
public:

	struct Settings {
		Viewport viewport;
		uint sampleCount;
		float kernelRadius;
		float bias;
	};

public:

	AmbientOcclusionShader( __in const GLuint shaderProgram, __in const Settings& settings );

	void Clear() override;

	void SetViewport( __in const Viewport& viewport ) override;

	void GetAmbientOcclusionTexture( __out GLuint& texId );

	void GetPositionMap( __out GLuint& texId );

	void GetNormalMap( __out GLuint& texId );

protected:

	void InitializeFramebuffers();
	
	void SetupActiveTextureProperties() override;

protected:

	const uint RENDER_PASS_DEPTH = 0;
	const uint RENDER_PASS_AO_MAP = 1;
	const uint RENDER_PASS_BLUR = 2;

	AmbientOcclusionKernel mKernel;
	uint mKernelScale;
	Vector3f* m_pKernelSamples;
	GLuint mKernelNoiseTexture;
	Vector<float, 2> mKernelNoiseScale;
	float mKernelRadius;
	float mBias;

};

