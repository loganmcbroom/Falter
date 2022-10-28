#include "AltarLogger.h"

#include "FalterLookandFeel.h"

AltarLogger::AltarLogger()
	: TextEditor( )
	, clearButton( "r", &FalterLookAndFeel::getLNF().fontWebdings, 20 )
	{
	setReadOnly( true );
	setCaretVisible( false );
	setMultiLine( true, true );

	clearButton.circle = true;
	addAndMakeVisible( &clearButton );
	clearButton.addListener( this );
	
	setFont( FalterLookAndFeel::getLNF().fontMonospace );
	}

AltarLogger::~AltarLogger() 
	{
	}

void AltarLogger::logMessage( const String & message )
	{
	// insertTextAtCaret( Time::getCurrentTime().formatted( "(%I:%M:%S) " ) );
	setCaretPosition( getText().length() );
	insertTextAtCaret( message + "\n" );

	// Make sure we don't go too crazy with this thing... A long program could output a ridiculous amount of text.
	// If it gets too long cut out half the text so we don't have to do it often
	if( getText().length() > 65536 ) 
		setText( getText().substring( getText().length() - 65536 / 2, false ) );
	}

void AltarLogger::paint( Graphics & g )
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	//TextEditor::paint( g );
	}

void AltarLogger::resized()
	{
	TextEditor::resized();
	const int l = 28;
	clearButton.setBounds( getWidth() - l - 4, 4, l, l );
	}

void AltarLogger::buttonClicked( Button * )
	{
	TextEditor::clear();
	}
