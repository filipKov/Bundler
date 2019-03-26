#include "stdafx.h"
#include <map>
#include <string>

#include <glew.h>
#include "Texture.h"

#include "TextureCache.h"


bool TextureCache::RegisterItem( __in_z const char* pId, __in const Texture& tex ) {
	std::string idStr( pId );
	return RegisterItem( idStr, tex );
}

bool TextureCache::RegisterItem( __in_z const std::string& id, __in const Texture& tex ) {
	if ( !IsCached( id ) ) {
		mMap.emplace( id, tex );
		return true;
	}

	return false;
}

bool TextureCache::IsCached( __in_z const char* pId ) const {
	std::string idStr( pId );
	return IsCached( idStr );
}

bool TextureCache::IsCached( __in const std::string& id ) const {
	return mMap.find( id ) != mMap.end();
}

bool TextureCache::RemoveItem( __in_z const char* pId ) {
	std::string idStr( pId );
	return RemoveItem( idStr );
}

bool TextureCache::RemoveItem( __in const std::string& id ) {
	if ( IsCached( id ) ) {
		mMap[ id ].Release();
		mMap.erase( id );
		return true;
	}
	return false;
}

void TextureCache::Clear() {
	for ( auto it = mMap.begin(); it != mMap.end(); it++ ) {
		it->second.Release();
	}

	mMap.clear();
}

bool TextureCache::Get( __in_z const char* pId, __deref_out const Texture** ppTex ) const {
	std::string idStr( pId );
	return Get( idStr, ppTex );
}

bool TextureCache::Get( __in_z const std::string& id, __deref_out const Texture** ppTex ) const {
	auto pPair = mMap.find( id );
	if ( pPair != mMap.end() ) {
		*ppTex = &pPair->second;
		return true;
	}
	*ppTex = nullptr;
	return false;
}

bool TextureCache::Get( __in_z const char* pId, __deref_out Texture** ppTex ) {
	std::string idStr( pId );
	return Get( idStr, ppTex );
}

bool TextureCache::Get( __in_z const std::string& id, __deref_out Texture** ppTex ) {
	auto pPair = mMap.find( id );
	if ( pPair != mMap.end() ) {
		*ppTex = &pPair->second;
		return true;
	}
	*ppTex = nullptr;
	return false;
}
