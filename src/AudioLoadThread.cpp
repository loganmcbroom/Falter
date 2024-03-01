#include "AudioLoadThread.h"

#include <flan/Audio/Audio.h>

AudioLoadThread::AudioLoadThread( const File & _file )
	: ThreadPoolJob( _file.getFullPathName() )
	, file( _file )
	, output( nullptr )
	{
	}

ThreadPoolJob::JobStatus AudioLoadThread::runJob() 
	{
	const flan::Second maxLength = 10 * 60; // Ten minute max

	// // Check that input length isn't too long
	// juce::File input( file.getFullPathName() );
	// juce::AudioFormatManager formatManager;
	// formatManager.registerBasicFormats();
	// if( auto reader = std::make_unique<juce::AudioFormatReader>( formatManager.createReaderFor( input ) ) )
	// 	{
	// 	const flan::Second length = reader->lengthInSamples / reader->sampleRate;
	// 	if( length > maxLength )
	// 		{
	// 		signalJobShouldExit();
	// 		return jobHasFinished;
	// 		}
	// 	}

	output = std::make_unique<flan::Audio>( file.getFullPathName().toStdString() );
	signalJobShouldExit();
	return jobHasFinished;
	}
