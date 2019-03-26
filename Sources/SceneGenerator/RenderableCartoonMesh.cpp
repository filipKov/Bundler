#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "Camera.h"

#include "RenderableCartoonMesh.h"

#define TEXTURE_UNIT_DIFFUSE 0
#define TEXTURE_UNIT_SHADOW 2
#define TEXTURE_UNIT_AO 3

#define TEXTURE_DIFFUSE GL_TEXTURE0 + TEXTURE_UNIT_DIFFUSE
#define TEXTURE_SHADOW GL_TEXTURE0 + TEXTURE_UNIT_SHADOW
#define TEXTURE_AO GL_TEXTURE0 + TEXTURE_UNIT_AO

#define ATTRIB_POINTER_ID_VERTICES 0
#define	ATTRIB_POINTER_ID_NORMALS 1
#define ATTRIB_POINTER_ID_VERTEX_COLORS 2
#define ATTRIB_POINTER_ID_TEXCOORDS 3

GlMesh::GlMesh() {
	ByteFill( 0, 1, this );
}

void GlMesh::Release() {
	if ( arrayObject != 0 ) {
		glDeleteBuffers( 5, &polygonIndexBuffer );

		glDeleteVertexArrays( 1, &arrayObject );
	}
}

RenderableCartoonMesh::RenderableCartoonMesh()
{
	ByteFill( 0, 1, this );
}


RenderableCartoonMesh::~RenderableCartoonMesh()
{
	Release();
}

void RenderableCartoonMesh::SetMesh( __in const GlMesh& mesh, __in const MeshInfo& info, __in const BoundingBox& bbox ) {
	mMesh = mesh;
	mInfo = info;
	mBBox = bbox;
}

void RenderableCartoonMesh::Release() {
	mMesh.Release();
	ByteFill( 0, 1, this );
}

void RenderableCartoonMesh::Render( __in const IRenderingContext* pContext ) const {
	switch ( pContext->GetShader().type )
	{
	case SHADER_TYPE::ALBEDO:
		RenderColor( pContext );
		break;
	case SHADER_TYPE::AMBIENT_OCCLUSION:
		RenderAmbientOcclusion( pContext );
		break;
	default:
		break;
	}
}

const BoundingBox& RenderableCartoonMesh::GetBoundingBox() const {
	return mBBox;
}

void RenderableCartoonMesh::RenderColor( __in const IRenderingContext* pContext ) const {
	const Shader shader = pContext->GetShader();
	
	SetTransformMatrices( pContext );
	SetTexture( shader.id );

	BindVertexArrayObject( mMesh );

	LoadUniform( shader.id, UNIFORM_RENDER_PASS, glUniform1ui, RENDER_PASS_OUTLINE );
	RenderWireframe( mInfo.faceCount );

	LoadUniform( shader.id, UNIFORM_RENDER_PASS, glUniform1ui, RENDER_PASS_COLOR );
	RenderSolid( mInfo.faceCount );

	UnbindVertexArrayObject();
}

void RenderableCartoonMesh::RenderAmbientOcclusion( __in const IRenderingContext* pContext ) const {
	SetTransformMatrices( pContext );
	
	BindVertexArrayObject( mMesh );
	RenderSolid( mInfo.faceCount );
	UnbindVertexArrayObject();
}

void RenderableCartoonMesh::SetTransformMatrices( __in const IRenderingContext* pContext ) const {
	const Shader shader = pContext->GetShader();

	auto modelViewMatrix = pContext->GetViewMatrix() * GetWorldTransform();;
	LoadUniform( shader.id, UNIFORM_VS_MODEL_VIEW_MATRIX, glUniformMatrix4fv, 1, (GLboolean)GL_TRUE, modelViewMatrix.Elements() );

	float normalMatrix[9];
	ShallowCopy( modelViewMatrix.Elements( ), 3, normalMatrix );
	ShallowCopy( modelViewMatrix.Elements( ) + 4, 3, normalMatrix + 3 );
	ShallowCopy( modelViewMatrix.Elements( ) + 8, 3, normalMatrix + 6 );

	LoadUniform( shader.id, UNIFORM_VS_NORMAL_MATRIX, glUniformMatrix3fv, 1, (GLboolean)GL_TRUE, normalMatrix );
}

void RenderableCartoonMesh::SetTexture( __in const GLuint shaderId ) const {
	bool isTextured = IsTextured();

	LoadUniform( shaderId, UNIFORM_FS_MATERIAL_IS_TEXTURED, glUniform1i, isTextured );
	if ( isTextured ) {
		LoadUniform( shaderId, UNIFORM_FS_MATERIAL_TEXTURE, glUniform1i, TEXTURE_UNIT_DIFFUSE );
		glActiveTexture( TEXTURE_DIFFUSE );
		glBindTexture( GL_TEXTURE_2D, GetGlTexture() );
	}

	LoadUniform( shaderId, UNIFORM_FS_MATERIAL_COLOR_DIFFUSE, glUniform4f, 0.5f, 0.5f, 0.5f, 1.0f );

	LoadUniform( shaderId, UNIFORM_VSFS_MATERIAL_HAS_VERTEX_COLORS, glUniform1i, mInfo.hasVertexColors );
}

bool RenderableCartoonMesh::IsTextured() const {
	return mInfo.hasTextureCoordinates && ( GetGlTexture() > 0 );
}

void RenderableCartoonMesh::BindVertexArrayObject( __in const GlMesh& mesh ) {
	glBindVertexArray( mesh.arrayObject );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
	glEnableVertexAttribArray( ATTRIB_POINTER_ID_VERTICES );
	glVertexAttribPointer( ATTRIB_POINTER_ID_VERTICES, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexNormalBuffer );
	glEnableVertexAttribArray( ATTRIB_POINTER_ID_NORMALS );
	glVertexAttribPointer( ATTRIB_POINTER_ID_NORMALS, 3, GL_FLOAT, GL_TRUE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexColorBuffer );
	glEnableVertexAttribArray( ATTRIB_POINTER_ID_VERTEX_COLORS );
	glVertexAttribPointer( ATTRIB_POINTER_ID_VERTEX_COLORS, 4, GL_FLOAT, GL_TRUE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.textureCoordinateBuffer );
	glEnableVertexAttribArray( ATTRIB_POINTER_ID_TEXCOORDS );
	glVertexAttribPointer( ATTRIB_POINTER_ID_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.polygonIndexBuffer );
}

void RenderableCartoonMesh::UnbindVertexArrayObject() {
	glDisableVertexAttribArray( ATTRIB_POINTER_ID_VERTICES );
	glDisableVertexAttribArray( ATTRIB_POINTER_ID_NORMALS );
	glDisableVertexAttribArray( ATTRIB_POINTER_ID_VERTEX_COLORS );
	glDisableVertexAttribArray( ATTRIB_POINTER_ID_TEXCOORDS );

	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindVertexArray( 0 );
}

void RenderableCartoonMesh::RenderWireframe( __in const uint faceCount ) const {
	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	glDepthFunc( GL_LEQUAL );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glLineWidth( 4 );

	glDrawElements( GL_TRIANGLES, 3 * faceCount, GL_UNSIGNED_INT, NULL );
}

void RenderableCartoonMesh::RenderSolid( __in const uint faceCount ) const {
	glCullFace( GL_BACK );
	glDepthFunc( GL_LESS );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glDrawElements( GL_TRIANGLES, 3 * faceCount, GL_UNSIGNED_INT, NULL );
}
