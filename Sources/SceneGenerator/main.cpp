// SceneGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <map>
#include <string>

#include <thread>

#include "GL/glew.h"
#include "GL/glut.h"

#include "AlgebraLib.h"
#include "Camera.h"
#include "RenderableObject.h"
#include "GlRenderer.h"

#include "Mouse.h"
#include "CameraController.h"

#include "SceneVisualiser.h"
using namespace Visualiser;


int main()
{
	try
	{
		SceneVisualiser visualiser( 1280, 720 );
		visualiser.Init();
	}
	catch ( const Exception& error )
	{
		printf( "An error has occured!:\n\t%s\n", error.GetMessage() );
	}

	
    return 0;
}

