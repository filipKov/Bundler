#pragma once

class IModifiableMesh {
public:

	virtual void SetPartsCount( __in const uint count ) = 0;

	virtual SceneMeshPart* GetPartPtr( __in const uint partIndex ) = 0;

};

class SceneMesh : public SceneObject, public IRenderable, public IModifiableMesh {

public:

	void Render( __in const IRenderingContext* pContext ) const override;

	BoundingBox GetBoundingBox() const;

	// Parts
	size_t GetPartCount() const;

	const SceneMeshPart* GetPartPtr( __in const uint partIndex ) const;

	// SceneObject
	IRenderable* GetRenderable() override;

	// IMeshModifier
	void SetPartsCount( __in const uint count ) override;

	SceneMeshPart* GetPartPtr( __in const uint partIndex ) override;

protected:

	Utils::Containers::PagedVector< SceneMeshPart, 5 > mParts;

};