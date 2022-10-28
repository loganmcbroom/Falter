#pragma once

#include <algorithm>

#include <JuceHeader.h>

#include "AltarButton.h"

class AltarClipList;
namespace flan { class Audio; }

class AltarClip : public Button // Button allows hover color
				, public ChangeListener
				, public Button::Listener
				, public Timer
{
	friend class AltarClipList;
public:
	AltarClip( std::shared_ptr<flan::Audio> audio
			 , AudioFormatManager & formatManager
			 , AudioThumbnailCache & thumbnailCache
			 , AudioTransportSource & transportSource
			 );

	~AltarClip();

	AudioThumbnail & getThumbnail();

	std::shared_ptr<flan::Audio> getAudio() { return audio; };

	void playPressed();
	void stopPressed();

	static AltarClip * active;

private:  
	void timerCallback();
	void changeListenerCallback( ChangeBroadcaster * source ) override;
	void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
	void buttonClicked( Button *button ) override;
	void resized() override;
	void mouseDrag( const MouseEvent & event ) override;

	AudioTransportSource & transportSource;
	AudioFormatManager & formatManager; 
	
	std::shared_ptr<flan::Audio> audio;
	std::vector<const float *> flanAudioChanPointers; // Needed to wrap flan buffer in Juce object
	AudioBuffer<float> juceAudio; // Juce buffer wrapper for flan Audio
	MemoryAudioSource audioSource; // Juce AudioSource wrapper for Juce buffer

	AudioThumbnail thumbnail;
	AltarButton busButton, saveButton;
	DrawableRectangle currentPosition;
}; 