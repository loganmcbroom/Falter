#include "FalterPlayer.h"

#include "FalterClip.h"

FalterPlayer::FalterPlayer()
	: formatManager( std::make_unique<AudioFormatManager>() )
	, transportSource( std::make_unique<AudioTransportSource>() )
	, activeClip( nullptr )
	{
	formatManager->registerBasicFormats();
	setAudioChannels( 0, 2 );
	}

FalterPlayer::~FalterPlayer()
	{
	shutdownAudio();
	}

AudioFormatManager & FalterPlayer::getFormatManager()
	{
	return *formatManager.get();
	}

AudioTransportSource & FalterPlayer::getTransportSource()
	{
	return *transportSource.get();
	}

void FalterPlayer::prepareToPlay( int samplesPerBlockExpected, double sampleRate )
	{
	transportSource->prepareToPlay( samplesPerBlockExpected, sampleRate );
	}

void FalterPlayer::getNextAudioBlock( const AudioSourceChannelInfo & bufferToFill )
	{
	transportSource->getNextAudioBlock( bufferToFill );
	}

void FalterPlayer::releaseResources()
	{
	transportSource->releaseResources();
	}

void FalterPlayer::playAudio( FalterClip * clip )
	{
	if( activeClip ) 
		{
		activeClip->setToggle( false );
		transportSource->removeChangeListener( activeClip );
		}
	activeClip = clip;

	transportSource->setSource( &clip->audioSource, 0, nullptr, clip->audio.getSampleRate(), clip->audio.getNumChannels() );
	transportSource->start();
	transportSource->addChangeListener( clip );
	}

void FalterPlayer::stop()
	{
	if( activeClip )
		transportSource->removeChangeListener( activeClip );
	transportSource->stop();
	transportSource->setSource( nullptr );
	activeClip = nullptr;
	}

void FalterPlayer::deactivateClip( const FalterClip * clip )
	{
	if( clip == activeClip )
		{
		stop();
		}
	}