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

void FalterPlayer::prepareToPlay( int samplesPerBlockExpected, double sample_rate )
	{
	transportSource->prepareToPlay( samplesPerBlockExpected, sample_rate );
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

	transportSource->setSource( &clip->audioSource, 0, nullptr, clip->audio->get_sample_rate(), clip->audio->get_num_channels() );
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

bool FalterPlayer::isActiveClip( const FalterClip * clip )
	{
	return activeClip == clip;
	}

void FalterPlayer::deactivateClip( const FalterClip * clip )
	{
	if( clip == activeClip )
		{
		stop();
		}
	}