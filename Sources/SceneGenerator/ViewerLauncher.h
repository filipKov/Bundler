#pragma once
class ViewerLauncher
{
public:

	// static void InitializeSparsePointCloud(
	// 	__in const uint indexCount,
	// 	__in_ecount( indexCount ) const uint* pVertexIndices,
	// 	__in const CapturingReality::Sfm::SfmReconstructionPoint* pVertices,
	// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox );
	// 
	// static void InitializeDensePointCloud(
	// 	__in const uint vertexCount,
	// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
	// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
	// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox );
	// 
	// static void InitializeUntexturedModel(
	// 	__in const uint vertexCount,
	// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
	// 	__in const uint faceCount,
	// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
	// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox );
	// 
	// static void InitializeColorizedModel(
	// 	__in const uint vertexCount,
	// 	__in_ecount( vertexCount ) const CapturingReality::LocalPoint* pVertices,
	// 	__in_ecount( vertexCount ) const DirectX::XMVECTOR* pVertexColors,
	// 	__in const uint faceCount,
	// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
	// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox );
	// 
	// static void InitializeTexturedModel(
	// 	__in const uint vertexCount,
	// 	__in_ecount( 3 * vertexCount ) const float* pVertices,
	// 	__in_ecount( vertexCount ) const TextureCoordinate* pVertexTextureCoords,
	// 	__in const uint faceCount,
	// 	__in_ecount( faceCount ) const CapturingReality::Geometry::Triangle* pFaces,
	// 	__in CapturingReality::IImageBuffer* pTexture,
	// 	__in const CapturingReality::Mvs::GlobalReconstructionVolume& bbox );

	static void InitializeEmpty( );

	static void InitializePointCloud (
		__in const uint pointCount,
		__in_ecount( pointCount ) const Bundler::Vector3* pPoints,
		__in_ecount_opt( pointCount ) const uint* pPointColors );

	static void LaunchViewer();
	
};

