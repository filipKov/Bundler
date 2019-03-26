#include "stdafx.h"

#include <glew.h>
#include "loadBMP.h"

#include "Texture.h"

Texture::Texture() {
	mValid = false;
	mGlTexture = 0;
}

Texture::Texture( __in_z const char* pFilename ) {
	Load( pFilename );
}

void Texture::Load( __in_z const char* pFilename ) {

	// BitmapImage image;
	// LoadBitmapImage( pFilename, image );

	uint width = 0;
	uint height = 0;
	const byte* pImageData = loadBMP( pFilename, width, height );

	glGenTextures( 1, &mGlTexture );
	glBindTexture( GL_TEXTURE_2D, mGlTexture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pImageData );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	mValid = true;

	delete[] pImageData;
}

bool Texture::IsValid() const {
	return mValid;
}

const GLuint Texture::GetGlTexture() const {
	return mGlTexture;
}

void Texture::SetGlTexture( __in GLuint tex ) {
	mGlTexture = tex;
	mValid = true;
}

void Texture::Release() {
	if ( IsValid() ) {
		glDeleteTextures( 1, &mGlTexture );
	}
}