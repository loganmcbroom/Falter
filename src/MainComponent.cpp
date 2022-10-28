#include "MainComponent.h"

#include <fstream>

#include <flan/Audio.h>

#include "AltarLogger.h"

MainComponent::MainComponent()
	: procButton( "P", 			&FalterLookAndFeel::getLNF().fontSymbol 	)
	, docButton( "&", 			&FalterLookAndFeel::getLNF().fontWingdings )
	, sampleFolderButton( "%", 	&FalterLookAndFeel::getLNF().fontWingdings )
	, scriptSelectButton( "3", 	&FalterLookAndFeel::getLNF().fontWingdings )
	, command( "" )
	, formatManager()
	, transportSource()
	, sampleBrowser()
	, inClips( formatManager, transportSource )
	, outClips( formatManager, transportSource )
	, log()
	, logHeight( FalterLookAndFeel::getLNF().unit * 5 + FalterLookAndFeel::getLNF().margin * 4 )
	{
	setLookAndFeel( &FalterLookAndFeel::getLNF() );

	log.reset( new AltarLogger() );

	Logger::setCurrentLogger( log.get() );
	addAndMakeVisible( *log 				);
	addAndMakeVisible( &procButton 			);
	addAndMakeVisible( &docButton 			);
	addAndMakeVisible( &sampleFolderButton 	);
	addAndMakeVisible( &scriptSelectButton 	);
	addAndMakeVisible( &command    			);
	addAndMakeVisible( &sampleBrowser 		);
	addAndMakeVisible( &inClips    			);
	addAndMakeVisible( &threads	   			);
	addAndMakeVisible( &outClips   			);

	procButton			.addListener( this );
	docButton			.addListener( this );
	sampleFolderButton	.addListener( this );
	scriptSelectButton	.addListener( this );
	sampleBrowser		.addListener( this );

	command.setFont( FalterLookAndFeel::getLNF().fontMonospace );
	command.setText( "Falter.lua", dontSendNotification );

	setSize( 1000, 700 );

	formatManager.registerBasicFormats();

	setAudioChannels( 0, 2 );

	// if( ! File::getCurrentWorkingDirectory().getChildFile( "Settings.lua" ).exists() )
	// 	{
	// 	FileChooser chooser( "Select your _cdprogs directory (it should be in cdprX/_cdp)" );
	// 	if( ! chooser.browseForDirectory() 
	// 	 || ! chooser.getResult().exists()
	// 	 || ! chooser.getResult().getChildFile( "modify.exe" ).exists() )
	// 		Logger::writeToLog( "Things won't work until you pick the correct _cdprogs directory" );
	// 	else
	// 		{
	// 		std::ofstream settingsFile( "Settings.lua" ); 
	// 		settingsFile << "cdpDir = \"" << chooser.getResult().getFullPathName().replace( "\\", "/" ) << "\"";
	// 		}
	// 	}

	Logger::writeToLog( "Falter Initialized" );
	}

MainComponent::~MainComponent()
	{
    shutdownAudio();
	Logger::setCurrentLogger( nullptr );
	setLookAndFeel( nullptr );
	}


//Called when the audio device is started or when its settings are changed
void MainComponent::prepareToPlay( int samplesPerBlockExpected, double sampleRate )
	{
	transportSource.prepareToPlay( samplesPerBlockExpected, sampleRate );
	}

//This function is handed an audio buffer to fill as is needed for output
void MainComponent::getNextAudioBlock( const AudioSourceChannelInfo& bufferToFill )
	{
	if( AltarClip::active == nullptr )
		{
		bufferToFill.clearActiveBufferRegion();
		return;
		}

	transportSource.getNextAudioBlock( bufferToFill );
	}

//Called when the audio device stops or when it is restarted
void MainComponent::releaseResources()
	{
	transportSource.releaseResources();
	}

//Paint
void MainComponent::paint( Graphics& g )
	{
    g.fillAll( FalterLookAndFeel::getLNF().shadow );
	}

