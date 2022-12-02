#pragma once

#include <JuceHeader.h>

class FalterAudioDeviceSelector : public AudioDeviceSelectorComponent
{
public:
	FalterAudioDeviceSelector( AudioDeviceManager & );
private:
	void paint( Graphics & g ) override;
};