#include "FalterButton.h"

#include "FalterLookAndFeel.h"

FalterButton::FalterButton( const String & _text, const Font * _font, size_t _fontHeight )
	: Button( _text )
	, font(_font)
	, circle( false )
	, down_text( _text )
	, baseColour( FalterLookAndFeel::getLNF().green )
	, hoverColour( FalterLookAndFeel::getLNF().red )
	{
	setButtonText(_text);
	if( _fontHeight != 0 )
		{
		fontHeight = _fontHeight;
		}
	else
		fontHeight = font->getHeight();
	}


void FalterButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown)
	{
	auto & lnf = FalterLookAndFeel::getLNF();

	if( circle )
		{
		auto r = getBounds();
		g.setColour( isMouseOverButton ? hoverColour : baseColour );
		g.fillEllipse( 0, 0, r.getWidth() - 1, r.getHeight() - 1 );
		}
	else
		{
		g.fillAll( isMouseOverButton ? hoverColour : baseColour );
		}

	if( font == nullptr ) return;
	Font && f = Font( *font );
	f.setHeight( fontHeight );
	if( isButtonDown ) f.setHeight( f.getHeight() * 0.9f );
	g.setFont( f );

	g.setColour( lnf.light );
	g.drawText( isButtonDown ? down_text : getButtonText(),
		Rectangle<int>( 0, 0, getWidth(), getHeight()), Justification::centred );

	if( isEnabled() ) setMouseCursor( MouseCursor::PointingHandCursor );
}

void FalterButton::setFont( Font *f )
	{
	font = f;
	}