//Resized
void MainComponent::resized()
	{
	const int m = FalterLookAndFeel::getLNF().margin;
	const int u = FalterLookAndFeel::getLNF().unit;
	const int w = getWidth();
	const int h = getHeight();

	auto boundButton = [&]( int n, Component * c )
		{
		c->setBounds( n * ( u + m ) + m, m, u, u );
		};

	boundButton( 0, &procButton );
	boundButton( 1, &docButton );
	boundButton( 2, &sampleFolderButton );
	boundButton( 3, &scriptSelectButton );

	command.setBounds( 
		4 * ( u + m ) + m, 
		m, 
		w - ( 4 * ( u + m ) + 2 * m ), 
		u 
		);

	auto boundColumn = [&]( int n, int N, Component * c )
		{
		const float iw = w - m;
		c->setBounds( 
			n * ( iw / N ) + m, // x
			m * 2 + u, // y
			iw / N - m, // Width
			h - ( u + m * 3 ) - ( logHeight + m ) // Height
			);
		};

	boundColumn( 0, 4, &sampleBrowser );
	boundColumn( 1, 4, &inClips );
	boundColumn( 2, 4, &threads );
	boundColumn( 3, 4, &outClips );

	Rectangle<int> errorRect( 
		m, getHeight() - ( logHeight + m ), 
		getWidth() - m * 2, logHeight );

	log->setBounds( errorRect );
	}

/// What is this for?
void MainComponent::changeListenerCallback( ChangeBroadcaster * )
	{
	}

//Called when a button is pressed and delegates functionality to specific functions
void MainComponent::buttonClicked( Button* button )
	{
		 if( button == &procButton	) 			{ procButtonClicked(); }
	else if( button == &docButton	) 			{ docButtonClicked(); }
	else if( button == &sampleFolderButton	) 	{ sampleFolderButtonClicked(); }
	else if( button == &scriptSelectButton	) 	{ scriptSelectButtonClicked(); }
	}

// Adds given file to the input cliplist
void MainComponent::importFile( File file )
	{
	auto audio = std::make_shared<flan::Audio>( file.getFullPathName().toStdString() );
	inClips.addClipFromAudio( audio );
	procButton.setEnabled( true );
	}

void MainComponent::procButtonClicked() 
	{
	if( ! File( File::getCurrentWorkingDirectory().getFullPathName() + "/" + command.getText() ).exists() )
		{
		Logger::writeToLog( "The provided script does not exist:" );
		Logger::writeToLog( command.getText() );
		return;
		}

	std::function< void( std::vector<std::shared_ptr<flan::Audio>> & ) > retrieveFiles = [&]( std::vector<std::shared_ptr<flan::Audio>> & as )
		{
		for( auto & a : as )
			outClips.addClipFromAudio( a );
		};

	std::vector<std::shared_ptr<flan::Audio>> inAudio;
	for( int i = 0; i < inClips.getNumItems(); ++i )
		inAudio.emplace_back( inClips.getItem( i )->getAudio() );
		
	threads.addThread( command.getText(), retrieveFiles, inAudio );
	}

void MainComponent::docButtonClicked()
	{
	Logger::writeToLog( "Remove this button." );
	}

void MainComponent::sampleFolderButtonClicked()
	{
	Logger::writeToLog( "Remove this button." );
	}

void MainComponent::scriptSelectButtonClicked()
	{
	FileChooser chooser( "Select a file to play", File(), "*.wav;*.pvoc" ); 

	if( chooser.browseForFileToOpen() )
		importFile( chooser.getResult() );
	}


//Called when files were dropped only the program from
void MainComponent::filesDropped( const StringArray & files, int, int )
	{
	for ( auto &&file : files )
		{ 
		importFile( File( file ) );
		}
	fileDragExit(files);
	}

bool MainComponent::isInterestedInFileDrag( const StringArray & ) { return true; }

void MainComponent::fileDragEnter( const StringArray &, int, int ) { scriptSelectButton.setButtonText( "1" ); }

void MainComponent::fileDragExit( const StringArray & ) { scriptSelectButton.setButtonText( "0" ); }

void MainComponent::fileDoubleClicked( const File & file ) 
	{
	importFile( file );
	}
