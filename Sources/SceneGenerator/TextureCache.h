#pragma once

class TextureCache
{
public:

	bool RegisterItem( __in_z const char* pId, __in const Texture& tex );
	bool RegisterItem( __in_z const std::string& id, __in const Texture& tex );

	bool IsCached( __in_z const char* pId ) const;
	bool IsCached( __in const std::string& id ) const;

	bool RemoveItem( __in_z const char* pId );
	bool RemoveItem( __in const std::string& id );

	void Clear();

	bool Get( __in_z const char* pId, __deref_out const Texture** ppTex ) const;
	bool Get( __in_z const std::string& id, __deref_out const Texture** ppTex ) const;

	bool Get( __in_z const char* pId, __deref_out Texture** ppTex );
	bool Get( __in_z const std::string& id, __deref_out Texture** ppTex );

protected:

	std::map< std::string, Texture > mMap;

};

