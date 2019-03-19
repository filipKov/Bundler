#pragma once

namespace Bundler { namespace Import {

	class TextStreamReader
	{
	public:

		template < typename T >
		static HRESULT ReadValue( __inout std::istream* pStream, __out T* pValue )
		{
			if ( pStream->good() )
			{
				pStream->operator >> ( *pValue );
				return S_OK;
			}
			return E_FAIL;
		}

		template < size_t maxLength >
		static HRESULT ReadLine( __inout std::istream* pStream, __out_z char( &string )[ maxLength ] )
		{
			if ( pStream->good() )
			{
				pStream->getline( string, maxLength );
				return S_OK;
			}
			return E_FAIL;
		}

		template < size_t maxLength >
		static HRESULT ReadLine( __inout std::istream* pStream, __in const char delim, __out_z char( &string )[ maxLength ] )
		{
			if ( pStream->good() )
			{
				pStream->getline( string, maxLength, delim );
				return S_OK;
			}
			return E_FAIL;
		}

		static HRESULT ReadLine( __inout std::istream* pStream, __in const size_t maxLength, __out_z char* pString )
		{
			if ( pStream->good() )
			{
				pStream->getline( pString, maxLength );
				return S_OK;
			}
			return E_FAIL;
		}

		inline static HRESULT ReadChar( __inout std::istream* pStream, __out char* pValue )
		{
			if ( pStream->good() )
			{
				*pValue = (char)pStream->get();
				return S_OK;
			}
			return E_FAIL;
		}

		inline static HRESULT ReadInt16( __inout std::istream* pStream, __out int16* pValue )
		{
			return ReadValue( pStream, pValue );
		}

		inline static HRESULT ReadInt32( __inout std::istream* pStream, __out int32* pValue )
		{
			return ReadValue( pStream, pValue );
		}

		inline static HRESULT ReadInt64( __inout std::istream* pStream, __out int64* pValue )
		{
			return ReadValue( pStream, pValue );
		}


		inline static HRESULT ReadByte( __inout std::istream* pStream, __out byte* pValue )
		{
			if ( pStream->good() )
			{
				*pValue = (byte)pStream->get();
				return S_OK;
			}
			return E_FAIL;
		}

		inline static HRESULT ReadUint16( __inout std::istream* pStream, __out uint16* pValue )
		{
			return ReadValue( pStream, pValue );
		}

		inline static HRESULT ReadUint32( __inout std::istream* pStream, __out uint32* pValue )
		{
			return ReadValue( pStream, pValue );
		}

		inline static HRESULT ReadUint64( __inout std::istream* pStream, __out uint64* pValue )
		{
			return ReadValue( pStream, pValue );
		}


		inline static HRESULT ReadFloat32( __inout std::istream* pStream, __out float* pValue )
		{
			return ReadValue( pStream, pValue );
		}

		inline static HRESULT ReadFloat64( __inout std::istream* pStream, __out double* pValue )
		{
			return ReadValue( pStream, pValue );
		}

	};

} }
