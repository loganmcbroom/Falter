#pragma once 

#include <JuceHeader.h>

#include "AltarButton.h"

class AltarLogger : public Logger
				  , public TextEditor
				  , public Button::Listener
{
public:
	AltarLogger();
	~AltarLogger();

	void logMessage( const String & message ) override;
	void paint( Graphics & g ) override;
	void resized() override;
	void buttonClicked( Button * b ) override;
	
	AltarButton clearButton;
};