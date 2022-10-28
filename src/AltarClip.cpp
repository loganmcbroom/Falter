#include "AltarClip.h"

#include <Windows.h>

#include <flan/Audio.h>

#include "AltarClipList.h"
#include "FalterLookandFeel.h"

AltarClip * AltarClip::active = nullptr;


static std::vector<const float *> getFlanChanPointers( std::shared_ptr<flan::Audio> a )
	{
	std::vector<const float *> ps;
	for( flan::Channel channel = 0; channel < a->getNumChannels(); ++channel )
		{
		ps.emplace_back( a->getSamplePointer( channel, 0 ) );
		}
	return ps;
	}

AltarClip::AltarClip( std::shared_ptr<flan::Audio> _audio
			, AudioFormatManager &_formatManager
			, AudioThumbnailCache &_thumbnailCache
			, AudioTransportSource &_transportSource
			) 
	: Button( "Temp Name" )
	, transportSource( _transportSource )
	, formatManager( _formatManager )
	, audio( _audio )
	, flanAudioChanPointers( getFlanChanPointers( audio ) )
	, juceAudio( (float * const *) &flanAudioChanPointers[0], audio->getNumChannels(), audio->getNumFrames() )
	, audioSource( juceAudio, false )
	, thumbnail( 512, _formatManager, _thumbnailCache )
	, busButton  ( "4", &FalterLookAndFeel::getLNF().fontWebdings, 18 ) 
	, saveButton ( "<", &FalterLookAndFeel::getLNF().fontWingdings, 18 )
	{
	auto & lnf = FalterLookAndFeel::getLNF();

	// Waveform thumbnail setup
	thumbnail.addChangeListener( this );
	thumbnail.reset( audio->getNumChannels(), audio->getSampleRate(), audio->getNumFrames() );
	//thumbnail.addBlock( 0, juceAudio, 0, audio->getNumFrames() );

	addAndMakeVisible( busButton   );
	addAndMakeVisible( saveButton  );

	saveButton.addListener( this );
	busButton .addListener( this );

	const int clipGroupID = 1;
	setRadioGroupId( clipGroupID );

	currentPosition.setFill( lnf.light.withAlpha( 0.85f ) );
    addAndMakeVisible( currentPosition );
	}


AltarClip::~AltarClip()
	{
	if( this == active ) stopPressed();
	}

void AltarClip::resized()
	{
	int s = getHeight() / 2;
	busButton.setBounds  ( 0, 0, s, s );
	saveButton.setBounds ( 0, s, s, s );
	}

void AltarClip::mouseDrag( const MouseEvent & )
	{
	DragAndDropContainer * dragC = DragAndDropContainer::findParentDragContainerFor( this );
	if (! dragC->isDragAndDropActive() ) 
		{
		dragC->startDragging( "Clip" , this );
		}
	}

AudioThumbnail &AltarClip::getThumbnail() { return thumbnail; }

void AltarClip::paintButton(Graphics &g, bool isMouseOverButton, bool )
	{
	auto & lnf = FalterLookAndFeel::getLNF();

	g.fillAll
		(
		getToggleState()?
			lnf.shadow.withBrightness( .32f ) :
			( isMouseOverButton? lnf.shadow.withBrightness( .27f ) : lnf.shadow )
		);

	g.setColour( lnf.accent1 );
	juce::Rectangle<int> rect( getLocalBounds().withLeft( getHeight() / 2 ) );
	thumbnail.drawChannels( g, rect.reduced( 2 ), 0, thumbnail.getTotalLength(), 1.0f );

	g.setColour( lnf.light );
	g.drawText( getName(), 
				juce::Rectangle<int>( int( getWidth() / 5.0f ), 0, int( getWidth() * ( 4.0f / 5.0f ) ), getHeight() ).reduced( 3 ), 
				Justification::topRight, true );
	if( audio )
		{
		g.drawText( String( audio->getLength() ),
						juce::Rectangle<int>( int( getHeight() / 2.0f ), 0, int( getWidth() / 5.0f ), getHeight() ).reduced( 3 ),
						Justification::topLeft );
		}
	}

void AltarClip::changeListenerCallback( ChangeBroadcaster* source )
	{
	if( source == &transportSource ) stopPressed();
	else if( source == &thumbnail ) repaint();
	}

void AltarClip::playPressed()
	{
	if( active != nullptr ) active->stopPressed();

	setToggleState( true, NotificationType::dontSendNotification );
	startTimer( 33 );
	active = this;
	
	busButton.setButtonText( "n" );
	busButton.font = &FalterLookAndFeel::getLNF().fontWingdings;

	transportSource.setSource( &audioSource, 0, nullptr, audio->getSampleRate(), audio->getNumChannels() );
	transportSource.start();
	transportSource.addChangeListener( this );
	}

void AltarClip::stopPressed()
	{
	stopTimer();
	transportSource.setSource( nullptr );
	setToggleState( false, NotificationType::dontSendNotification );
	busButton.setButtonText( "4" );
	busButton.font = &FalterLookAndFeel::getLNF().fontWebdings;
	transportSource.removeChangeListener( this );
	active = nullptr;
	transportSource.stop();
	currentPosition.setRectangle( juce::Rectangle< float >(0,0,0,0) );
	}

void AltarClip::buttonClicked( Button * button )
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
			audio->save( choice.getFullPathName().toStdString() );
			}
		setName( chooser.getResult().getFileName() );
		repaint();
		}
	}

void AltarClip::timerCallback()
	{
	// I was doing this before ???
	// const float initialX = transportSource.getCurrentPosition() 
	// 	* ( getWidth() - getHeight() / 2.0f )
	// 	/ audio->getLength() 
	// 	/ audio->getSampleRate() 
	// 	+ getHeight() / 2.0f;

	// Draw white line to show current playback position
	const float initialX = transportSource.getCurrentPosition() / audio->getLength() * getWidth();
	currentPosition.setRectangle( juce::Rectangle<float>( 
		initialX,
		0.0f, 
		getToggleState() ? 2.0f : 0.0f, 
		float( getHeight() ) ) );
	}
