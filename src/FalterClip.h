#pragma once

#include <algorithm>

#include <flan/Audio.h>

#include <JuceHeader.h>

#include "FalterButton.h"

class FalterClipList;
class FalterPlayer;

class FalterClip : public Button // Button allows hover color
				, public ChangeListener
				, public Button::Listener
				, public Timer
{
friend class FalterClipList;
friend class FalterPlayer;

public:
	FalterClip( flan::Audio audio
			 , FalterPlayer & player
			 , AudioThumbnailCache & thumbnailCache
			 );
	~FalterClip();

	AudioThumbnail & getThumbnail();

	flan::Audio getAudio() { return audio; };

	void playPressed();
	void stopPressed();
	void setToggle( bool );

private:  
	void timerCallback() override;
	void changeListenerCallback( ChangeBroadcaster * source ) override;
	void paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown) override;
	void buttonClicked( Button *button ) override;
	void resized() override;
	void mouseDrag( const MouseEvent & event ) override;

	FalterPlayer & player;
	
	flan::Audio audio;
	std::vector<const float *> flanAudioChanPointers; // Needed to wrap flan buffer in Juce object
	AudioBuffer<float> juceAudio; // Juce buffer wrapper for flan Audio
	MemoryAudioSource audioSource; // Juce AudioSource wrapper for Juce buffer

	AudioThumbnail thumbnail;
	FalterButton busButton, saveButton;
	DrawableRectangle currentPosition;
}; 