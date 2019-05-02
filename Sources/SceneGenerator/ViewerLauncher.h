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

	static void ShowBundle( 
		__in const Bundler::Bundle* pBundle,
		__in_opt const Bundler::BundleAdditionalPayload* pMetadata,
		__in_ecount_opt( 3 ) float* pPivot );

	static void LaunchViewer();
	
};

