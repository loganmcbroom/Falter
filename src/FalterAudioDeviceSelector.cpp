#include "FalterAudioDeviceSelector.h"

#include "FalterLookAndFeel.h"

FalterAudioDeviceSelector::FalterAudioDeviceSelector( AudioDeviceManager & manager )
	: AudioDeviceSelectorComponent( manager, 1, 2, 1, 2, false, false, true, false )
	{}

void FalterAudioDeviceSelector::paint( Graphics & g ) 
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	}