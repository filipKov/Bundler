#pragma once

#define MAX_COLOR_ATTACHMENTS 8
#define ATTACHMENTS_PER_COMPONENT MAX_COLOR_ATTACHMENTS + 2
#define MAX_FRAMEBUFFER_ATTACHMENTS 8
#define MAX_TOTAL_TEXTURES MAX_FRAMEBUFFER_ATTACHMENTS * ATTACHMENTS_PER_COMPONENT

struct FramebufferAttachment {
	FramebufferAttachment() {
		colorAttachmentCnt = 0;
		depthAttachmentActive = false;
		depthAttachmentRenderbuffer = false;
		stencilAttachmentActive = false;
	}

	uint colorAttachmentCnt;
	GLuint colorAttachments[MAX_COLOR_ATTACHMENTS];

	bool depthAttachmentActive;
	bool depthAttachmentRenderbuffer;
	GLuint depthAttachment;

	bool stencilAttachmentActive;
	GLuint stencilAttachment;
};

struct TextureParam {
	GLint parameterName;
	GLint parameterValue;
};

struct TextureParameters {
	GLint internalFormat;
	uint width;
	uint height;
	GLenum format;
	GLenum type;

	uint paramCnt;
	TextureParam* pParams;
};

class RenderToTextureShader : public ShaderProgram {
public:

	~RenderToTextureShader();

	void GetFramebufferAttachmentsPtr( __out uint& count, __deref_out FramebufferAttachment** ppAttachments );

	virtual void Clear();

	void Render( __in const uint objectCnt, __in_ecount( objectCnt ) IRenderable* const* ppObjects ) override;

protected:

	RenderToTextureShader( __in const SHADER_TYPE type, __in const GLuint shaderProgram, __in const Viewport& viewport );

	void BindFramebufferAttachments( __in const FramebufferAttachment& attachment );

	virtual void SetupActiveTextureProperties() = 0;

	bool GenerateFramebufferAttachment( __inout FramebufferAttachment& fba );

	bool AllocateTexture( __out GLuint& textureId );

	bool AllocateRenderbuffer( __out GLuint& rbId );

	void GenerateDepthTexture( __in const GLint internalFormat, __in const uint width, __in const uint height, __in const GLuint textureId );

	void GenerateColorTexture( __in const GLint internalFormat, __in const uint width, __in const uint height, __in const GLuint textureId, __in const GLint type = GL_UNSIGNED_BYTE );

	void GenerateTexture( __in const GLuint textureId, __in const TextureParameters& texParams );

	void GenerateRenderbuffer( __in const GLuint rbId, __in const TextureParameters& params );

protected:

	GLuint mTextureStorage[MAX_TOTAL_TEXTURES];
	uint mAllocatedTextureCnt;
	uint mActiveTextureCnt;

	GLuint mRenderbufferStorage[MAX_TOTAL_TEXTURES];
	uint mAllocatedRenderbufferCnt;
	uint mActiveRenderbufferCnt;
	
	FramebufferAttachment mFramebufferAttachments[MAX_FRAMEBUFFER_ATTACHMENTS];
	uint mActiveFramebufferAttachmentCnt;
	uint mActiveFramebufferAttachmentIx;
	
	GLuint mFramebufferObject;

};

