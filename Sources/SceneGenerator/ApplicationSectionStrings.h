#pragma once

static const uint AlgID_RcActivation				= 0x16400;
static const uint AlgID_RcPointCloudReconstruction	= 0x16401;
static const uint AlgID_RcComponentLoad				= 0x16402;
static const uint AlgID_RcMeshGeneration			= 0x16403;
static const uint AlgID_PointCloudFiltering			= 0x16404;
static const uint AlgID_PointCloudDehomogenize		= 0x16405;
static const uint AlgID_ViewerInitialization		= 0x16406;
static const uint AlgID_ViewerDefaultSceneLoad		= 0x16407;
static const uint AlgID_ViewerDataConversion		= 0x16408;

bool LoadProgressString( __in const uint id, __out_ecount( 256 ) char* pStr );