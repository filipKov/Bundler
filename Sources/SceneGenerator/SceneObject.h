#pragma once

// struct ObjectLoadParams {
// 
// 	const char* pMeshFile;
// 
// 	uint textureCount;
// 	const char* diffuseTextures[MAX_COMPONENT_COUNT];
// 
// 
// };
// 
// class SceneObject
// {
// public:
// 
// 	void Load( __in ObjectLoadParams& params, __inout TextureCache& texCache );
// 	
// 	const Vector4f& GetPivotPoint() const;
// 
// 	const uint GetComponentCount() const;
// 
// 	const ObjectComponent& GetComponent( __in const uint ix ) const;
// 
// 	
// 	// Manipulation
// 	void Move( __in const Vector3f& translation );
// 	
// 	void MoveTo( __in const Vector3f& position );
// 
// 	void Rotate( __in const float angle, __in const Vector3f& rotVector );
// 
// 	void Scale( __in const Vector3f& scale );
// 
// 
// 	// Rendering + Shaders
// 	void RegisterGlobalShader( __in const GLuint shaderId );
// 
// protected:
// 
// 	Vector4f mGlobalPivot;
// 
// 	std::vector< ObjectComponent > mComponents;
// 
// };

class SceneObject : public Transformable {
public:

	virtual IRenderable* GetRenderable() = 0;

};

