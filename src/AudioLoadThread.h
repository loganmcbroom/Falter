#pragma once

#include <JuceHeader.h>

#include "Types.h"

//using AudioLoadThreadCallback = std::function<void ( const flan::Audio & )>;

struct AudioLoadThread : public ThreadPoolJob
{
	AudioLoadThread( const File & file );

	JobStatus runJob() override;

	std::unique_ptr<flan::Audio> output;
	const File file;
};