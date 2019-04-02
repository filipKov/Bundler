#pragma once

namespace Bundler { namespace Async {

	class ITask
	{
	public:

		virtual void Execute( ) = 0;

	};

} }