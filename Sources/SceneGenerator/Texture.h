#pragma once
class Texture
{
public:

	Texture();

	Texture( __in_z const char* pFilename );
	
	void Load( __in_z const char* pFilename );

	bool IsValid() const;

	const GLuint GetGlTexture() const;

	void SetGlTexture( __in GLuint tex );

	void Release();

protected:

	bool mValid;
	GLuint mGlTexture;

};

