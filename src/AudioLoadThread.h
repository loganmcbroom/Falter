#pragma once

#include <JuceHeader.h>

namespace flan { class Audio; }

//using AudioLoadThreadCallback = std::function<void ( const flan::Audio & )>;

struct AudioLoadThread : public ThreadPoolJob
{
	AudioLoadThread( const File & file );

	JobStatus runJob() override;

	std::unique_ptr<flan::Audio> output;
	const File file;
};