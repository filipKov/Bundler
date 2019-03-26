#pragma once

#define MAX_UNIFORM_NAME_LENGTH 255
const char* GetUniformArrayName( char* pBuffer, const char* pTemplate, uint ix );

#ifndef STRIGIFY 
	#define STRINGIFY(a) #a
#endif

#define UNIFORM_VS_MODEL_VIEW_MATRIX "modelViewMatrix"
#define UNIFORM_VS_NORMAL_MATRIX "normalMatrix"
#define UNIFORM_VS_PROJECTION_MATRIX "projectionMatrix"
#define UNIFORM_VS_VIEW_MATRIX "viewMatrix"
#define UNIFORM_VS_MODEL_MATRIX "modelMatrix"

#define UNIFORM_VSFS_LIGHT_COUNT "lightCount"
#define UNIFORM_VS_ACTIVE_LIGHT_IX "lightIx"
#define UNIFORM_VS_LIGHT_VIEW_MATRIX(buffer, i) GetUniformArrayName( buffer, "lightViewMats[%u]", i )
#define UNIFORM_VS_LIGHT_PROJECTION(buffer, i) GetUniformArrayName( buffer, "lightProjectionMats[%u]", i )
#define UNIFORM_FS_LIGHT_DEPTH_MAP( buffer, i ) GetUniformArrayName( buffer, "lightDepthMaps[%u]", i )

#define UNIFORM_FS_DIR_LIGHT_CNT "directionalLightCount"
#define UNIFORM_FS_DIR_LIGHT_DIR(buffer, i) GetUniformArrayName( buffer, "dirLights[%u].direction", i )
#define UNIFORM_FS_DIR_LIGHT_COLOR(buffer, i)  GetUniformArrayName( buffer, "dirLights[%u].color", i )

#define UNIFORM_FS_POINT_LIGHT_CNT "pointLightCount"
#define UNIFORM_FS_POINT_LIGHT_POS(buffer, i)  GetUniformArrayName( buffer, "pointLights[%u].position", i )
#define UNIFORM_FS_POINT_LIGHT_COLOR(buffer, i)  GetUniformArrayName( buffer, "pointLights[%u].color", i )
#define UNIFORM_FS_POINT_LIGHT_ATTENUATION(buffer, i)  GetUniformArrayName( buffer, "pointLights[%u].attenuation", i )

#define UNIFORM_VSFS_SHADOW_MAP_CNT "shadowMapCount"
#define UNIFORM_VS_SHADOW_TRANFORM_PROJECTION( buffer, i ) GetUniformArrayName( buffer, "shadowMapTransforms[%u].biasedProjection", i )
#define UNIFORM_VS_SHADOW_TRANFORM_MODEL_VIEW( buffer, i ) GetUniformArrayName( buffer, "shadowMapTransforms[%u].modelView", i )
#define UNIFORM_FS_SHADOW_MAPS(buffer, i)  GetUniformArrayName( buffer, "shadowMaps[%u]", i )
#define UNIFORM_FS_SHADOWS_ENABLED "shadowsEnabled"
#define UNIFORM_FS_SHADOW_MAP "shadowMap"

#define UNIFORM_FS_MATERIAL_COLOR_DIFFUSE "material.diffuseColor"
#define UNIFORM_FS_MATERIAL_IS_TEXTURED "material.isTextured"
#define UNIFORM_FS_MATERIAL_TEXTURE "material.diffuseTexture"
#define UNIFORM_VSFS_MATERIAL_HAS_VERTEX_COLORS "material.hasVertexColors"

#define UNIFORM_FS_CAMERA_CLIP_NEAR "clipNear"
#define UNIFORM_FS_CAMERA_CLIP_FAR "clipFar"

#define UNIFORM_FS_BIAS "bias"

#define UNIFORM_FS_POSITION_MAP "positionMap"
#define UNIFORM_FS_NORMAL_MAP "normalMap"
#define UNIFORM_FS_AO_ENABLED "ssaoEnabled"
#define UNIFORM_FS_AO_MAP "ssaoMap"
#define UNIFORM_FS_AO_KERNEL_SCALE "kernelScale"
#define UNIFORM_FS_AO_KERNEL( buffer, i ) GetUniformArrayName( buffer, "kernelSamples[%u]", i )
#define UNIFORM_FS_AO_KERNEL_NOISE "kernelNoise"
#define UNIFORM_FS_AO_KERNEL_NOISE_SCALE "kernelNoiseScale"
#define UNIFORM_FS_AO_KERNEL_RADIUS "kernelRadius"
#define UNIFORM_FS_AO_BIAS UNIFORM_FS_BIAS
#define UNIFORM_FS_AO_GAUSSIAN "gaussian"

#define UNIFORM_VS_HAS_VERTEX_COLORS "hasVertexColors"

#define UNIFORM_RENDER_PASS "renderPass"


