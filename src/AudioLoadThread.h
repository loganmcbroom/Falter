#pragma once

#include <JuceHeader.h>

#include <flan/Audio/Audio.h>

//using AudioLoadThreadCallback = std::function<void ( const flan::Audio & )>;

struct AudioLoadThread : public ThreadPoolJob
{
	AudioLoadThread( const File & file );

	JobStatus runJob() override;

	std::unique_ptr<flan::Audio> output;
	flan::AudioBuffer::SndfileStrings strings;

	const File file;
};