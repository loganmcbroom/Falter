#include "AudioLoadThread.h"

#include <flan/Audio.h>

AudioLoadThread::AudioLoadThread( const File & _file )
	: ThreadPoolJob( _file.getFullPathName() )
	, file( _file )
	, output( nullptr )
	{
	}

ThreadPoolJob::JobStatus AudioLoadThread::runJob() 
	{
	output = std::make_unique<flan::Audio>( file.getFullPathName().toStdString() );
	signalJobShouldExit();
	return jobHasFinished;
	}
