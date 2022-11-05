#pragma once

#include <algorithm>

#include <flan/Audio.h>

#include <JuceHeader.h>

#include "FalterButton.h"

class FalterClipList;
namespace flan { class Audio; }

class FalterClip : public Button // Button allows hover color
				, public ChangeListener
				, public Button::Listener
				, public Timer
{
	friend class FalterClipList;
public:
	FalterClip( flan::Audio audio
			 , AudioFormatManager & formatManager
			 , AudioThumbnailCache & thumbnailCache
			 , AudioTransportSource & transportSource
			 );

	~FalterClip();

	AudioThumbnail & getThumbnail();

	flan::Audio getAudio() { return audio; };

	void playPressed();
	void stopPressed();

	static FalterClip * active;

private:  
	void timerCallback();
	void changeListenerCallback( ChangeBroadcaster * source ) override;
	void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
	void buttonClicked( Button *button ) override;
	void resized() override;
	void mouseDrag( const MouseEvent & event ) override;

	AudioTransportSource & transportSource;
	AudioFormatManager & formatManager; 
	
	flan::Audio audio;
	std::vector<const float *> flanAudioChanPointers; // Needed to wrap flan buffer in Juce object
	AudioBuffer<float> juceAudio; // Juce buffer wrapper for flan Audio
	MemoryAudioSource audioSource; // Juce AudioSource wrapper for Juce buffer

	AudioThumbnail thumbnail;
	FalterButton busButton, saveButton;
	DrawableRectangle currentPosition;
}; 