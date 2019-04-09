#pragma once

namespace Bundler { namespace Async {

	class ITask
	{
	public:

		virtual ~ITask( )
		{

		}

		virtual void Execute( ) = 0;

	};

} }