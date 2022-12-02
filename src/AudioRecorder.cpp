#include "AudioRecorder.h"

#include <memory>

#include <flan/Audio.h>

AudioRecorder::AudioRecorder()
	: numRecords( 1 )
	, outputBuffer( nullptr )
    , sampleRate( 0 )
	, numChannels( 0 )
	{
	}

AudioRecorder::~AudioRecorder()
	{
	stopRecording();
	}

bool AudioRecorder::isRecording() const
	{
	return outputBuffer != nullptr;
	}

void AudioRecorder::startRecording()
	{
	if( isRecording() ) return Logger::writeToLog( "Error: recorder was already recording." );	
	if( numChannels == 0 ) return Logger::writeToLog( "Error: bad number of channels in audio recorder." );	
    if( sampleRate == 0 ) return Logger::writeToLog( "Error: bad sample rate in audio recorder." );	

	++numRecords;

	outputBuffer = std::make_unique<std::vector<std::vector<float>>>();
	outputBuffer->resize( numChannels );
	}

std::unique_ptr<flan::Audio> AudioRecorder::stopRecording()
	{
	if( ! isRecording() ) return std::unique_ptr<flan::Audio>( nullptr );

	flan::Audio::Format format;
	format.numChannels = numChannels;
	format.numFrames = (*outputBuffer)[0].size();
	format.sampleRate = sampleRate;
	auto out = std::make_unique<flan::Audio>( format );

	// The buffer being recorded to had an unknown size as recoding happened, so a copy is required here
	for( int i = 0; i < outputBuffer->size(); ++i )
		{
		const std::vector<float> & channel = (*outputBuffer)[i];
		std::copy( channel.begin(), channel.end(), out->begin() + i * format.numFrames );
		}

	outputBuffer.reset( nullptr );

	return std::move( out );
	}

int AudioRecorder::getNumRecords() const
	{
	return numRecords;
	}

void AudioRecorder::audioDeviceAboutToStart( AudioIODevice * device ) 
	{
	numChannels = static_cast<int>( device->getActiveInputChannels().toInt64() );
	sampleRate = device->getCurrentSampleRate();
	}

void AudioRecorder::audioDeviceStopped() 
	{
	numChannels = 0;
	sampleRate = 0;
	}

void AudioRecorder::audioDeviceIOCallback( const float ** inputChannelData, int numInputChannels, float ** outputChannelData, int numOutputChannels, int numNewFrames ) 
	{
	if( outputBuffer )
		{
		// Copy inputChannelData into outputBuffer
		for( int i = 0; i < numInputChannels; ++i )
			{
			const float * inChannel = inputChannelData[i];
			std::vector<float> & outChannel = (*outputBuffer)[i];
			int currentNumFrames = outChannel.size();
			outChannel.resize( currentNumFrames + numNewFrames );
			std::copy( inChannel, inChannel + numNewFrames, outChannel.begin() + currentNumFrames );
			}
		}

	// We need to clear the output buffers in case they're full of junk
	for( int i = 0; i < numOutputChannels; ++i )
		if( outputChannelData[i] != nullptr )
			FloatVectorOperations::clear( outputChannelData[i], numNewFrames );
	}

