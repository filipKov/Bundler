#pragma once

// #include <WinBase.h>
// 
// struct _declspec( novtable ) _declspec( uuid( "{FA3F6B33-E8C8-4161-ACB0-D6093AFD117A}" ) ) IUnknown {
// 
// 	virtual ULONG AddReference() = 0;
// 	virtual ULONG Release() = 0;
// 	virtual void QueryInterface( const GUID& interfaceId, __deref_out void** ppInterface ) = 0;
// };
// 
// #define DECLARE_INTERFACE( name, base, guid ) struct _declspec( novtable ) _declspec( uuid( guid ) ) name: public base
// 
// template < class T >
// class _declspec(novtable) SmartObjectBase : T {
// 
// 	virtual ULONG AddReference() = 0;
// 	virtual ULONG Release() = 0;
// 	virtual void QueryInterface( const GUID& interfaceId, __deref_out void** ppInterface ) = 0;
// 
// };
// 
// template < class T >
// class SmartObject : SmartObjectBase< T > {
// public:
// 
// 	ULONG AddReference() override {
// 		return InterlockedIncrement( &m_referenceCount );
// 	}
// 
// 	ULONG Release() override {
// 		InterlockedDecrement( &m_referenceCount );
// 		if ( m_referenceCount == 0 ) {
// 			delete this;
// 		}
// 	}
// 
// 	void QueryInterface( const GUID& interfaceId, __deref_out void** ppInterface ) override {
// 		UNREFERENCED_PARAMETER( interfaceId );
// 		UNREFERENCED_PARAMETER( ppInterface );
// 		throw 9;
// 	}
// 
// 	// template < class Interface, typename ... ArgTypes >
// 	// Interface* CreateInstance( ArgTypes ... args )
// 	// {
// 	// 	
// 	// }
// 
// protected:
// 
// 	volatile ULONG m_referenceCount;
// 
// };

