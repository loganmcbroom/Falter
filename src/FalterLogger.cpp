#include "FalterLogger.h"

#include "FalterLookandFeel.h"

FalterLogger::FalterLogger()
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

	// redirect std::cout to the logger
	oldStreamBuffer = std::cout.rdbuf( this );
	}

FalterLogger::~FalterLogger() 
	{
	std::cout.rdbuf( oldStreamBuffer );
	}

void FalterLogger::logMessage( const String & message )
	{
	// insertTextAtCaret( Time::getCurrentTime().formatted( "(%I:%M:%S) " ) );
	setCaretPosition( getText().length() );
	juce::Thread * thread = Thread::getCurrentThread();
	if( thread )
		insertTextAtCaret( "{" + thread->getThreadName() + "} " + message + "\n" );
	else
		insertTextAtCaret( "{Main} " + message + "\n" );

	// Make sure we don't go too crazy with this thing... A long program could output a ridiculous amount of text.
	// If it gets too long cut out half the text so we don't have to do it often
	constexpr int maxLength = 65536;
	if( getText().length() > maxLength ) 
		setText( getText().substring( getText().length() - maxLength / 2, false ) );
	}

void FalterLogger::paint( Graphics & g )
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	//TextEditor::paint( g );
	}

void FalterLogger::resized()
	{
	TextEditor::resized();
	const int l = 28;
	clearButton.setBounds( getWidth() - l - 4, 4, l, l );
	}

void FalterLogger::buttonClicked( Button * )
	{
	TextEditor::clear();
	}

std::streamsize FalterLogger::xsputn( const char_type * s, std::streamsize n ) 
	{
	MessageManagerLock mml;
	logMessage( s );
	return n;
    }

// I don't know what this does but the base class streambuf defaults to a "failed to overflow" return value
// So if it isn't overloaded the cout redirect stops working
std::streambuf::int_type FalterLogger::overflow( int_type ch ) { return 0; }