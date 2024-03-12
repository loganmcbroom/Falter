#include "MainComponent.h"

#include <fstream>

#include <flan/Audio/Audio.h>

#include "simplefilewatcher/FileWatcher.h"

#include "FalterLogger.h"
#include "FalterPlayer.h"
#include "Settings.h"
#include "AudioLoadThread.h"
#include "AudioRecorder.h"
#include "DragAndDropTypes.h"

MainComponent * MainComponent::instance = nullptr;

MainComponent * MainComponent::getInstance()
	{
	jassert( instance );
	return instance;
	}

static std::unique_ptr<FalterLogger> loggerFactory()
	{
	auto log = std::make_unique<FalterLogger>();
	Logger::setCurrentLogger( log.get() );
	return log;
	}

MainComponent::MainComponent()
	: log( loggerFactory() )
	, settings( std::make_unique<Settings>() )
	, audioDeviceManager( std::make_unique<AudioDeviceManager>() )
	, player( std::make_unique<FalterPlayer>() )
	, recorder( std::make_unique<AudioRecorder>() )
	, settingsMode( false )
	, autoProcess( true )
	, fileWatcher()
	, recentlyAutoProcessed( false )
	, watchID( 0 )
	, mainContainer()
	, procButton( "N", 			&FalterLookAndFeel::getLNF().fontWingdings 	)
	, recordButton( "l", 		&FalterLookAndFeel::getLNF().fontWingdings  )
	, settingsButton( "$", 		&FalterLookAndFeel::getLNF().fontWingdings  )
	, scriptSelectButton( "0", 	&FalterLookAndFeel::getLNF().fontWingdings 	)
	, autoProcessButton( "R", 	&FalterLookAndFeel::getLNF().fontWingdings  )
	, scriptLabel( "" )
	, sampleBrowser()
	, inClips( *player.get() )
	, outClips( *player.get() )
	, threads()
	, settingsContainer()
	, deviceSelector( *audioDeviceManager )
	, logHeight( FalterLookAndFeel::getLNF().unit * 5 + FalterLookAndFeel::getLNF().margin * 4 )
	, workingDirectory()
	{
	instance = this;

	setLookAndFeel( &FalterLookAndFeel::getLNF() );

	audioDeviceManager->initialise( 2, 2, nullptr, true );
	audioDeviceManager->addAudioCallback( recorder.get() );

	addAndMakeVisible( procButton 			);
	addAndMakeVisible( recordButton			);
	addAndMakeVisible( settingsButton		);
	addAndMakeVisible( scriptSelectButton 	);
	addAndMakeVisible( autoProcessButton  	);
	addAndMakeVisible( scriptLabel    		);

	procButton         .down_text = "S";
	recordButton       .down_text = ">";     
	settingsButton     .down_text = "O";       
	scriptSelectButton .down_text = "1";           
	autoProcessButton  .down_text = "Q";          

	addAndMakeVisible( &mainContainer );
		mainContainer.addAndMakeVisible( sampleBrowser 			);
		mainContainer.addAndMakeVisible( inClips    			);
		mainContainer.addAndMakeVisible( threads	   			);
		mainContainer.addAndMakeVisible( outClips   			);

	addChildComponent( &settingsContainer );
		settingsContainer.addAndMakeVisible( &deviceSelector 	);
		settingsContainer.setColour( Label::ColourIds::backgroundColourId, FalterLookAndFeel::getLNF().dark );

	addAndMakeVisible( *log );
	
	procButton			.addListener( this );
	recordButton		.addListener( this );
	settingsButton		.addListener( this );
	scriptSelectButton	.addListener( this );
	sampleBrowser		.addListener( this );
	autoProcessButton	.addListener( this );

	inClips	.setName( "Inputs" 	);
	threads	.setName( "Threads" );
	outClips.setName( "Outputs" );

	scriptLabel.setFont( FalterLookAndFeel::getLNF().fontMonospace );
	scriptLabel.setText( Settings::getScriptFile().getFullPathName(), dontSendNotification );
	if( File( scriptLabel.getText() ).exists() )
		addWatchProtected();

	setSize( 1000, 700 );

	startTimer( 100 );

	// Set up the working directory
	workingDirectory = File::getCurrentWorkingDirectory().getChildFile( "workspace" );
	if( workingDirectory.exists() && workingDirectory.isDirectory() )
		workingDirectory.deleteRecursively();
	workingDirectory.createDirectory();

	Logger::writeToLog( "Falter Initialized\n" );
	}

