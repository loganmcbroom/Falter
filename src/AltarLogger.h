#pragma once 

#include <JuceHeader.h>

#include "AltarButton.h"

class AltarLogger : public Logger
				  , public TextEditor
				  , public Button::Listener
				  , public std::streambuf
{
public:
	AltarLogger();
	~AltarLogger();

	void logMessage( const String & message ) override;
	void paint( Graphics & g ) override;
	void resized() override;
	void buttonClicked( Button * b ) override;
	
	AltarButton clearButton;

protected:
	std::streamsize xsputn( const char_type * s, std::streamsize n ) override;
	int_type overflow( int_type ch = std::char_traits<char_type>::eof() ) override;
	
	std::streambuf * oldStreamBuffer;
};