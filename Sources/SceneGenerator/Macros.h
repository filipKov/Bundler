#pragma once

#define RESOURCES_DIR_RELATIVE "Resources/"
#define GET_RESOURCE_PATH( filename ) RESOURCES_DIR_RELATIVE filename
#define GET_SHADER_PATH( shaderFile ) RESOURCES_DIR_RELATIVE "Shaders/" shaderFile
#define GET_MODEL_PATH( modelFile ) RESOURCES_DIR_RELATIVE "Models/" modelFile
#define GET_TEXTURE_PATH( textureFile ) RESOURCES_DIR_RELATIVE "Textures/" textureFile

#define MAX_COMPONENT_COUNT 512

#define WINDOW_720P_W 1280
#define WINDOW_720P_H 720
#define WINDOW_1080P_W 1920
#define WINDOW_1080P_H 1080
#define WINDOW_2048 2048

#define DEFAULT_WINDOW_WIDTH WINDOW_720P_W
#define DEFAULT_WINDOW_HEIGHT WINDOW_720P_H