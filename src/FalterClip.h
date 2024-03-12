#pragma once

#include <algorithm>

#include <flan/Audio/Audio.h>

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
	FalterClip( std::shared_ptr<flan::Audio> audio
			 , FalterPlayer & player
			 , AudioThumbnailCache & thumbnailCache
			 , const String & name = "-"
			 , const File & global_file = File()
			 , flan::Audio::SndfileStrings = flan::Audio::SndfileStrings()
			 );
	~FalterClip() override;

	AudioThumbnail & getThumbnail();

	std::shared_ptr<flan::Audio> getAudio() { return audio; };

	void playPressed();
	void stopPressed();
	void setToggle( bool );

	File getWorkspaceFile() const;
	File getGlobalFile() const;
	flan::Audio::SndfileStrings getSndFileStrings();

private:  
	void timerCallback() override;
	void changeListenerCallback( ChangeBroadcaster * source ) override;
	void paintButton( Graphics & g, bool isMouseOverButton, bool isButtonDown ) override;
	void buttonClicked( Button * button ) override;
	void resized() override;
	void mouseDrag( const MouseEvent & event ) override;

	FalterPlayer & player;
	
	std::shared_ptr<flan::Audio> audio;
	std::vector<const float *> flanAudioChanPointers; // Needed to wrap flan buffer in Juce object
	AudioBuffer<float> juceAudio; // Juce buffer wrapper for flan Audio
	MemoryAudioSource audioSource; // Juce AudioSource wrapper for Juce buffer

	AudioThumbnail thumbnail;
	FalterButton busButton, saveButton, retrieveScriptButton, retrieveInputsButton;
	DrawableRectangle currentPosition;

	void saveButtonPressed(); // If the file is empty, create the file and save to it
	File local_file; // Local file stored in workspace folder
	File global_file;
	juce::Uuid id;

	void retrieveScriptButtonPressed() const;
	void retrieveInputsButtonPressed() const;
	flan::Audio::SndfileStrings sndfileStrings;
}; 