#include "AltarButton.h"

#include "FalterLookAndFeel.h"

AltarButton::AltarButton( const String & _text, const Font * _font, size_t _fontHeight )
	: Button( _text )
	, font(_font)
	, circle( false )
	{
	setButtonText(_text);
	if( _fontHeight != 0 )
		{
		fontHeight = _fontHeight;
		}
	else
		fontHeight = font->getHeight();
	}


void AltarButton::paintButton(Graphics &g, bool isMouseOverButton, bool isButtonDown)
	{
	auto & lnf = FalterLookAndFeel::getLNF();

	if( circle )
		{
		auto r = getBounds();
		g.setColour( isMouseOverButton ? lnf.accent1 : lnf.accent2 );
		g.fillEllipse( 0, 0, r.getWidth() - 1, r.getHeight() - 1 );
		}
	else
		{
		g.fillAll( isMouseOverButton ? lnf.accent1 : lnf.accent2 );
		}

	if( font == nullptr ) return;
	Font &f = Font( *font );
	f.setHeight( fontHeight );
	if( isButtonDown ) f.setHeight( f.getHeight() * 0.9f );
	g.setFont( f );

	g.setColour( lnf.light );
	g.drawText( getButtonText(),
		Rectangle<int>( 0, 0, getWidth(), getHeight()), Justification::centred );

	if( isEnabled() ) setMouseCursor( MouseCursor::PointingHandCursor );
}

void AltarButton::setFont( Font *f )
	{
	font = f;
	}