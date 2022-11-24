#include "FalterClip.h"

#include <Windows.h>

#include <flan/Audio.h>

#include "FalterClipList.h"
#include "FalterLookandFeel.h"
#include "FalterPlayer.h"

static std::vector<const float *> getFlanChanPointers( flan::Audio a )
	{
	std::vector<const float *> ps;
	for( flan::Channel channel = 0; channel < a.getNumChannels(); ++channel )
		{
		ps.emplace_back( a.getSamplePointer( channel, 0 ) );
		}
	return ps;
	}

FalterClip::FalterClip( flan::Audio _audio
			, FalterPlayer & _player
			, AudioThumbnailCache &_thumbnailCache
			, const String & name
			) 
	: Button( name )
	, player( _player )
	, audio( _audio )
	, flanAudioChanPointers( getFlanChanPointers( audio ) )
	, juceAudio( (float * const *) &flanAudioChanPointers[0], audio.getNumChannels(), audio.getNumFrames() )
	, audioSource( juceAudio, false )
	, thumbnail( 512, player.getFormatManager(), _thumbnailCache )
	, busButton  ( "4", &FalterLookAndFeel::getLNF().fontWebdings, 18 ) 
	, saveButton ( "<", &FalterLookAndFeel::getLNF().fontWingdings, 18 )
	{
	auto & lnf = FalterLookAndFeel::getLNF();

	// Waveform thumbnail setup
	thumbnail.addChangeListener( this );
	thumbnail.reset( audio.getNumChannels(), audio.getSampleRate(), audio.getNumFrames() );
	thumbnail.addBlock( 0, juceAudio, 0, audio.getNumFrames() );

	addAndMakeVisible( busButton   );
	addAndMakeVisible( saveButton  );

	saveButton.addListener( this );
	busButton .addListener( this );

	const int clipGroupID = 1;
	setRadioGroupId( clipGroupID );

	currentPosition.setFill( lnf.light.withAlpha( 0.85f ) );
    addAndMakeVisible( currentPosition );
	}


FalterClip::~FalterClip()
	{
	}

void FalterClip::resized()
	{
	int s = getHeight() / 2;
	busButton.setBounds  ( 0, 0, s, s );
	saveButton.setBounds ( 0, s, s, s );
	}

void FalterClip::mouseDrag( const MouseEvent & )
	{
	DragAndDropContainer * dragC = DragAndDropContainer::findParentDragContainerFor( this );
	if (! dragC->isDragAndDropActive() ) 
		{
		dragC->startDragging( "Clip" , this );
		}
	}

AudioThumbnail &FalterClip::getThumbnail() { return thumbnail; }

void FalterClip::paintButton(Graphics &g, bool isMouseOverButton, bool )
	{
	auto & lnf = FalterLookAndFeel::getLNF();

	g.fillAll( lnf.shadow );
	
	if( audio.getNumFrames() > 0 )
		{
		// Draw audio thumbnail
		g.setColour( isMouseOverButton? lnf.accent1.withAlpha( .3f ) : lnf.accent1 );
		juce::Rectangle<int> rect( getLocalBounds().withLeft( getHeight() / 2 ) );
		thumbnail.drawChannels( g, rect.reduced( 2 ), 0, thumbnail.getTotalLength(), 1.0f );

		if( isMouseOverButton )
			{
			g.setFont( lnf.fontMonospace );

			g.setColour( lnf.light );
			const auto rect = juce::Rectangle<int>( int( getHeight() / 2.0f ), 0, int( getWidth() ), getHeight() ).reduced( 3 );

			// Write audio length
			auto r2dec = []( float x ){ return std::round( x * 1000 ) / 1000.0f; };
			const float length = audio.getLength();
			String lengthText = length > 60?
				String( r2dec( length / 60.0f ) ) + " min" :
				String( r2dec( length ) ) + " sec";
			g.drawText( lengthText, rect, Justification::bottomLeft );

			// write audio name
			g.drawText( getName(), rect, Justification::topLeft );
			}
		}
	}

void FalterClip::changeListenerCallback( ChangeBroadcaster * source )
	{
	if( source == &player.getTransportSource() && player.getTransportSource().hasStreamFinished() ) 
		{
		stopPressed();
		}
	else if( source == &thumbnail ) 
		repaint();
	}

void FalterClip::playPressed()
	{
	setToggle( true );
	player.playAudio( this );
	}

void FalterClip::stopPressed()
	{
	setToggle( false );
	player.stop();
	}

void FalterClip::setToggle( bool playMode )
	{
	setToggleState( playMode, NotificationType::dontSendNotification );
	if( playMode )
		{
		startTimer( 33 );
		busButton.setButtonText( "n" );
		busButton.font = &FalterLookAndFeel::getLNF().fontWingdings;
		}
	else
		{
		stopTimer();
		busButton.setButtonText( "4" );
		busButton.font = &FalterLookAndFeel::getLNF().fontWebdings;
		currentPosition.setRectangle( juce::Rectangle< float >(0,0,0,0) );
		}
	}

void FalterClip::buttonClicked( Button * button )
	{
	if( button == &busButton )
		{
		if( ! getToggleState() ) playPressed();
		else stopPressed();
		}

	else if( button == &saveButton )
		{
		FileChooser chooser( "Save file as", File("C:/Users/logan/Documents/Samples"), ".wav" );

		if( chooser.browseForFileToSave( true ) )
			{
			File choice = chooser.getResult();
			audio.save( choice.getFullPathName().toStdString() );
			}
		setName( chooser.getResult().getFileName() );
		repaint();
		}
	}

void FalterClip::timerCallback()
	{
	auto & source = player.getTransportSource();

	// The weird computation handles some buttons being stuck on top of the component
	const float s = getHeight() / 2.0f;
	const float e = getWidth();
	const float r = source.getCurrentPosition() / audio.getLength();
	const float initialX = s + ( e - s ) * r;
	
	// Draw white line to show current playback position
	currentPosition.setRectangle( juce::Rectangle<float>( 
		initialX,
		0.0f, 
		getToggleState() ? 2.0f : 0.0f, 
		float( getHeight() ) ) );
	}
