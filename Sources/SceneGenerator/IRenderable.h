#pragma once

class IRenderable {
public:
	
	virtual void Render( __in const IRenderingContext* pContext ) const = 0;

};