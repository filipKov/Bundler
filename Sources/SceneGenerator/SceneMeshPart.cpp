#include "stdafx.h"

#include "RenderingLib.h"

#include "Transformable.h"

#include "Texture.h"

#include "SceneMeshPart.h"


SceneMeshPart::SceneMeshPart()
{
}


SceneMeshPart::~SceneMeshPart()
{
}

void SceneMeshPart::SetTexture( __in const Texture* pTex ) {
	m_pTexture = pTex;
}

void SceneMeshPart::SetParent( __in const Transformable* pParent ) {
	m_pParent = pParent;
}

Matrix<float, 4, 4> SceneMeshPart::GetWorldTransform() const {
	return m_pParent->GetTransform() * GetTransform();
}

const GLuint SceneMeshPart::GetGlTexture() const {
	return m_pTexture->GetGlTexture();
}