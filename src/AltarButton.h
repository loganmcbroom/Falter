#pragma once

#include <JuceHeader.h>

class AltarButton : public Button
{
public:
	AltarButton( const String & text = "", const Font * font = nullptr, size_t fontHeight = 0 );

	void paintButton( Graphics &g, bool isMouseOverButton, bool isButtonDown) override;

	void setFont( Font *f );

	const Font * font;
	size_t fontHeight;
	bool circle;
};

