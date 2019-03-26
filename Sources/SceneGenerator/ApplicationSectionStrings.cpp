#include "stdafx.h"
#include "ApplicationSectionStrings.h"

bool LoadProgressString( __in const uint id, __out_ecount( 256 ) char* pStr ) {
	const char* pSrcStr = "unknown";

	switch ( id )
	{
	case AlgID_RcActivation:
		pSrcStr = "Activating RealityCapture Engine";
		break;
	case AlgID_RcPointCloudReconstruction:
		pSrcStr = "Aligning Images";
		break;
	case AlgID_RcComponentLoad:
		pSrcStr = "Loading RealityCapture Component";
		break;
	case AlgID_RcMeshGeneration:
		pSrcStr = "Creating Mesh";
		break;
	case AlgID_PointCloudFiltering:
		pSrcStr = "Filtering Point Cloud";
		break;
	case AlgID_PointCloudDehomogenize:
		pSrcStr = "Dehomogenizing Points";
		break;
	case AlgID_ViewerInitialization:
		pSrcStr = "Initializing Viewer";
		break;
	case AlgID_ViewerDefaultSceneLoad:
		pSrcStr = "Loading Default Scene";
		break;
	case AlgID_ViewerDataConversion:
		pSrcStr = "Converting Data";
		break;
	default:
		return false;
	}

	strcpy_s( pStr, 255, pSrcStr );
	return true;
}