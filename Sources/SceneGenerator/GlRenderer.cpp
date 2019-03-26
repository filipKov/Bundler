#include "stdafx.h"
#include <map>
#include <string>

#include "glslang.h"

#include "Macros.h"
#include "AlgebraLib.h"

#include "Camera.h"

#include "RenderableObject.h"

#include "GlRenderer.h"


void GlRenderer::Init() {
	m_vertexColorDiffuseShader = createShaderProgram( GET_SHADER_PATH( "vertexColor.vert" ), GET_SHADER_PATH( "vertexColor.frag" ) );
}

void GlRenderer::Render() const {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	InitializeShader();

	for ( auto objectIter = m_objects.begin(); objectIter != m_objects.end(); objectIter++ ) {
		objectIter->second->Render( m_vertexColorDiffuseShader );
	}

	glutSwapBuffers();
}

void GlRenderer::SetViewportCamera( __in Camera& camera ) {
	m_pCamera = &camera;
}

void GlRenderer::RegisterObject( __in_z const char* pId, __in RenderableObject* pObject ) {
	m_objects[pId] = pObject;
}

void GlRenderer::ClearObjects() {
	m_objects.clear();
}

void GlRenderer::InitializeShader() const {
	glUseProgram( m_vertexColorDiffuseShader );

	int projMatrixLocation = glGetUniformLocation( m_vertexColorDiffuseShader, "projectionMatrix" );
	if ( projMatrixLocation > -1 ) {
		glUniformMatrix4fv( projMatrixLocation, 1, GL_FALSE, m_pCamera->GetProjectionMatrix().Elements() );
	}

	int modelViewLocation = glGetUniformLocation( m_vertexColorDiffuseShader, "modelViewMatrix" );
	if ( modelViewLocation > -1 ) {
		glUniformMatrix4fv( modelViewLocation, 1, GL_TRUE, m_pCamera->GetViewMatrix().Elements() );
	}
}

/*

#include "AlgebraLib.h"
#include "glStructs.h"
#include "RenderingUtils.h"

#include "Camera.h"

#include "SceneMesh.h"
#include "Texture.h"
#include "ObjectComponent.h"
#include "SceneObject.h"
#include "Grid3D.h"

#include "Light.h"
#include "DiffuseLight.h"

#include "Scene.h"


#include "GlRenderer.h"


void GlRenderer::Render( __in const Scene& scene ) const {

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if ( mGrid.bEnabled ) {
		RenderGrid( scene.camera );
	}

	for ( auto shaderPair = mShaderPrograms.begin(); shaderPair != mShaderPrograms.end(); shaderPair++ ) {
		const ShaderProgram& shaderProgram = shaderPair->second;

		glUseProgram( shaderProgram.glProgram );
		InitGlobalShaderParameters( shaderProgram.glProgram, scene );

		for ( auto objectIter = scene.objects.begin(); objectIter != scene.objects.end(); objectIter++ ) {
			const uint componentCount = objectIter->GetComponentCount();
			for ( uint componentI = 0; componentI < componentCount; componentI++ ) {
				const ObjectComponent& component = objectIter->GetComponent( componentI );
				if ( component.UsesShader( shaderProgram.glProgram ) ) {
					InitObjectShaderParameters( shaderProgram.glProgram, scene, component );
					RenderComponent( component );
				}
			}
		}
	}

	glutSwapBuffers();
}

void GlRenderer::RegisterShaderProgram( __in const std::string identifier, __in const SHADER_TYPE type, __in const GLuint glProgram ) {
	ShaderProgram sp;
	sp.type = type;
	sp.glProgram = glProgram;
	mShaderPrograms[identifier] = sp;
}

void GlRenderer::EnableGrid( __in const Grid3D& grid ) {
	mGrid = grid;
	mGrid.bEnabled = true;
}

void GlRenderer::InitGlobalShaderParameters( __in const GLuint shader, __in const Scene& scene ) const {
	int projMatrixLocation = glGetUniformLocation( shader, "projectionMatrix" );
	if ( projMatrixLocation > -1 ) {
		glUniformMatrix4fv( projMatrixLocation, 1, GL_FALSE, scene.camera.GetProjectionMatrix().Elements() );
	}

	CreateLights( shader, scene.camera, scene.lights.size(), scene.lights.data() );
}

void GlRenderer::CreateLights( __in const GLuint shader, __in const Camera& camera, __in const uint lightCnt, __in_ecount( lightCnt ) const Light* pLights ) const {
	for ( uint i = 0; i < lightCnt; i++ ) {
		auto lightPos = camera.GetViewMatrix() * pLights->GetPosition();

		int lightPosLocation = glGetUniformLocation( shader, "lightPos" );
		glUniform3fv( lightPosLocation, 3, lightPos.Elements() );

		int lightColorLocation = glGetUniformLocation( shader, "lightColor" );
		glUniform3fv( lightColorLocation, 3, pLights->GetColor().Elements() );

		pLights++;
	}
}

void GlRenderer::InitObjectShaderParameters( __in const GLuint shader, __in const Scene& scene, __in const ObjectComponent& object ) const {
	auto modelViewMatrix = scene.camera.GetViewMatrix() * object.GetTransformation();
	
	int modelViewLocation = glGetUniformLocation( shader, "modelViewMatrix" );
	if ( modelViewLocation > -1 ) {
		glUniformMatrix4fv( modelViewLocation, 1, GL_TRUE, modelViewMatrix.Elements() );
	}

	int normalMatrixLocation = glGetUniformLocation( shader, "normalMatrix" );
	if ( normalMatrixLocation > -1 ) {
		Matrix3x3 normalMatrix( modelViewMatrix );
		glUniformMatrix3fv( normalMatrixLocation, 1, GL_TRUE, normalMatrix.Elements() );
	}

	int texturePresentLocation = glGetUniformLocation( shader, "texturePresent" );
	if ( texturePresentLocation > -1 ) {
		glUniform1i( texturePresentLocation, object.IsTextured() );
		if ( object.IsTextured() ) {
			int textureLocation = glGetUniformLocation( shader, "diffuseTexture" );
			if ( textureLocation > -1 ) {
				glUniform1i( textureLocation, 0 );
			}
		}
	}
}

void GlRenderer::RenderComponent( __in const ObjectComponent& object ) const {
	const GlGpuObject& mesh = object.GetMesh().mGpuObject;
	const MeshInfo& meshInfo = object.GetMesh().mProperties;
	glBindVertexArray( mesh.arrayObject );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexBuffer );
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.vertexNormalBuffer );
	glEnableVertexAttribArray( 1 );
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_TRUE, 0, NULL );

	glBindBuffer( GL_ARRAY_BUFFER, mesh.textureCoordinateBuffer );
	glEnableVertexAttribArray( 2 );
	glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 0, NULL );

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mesh.polygonIndexBuffer );

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, object.GetTexture() );

	glDrawElements( GL_TRIANGLES, 3* meshInfo.faceCount, GL_UNSIGNED_INT, NULL );
}

void GlRenderer::RenderGrid( __in const Camera& camera ) const {
	glUseProgram( mGrid.shader.glProgram );
	mGrid.SetShaderUniforms( camera.GetViewMatrix(), camera.GetProjectionMatrix() );

	glBindVertexArray( mGrid.gpuObject.arrayObject );
	glDrawElements( GL_LINES, mGrid.lineIndexCount, GL_UNSIGNED_INT, NULL );
}

*/