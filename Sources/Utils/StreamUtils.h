#pragma once

namespace Utils {

		template < class streamType >
		streamType OpenStreamOnFile( __in const char* pFilename ) {
			streamType stream( pFilename );
			if ( !stream.good() ) {
				throw InvalidArgumentException( GET_VARIABLE_NAME( pFilename ), ExceptionReasons::INVALID_FILE );
			}

			return stream;
		};

}