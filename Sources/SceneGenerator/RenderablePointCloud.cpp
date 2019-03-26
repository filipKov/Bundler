#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "Camera.h"

#include "Shader.h"

#include "RenderablePointCloud.h"

#define ATTRIB_POINTER_ID_VERTICES 0
#define ATTRIB_POINTER_ID_VERTEX_COLORS 1

void GlPointCloud::Release() {
	glDeleteBuffers( 2, &vertexBuffer );
	glDeleteVertexArrays( 1, &arrayObject );
}


RenderablePointCloud::RenderablePointCloud()
{
	mVertexCount = 0;
	ByteFill( 0, 1, &mPointCloud );
}


RenderablePointCloud::~RenderablePointCloud() {
	Release();
}

void RenderablePointCloud::SetPointCloud( __in const uint vertexCount, __in const GlPointCloud& pc, __in const BoundingBox& bbox ) {
	mVertexCount = vertexCount;
	mPointCloud = pc;
	mBBox = bbox;
}

void RenderablePointCloud::Release() {
	mPointCloud.Release();
	mVertexCount = 0;
}

void RenderablePointCloud::Render( __in const IRenderingContext* pContext ) const {
	switch ( pContext->GetShader().type )
	{
	case SHADER_TYPE::ALBEDO_POINT_CLOUD:
		RenderColored( pContext );
		break;
	default:
		break;
	}
}

const BoundingBox& RenderablePointCloud::GetBoundingBox() const {
	return mBBox;
}

void RenderablePointCloud::RenderColored( __in const IRenderingContext* pContext ) const {
	SetTransformMatrices( pContext );
	
	LoadUniform( pContext->GetShader().id, UNIFORM_VS_HAS_VERTEX_COLORS, glUniform1i, ( mPointCloud.vertexColorBuffer != 0 ) );

	BindVertexArrayObject( mPointCloud );
	glEnable( GL_POINT_SMOOTH );
	glPointSize( 5.0f );
	glDrawArrays( GL_POINTS, 0, mVertexCount );
	UnbindVertexArrayObject();
}

void RenderablePointCloud::SetTransformMatrices( __in const IRenderingContext* pContext ) const {
	auto modelViewMatrix = pContext->GetViewMatrix() * GetWorldTransform();
	LoadUniform( pContext->GetShader().id, UNIFORM_VS_MODEL_VIEW_MATRIX, glUniformMatrix4fv, 1, (GLboolean)GL_TRUE, modelViewMatrix.Elements() );
}

void RenderablePointCloud::BindVertexArrayObject( __in const GlPointCloud& pc ) {
	glBindVertexArray( pc.arrayObject );
	
	glBindBuffer( GL_ARRAY_BUFFER, pc.vertexBuffer );
	glEnableVertexAttribArray( ATTRIB_POINTER_ID_VERTICES );
	glVertexAttribPointer( ATTRIB_POINTER_ID_VERTICES, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	if ( pc.vertexColorBuffer != 0 ) {
		glBindBuffer( GL_ARRAY_BUFFER, pc.vertexColorBuffer );
		glEnableVertexAttribArray( ATTRIB_POINTER_ID_VERTEX_COLORS );
		glVertexAttribPointer( ATTRIB_POINTER_ID_VERTEX_COLORS, 4, GL_FLOAT, GL_TRUE, 0, NULL );
	}
}

void RenderablePointCloud::UnbindVertexArrayObject() {
	glDisableVertexAttribArray( ATTRIB_POINTER_ID_VERTICES );
	glDisableVertexAttribArray( ATTRIB_POINTER_ID_VERTEX_COLORS );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}
