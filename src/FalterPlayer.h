#pragma once

#include <JuceHeader.h>

namespace flan { class Audio; }
class FalterClip;

class FalterPlayer : public AudioAppComponent
{
public:
	FalterPlayer();
	~FalterPlayer();

	AudioFormatManager & getFormatManager();
	AudioTransportSource & getTransportSource();

	void prepareToPlay( int samplesPerBlockExpected, double sample_rate ) override;
    void getNextAudioBlock( const AudioSourceChannelInfo& bufferToFill ) override;
    void releaseResources() override;
	//bool isPlaying() const { return transportSource->isPlaying(); }

	void playAudio( FalterClip * clip );
	void stop();

	bool isActiveClip( const FalterClip * );
	void deactivateClip( const FalterClip * );

private:
	std::unique_ptr<AudioFormatManager> formatManager;
	std::unique_ptr<AudioTransportSource> transportSource;
	FalterClip * activeClip;
};