MainComponent::~MainComponent()
	{
	player->stop();
	recorder->stopRecording();
	audioDeviceManager->removeAudioCallback( recorder.get() );
	Logger::setCurrentLogger( nullptr );
	setLookAndFeel( nullptr );

	instance = nullptr;

	// if( workingDirectory.exists() && workingDirectory.findChildFiles( File::TypesOfFileToFind::findFiles, false ).isEmpty() )
	// 	workingDirectory.deleteFile();
	}

void MainComponent::paint( Graphics& g )
	{
    g.fillAll( FalterLookAndFeel::getLNF().shadow );
	}

void MainComponent::resized()
	{
	const int m = FalterLookAndFeel::getLNF().margin;
	const int u = FalterLookAndFeel::getLNF().unit;
	const int w = getWidth();
	const int h = getHeight();
	const int numButtons = 5;

	Rectangle<int> containerRect( 
		0, 
		m * 2 + u, 
		w, 
		h - ( u + m * 2 ) - ( logHeight + m * 2 ) );
	mainContainer.setBounds( containerRect );
	settingsContainer.setBounds( containerRect );

	auto boundButton = [&]( int n, Component * c )
		{
		c->setBounds( n * ( u + m ) + m, m, u, u );
		};

	boundButton( 0, &procButton );
	boundButton( 1, &recordButton );
	boundButton( 2, &settingsButton );
	boundButton( 3, &autoProcessButton );
	boundButton( 4, &scriptSelectButton );

	scriptLabel.setBounds( 
		numButtons * ( u + m ) + m, 
		m, 
		w - ( numButtons * ( u + m ) + 2 * m ), 
		u 
		);

	auto boundColumn = [&]( int n, int N, Component * c )
		{
		const float iw = static_cast<float>( w - m );
		c->setBounds( 
			static_cast<int>( n * ( iw / N ) + m ), // x
			0, // y
			static_cast<int>( iw / N - m ), // Width
			c->getParentHeight() // Height
			);
		};

	boundColumn( 0, 4, &sampleBrowser );
	boundColumn( 1, 4, &inClips );
	boundColumn( 2, 4, &threads );
	boundColumn( 3, 4, &outClips );

	deviceSelector.setBounds( m, 0, deviceSelector.getParentWidth() - 2 * m, deviceSelector.getParentHeight() );

	Rectangle<int> bottomRect( 
		m, getHeight() - ( logHeight + m ), 
		getWidth() - m * 2, logHeight );

	log->setBounds( bottomRect );
	}

void MainComponent::buttonClicked( Button* button )
	{
		 if( button == &procButton			) procButtonClicked(); 			
	else if( button == &recordButton		) recordButtonClicked();		
	else if( button == &settingsButton		) settingsButtonClicked();		
	else if( button == &scriptSelectButton	) scriptSelectButtonClicked(); 	
	else if( button == &autoProcessButton	) autoProcessButtonClicked(); 	
	}

void MainComponent::importFile( File file )
	{
	inClips.importAudioFileAsync( file );
	}

void MainComponent::procButtonClicked() 
	{
	const File file = File( scriptLabel.getText() );
	if( ! file.exists() )
		{
		Logger::writeToLog( "The provided script does not exist: " + scriptLabel.getText() );
		return;
		}

	AudioVec inAudio;
	for( int i = 0; i < inClips.getNumItems(); ++i )
		{
		flan::Audio input_copy = dynamic_pointer_cast<FalterClip>( inClips.getItem( i ) )->getAudio()->copy();
		inAudio.push_back( std::make_shared<flan::Audio>( std::move( input_copy ) ) );
		}

	flan::Audio::SndfileStrings sndfileStrings;
	sndfileStrings.comment = file.loadFileAsString().toStdString();
	sndfileStrings.artist = scriptLabel.getText().toStdString();
	String inputs_string;
	for( int i = 0; i < inClips.getNumItems(); ++i )
		inputs_string += ( dynamic_pointer_cast<FalterClip>( inClips.getItem( i ) )->getName() + "\n" );
	sndfileStrings.title = inputs_string.toStdString();

	auto retrieveFiles = [&, sndfileStrings = sndfileStrings]( AudioVec & as, const String & threadName )
		{
		for( int i = as.size()-1; i >= 0; --i )
			{
			if( ! as[i]->is_null() )
				outClips.insertClipFromAudio( as[i], 0, String( "Output " ) + String( i + 1 ) + String( " of: " ) + threadName, sndfileStrings );
			else
				Logger::writeToLog( "Null output recieved, this is usually caused by an invalid method input" );
			}
		// If we are in hot mode and got outputs, play the first of them
		if( !std::all_of( as.begin(), as.end(), []( std::shared_ptr<flan::Audio> a ){ return a->is_null(); } ) && autoProcess )
			{
			auto child = outClips.getItem( 0 );
			if( auto clip = std::dynamic_pointer_cast<FalterClip>( child ) )
				clip->playPressed();
			}
		};
		
	threads.addThread( scriptLabel.getText(), retrieveFiles, inAudio );
	}

