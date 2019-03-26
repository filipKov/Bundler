#include "stdafx.h"

#include "RenderingHelpers.h"
#include "IRenderingContext.h"
#include "IRenderable.h"

#include "Camera.h"

#include "Light.h"
#include "DirectionalLight.h"
#include "PointLight.h"

#include "Shader.h"
#include "AlbedoPointCloudShader.h"


AlbedoPointCloudShader::AlbedoPointCloudShader( __in const GLuint shaderProgram, __in const Viewport& viewport ) :
	ShaderProgram( SHADER_TYPE::ALBEDO_POINT_CLOUD, shaderProgram, viewport )
{
	m_pCamera = nullptr;
}
