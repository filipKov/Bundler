#pragma once

namespace Bundler { namespace Async {

	enum class WorkerThreadType
	{
		CPU,
		GPU
	};
	
	struct WorkerThreadInfo
	{
		WorkerThreadType type;
	};

} }