void MainComponent::recordButtonClicked()
	{
	if( recorder->isRecording() )
		{
		std::shared_ptr<flan::Audio> recording = recorder->stopRecording();
		inClips.insertClipFromAudio( recording, -1, String( "Recording " ) + String( recorder->getNumRecords() ) );
		recordButton.setButtonText( "l" );
		}
	else
		{
		recorder->startRecording();
		recordButton.setButtonText( "n" );
		}
	}

void MainComponent::settingsButtonClicked()
	{
	settingsMode = !settingsMode;

	if( settingsMode )
		settingsButton.setButtonText( "M" );
	else
		settingsButton.setButtonText( "$" );

	mainContainer.setVisible( ! settingsMode );
	settingsContainer.setVisible( settingsMode );
	}

void MainComponent::scriptSelectButtonClicked()
	{
	FileChooser chooser( "Select a Lua script to run", Settings::getScriptFile(), "*.lua;*.txt" ); 

	if( chooser.browseForFileToOpen() )
		{
		File result = chooser.getResult();
		Settings::setScriptFile( result );
		scriptLabel.setText( result.getFullPathName(), NotificationType::dontSendNotification );

		// Start watching new script for changes
		addWatchProtected();
		}
	}

void MainComponent::autoProcessButtonClicked()
	{
	autoProcess = !autoProcess;

	if( autoProcess )
		autoProcessButton.setButtonText( "R" );
	else
		autoProcessButton.setButtonText( "T" );
	}

void MainComponent::filesDropped( const StringArray & files, int, int )
	{
	for( const auto & file_string : files )
		{
		File file( file_string );
		if( file.exists() && !file.isDirectory() )
			importFile( file );
		}
	fileDragExit( files );
	}

bool MainComponent::isInterestedInFileDrag( const StringArray & ) 
	{ 
	return true; 
	}

void MainComponent::fileDragEnter( const StringArray & s, int, int ) 
	{ 
	inClips.setClearButtonText( "Y" ); 
	inClips.setName( File( s[0] ).getFileName() );
	inClips.repaint();
	}

void MainComponent::fileDragExit( const StringArray & ) 
	{ 
	inClips.setClearButtonText( "r" ); 
	inClips.setName( "Inputs" );
	inClips.repaint();
	}

bool MainComponent::shouldDropFilesWhenDraggedExternally( 
	const DragAndDropTarget::SourceDetails & sourceDetails, 
	StringArray & files, 
	bool & canMoveFiles ) 
	{
	if( sourceDetails.description == DragAndDropTypes::AudioClip )
		{
		auto * clip = dynamic_cast<FalterClip *>( sourceDetails.sourceComponent.get() );
		if( !clip ) return false;

		File f;
		if( clip->getGlobalFile().existsAsFile() )
			f = clip->getGlobalFile();
		else if( clip->getWorkspaceFile().existsAsFile() )
			f = clip->getWorkspaceFile();
		else return false;

		files.add( f.getFullPathName() );
		canMoveFiles = false;
		return true;
		}
	return false;
	}

void MainComponent::selectionChanged()
	{
	}

void MainComponent::fileClicked( const File &, const MouseEvent & )
	{
	}

void MainComponent::fileDoubleClicked( const File & file ) 
	{
	importFile( file );
	}

void MainComponent::browserRootChanged( const File & dir )
	{
	settings->setAudioLoadDir( dir );
	}

void MainComponent::handleFileAction( FW::WatchID otherWatchID, const FW::String &, const FW::String & filename, FW::Action action )
	{
	if( autoProcess && otherWatchID == watchID && filename == File( scriptLabel.getText() ).getFileName().toStdString() )
		{
		if( action == FW::Action::Modified && ! recentlyAutoProcessed )
			{
			procButtonClicked();
			recentlyAutoProcessed = true;
			}
		else if( action == FW::Action::Delete )
			fileWatcher.removeWatch( watchID );
		}
	}

void MainComponent::addWatchProtected()
	{
	String dirString = File( scriptLabel.getText() ).getParentDirectory().getFullPathName();
	try 
		{ 
		fileWatcher.removeWatch( watchID );
		watchID = fileWatcher.addWatch( dirString.toStdString(), this, true ); 
		}
	catch( FW::FileNotFoundException & ) 
		{ std::cout << "File watcher could not be created, automatic processing disabled."; }
	}

void MainComponent::timerCallback() 
	{
	recentlyAutoProcessed = false;
	fileWatcher.update();
	}
