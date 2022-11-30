#include "MainComponent.h"

#include <fstream>

#include <flan/Audio.h>

#include "simplefilewatcher/FileWatcher.h"

#include "FalterLogger.h"
#include "FalterPlayer.h"
#include "Settings.h"
#include "AudioLoadThread.h"

static std::unique_ptr<FalterLogger> loggerFactory()
	{
	auto log = std::make_unique<FalterLogger>();
	Logger::setCurrentLogger( log.get() );
	return log;
	}

MainComponent::MainComponent()
	: procButton( "P", 			&FalterLookAndFeel::getLNF().fontSymbol 	)
	, scriptSelectButton( "3", 	&FalterLookAndFeel::getLNF().fontWingdings 	)
	, scriptLabel( "" )
	, player( std::make_unique<FalterPlayer>() )
	, fileWatcher()
	, recentlyAutoProcessed( false )
	, watchID( 0 )
	, sampleBrowser()
	, inClips( *player.get() )
	, outClips( *player.get() )
	, threads()
	, log( loggerFactory() )
	, settings( std::make_unique<Settings>() )
	, logHeight( FalterLookAndFeel::getLNF().unit * 5 + FalterLookAndFeel::getLNF().margin * 4 )
	{
	setLookAndFeel( &FalterLookAndFeel::getLNF() );

	addAndMakeVisible( *log 				);
	addAndMakeVisible( &procButton 			);
	addAndMakeVisible( &scriptSelectButton 	);
	addAndMakeVisible( &scriptLabel    		);
	addAndMakeVisible( &sampleBrowser 		);
	addAndMakeVisible( &inClips    			);
	addAndMakeVisible( &threads	   			);
	addAndMakeVisible( &outClips   			);

	procButton			.addListener( this );
	scriptSelectButton	.addListener( this );
	sampleBrowser		.addListener( this );

	inClips.setName( "Inputs" );
	threads.setName( "Threads" );
	outClips.setName( "Outputs" );

	scriptLabel.setFont( FalterLookAndFeel::getLNF().fontMonospace );
	scriptLabel.setText( Settings::getScriptFile().getFullPathName(), dontSendNotification );
	if( File( scriptLabel.getText() ).exists() )
		addWatchProtected();

	setSize( 1000, 700 );

	Logger::writeToLog( "Falter Initialized\n" );

	startTimer( 100 );
	}

MainComponent::~MainComponent()
	{
	player->stop();
	Logger::setCurrentLogger( nullptr );
	setLookAndFeel( nullptr );
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
	const int numButtons = 2;

	auto boundButton = [&]( int n, Component * c )
		{
		c->setBounds( n * ( u + m ) + m, m, u, u );
		};

	boundButton( 0, &procButton );
	boundButton( 1, &scriptSelectButton );

	scriptLabel.setBounds( 
		numButtons * ( u + m ) + m, 
		m, 
		w - ( numButtons * ( u + m ) + 2 * m ), 
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

void MainComponent::buttonClicked( Button* button )
	{
		 if( button == &procButton	) 			{ procButtonClicked(); }
	else if( button == &scriptSelectButton	) 	{ scriptSelectButtonClicked(); }
	}

void MainComponent::importFile( File file )
	{
	inClips.importAudioFileAsync( file );
	}

void MainComponent::procButtonClicked() 
	{
	if( ! File( scriptLabel.getText() ).exists() )
		{
		Logger::writeToLog( "The provided script does not exist: " + scriptLabel.getText() );
		return;
		}

	auto retrieveFiles = [&]( AudioVec & as, const String & threadName )
		{
		for( auto & a : as )
			if( ! a.isNull() )
				outClips.insertClipFromAudio( a, -1, String( "Output of: " ) + threadName );
		};

	AudioVec inAudio;
	for( int i = 0; i < inClips.getNumItems(); ++i )
		inAudio.emplace_back( dynamic_pointer_cast<FalterClip>( inClips.getItem( i ) )->getAudio() );
		
	threads.addThread( scriptLabel.getText(), retrieveFiles, inAudio );
	}

void MainComponent::scriptSelectButtonClicked()
	{
	FileChooser chooser( "Select a Lua script to run", Settings::getScriptFile(), "*.lua;*.txt" ); 

	if( chooser.browseForFileToOpen() )
		{
		File result = chooser.getResult();
		Settings::setScriptFile( result );
		scriptLabel.setText( result.getFullPathName(), NotificationType::dontSendNotification );

		// Start watching new script
		addWatchProtected();
		}
	}

void MainComponent::filesDropped( const StringArray & files, int, int )
	{
	for( const auto & file : files )
		importFile( File( file ) );
	fileDragExit( files );
	}

bool MainComponent::isInterestedInFileDrag( const StringArray & ) { return true; }

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
	settings->setFileLoadDir( dir );
	}

void MainComponent::handleFileAction( FW::WatchID otherWatchID, const FW::String & dir, const FW::String & filename, FW::Action action )
	{
	if( otherWatchID == watchID )
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
