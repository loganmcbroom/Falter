#pragma once

#include <JuceHeader.h>

#include "simplefilewatcher/FileWatcher.h"

#include "FalterButton.h"
#include "FalterFileBrowser.h"
#include "FalterClipList.h"
#include "FalterThreadList.h"
#include "FalterLookAndFeel.h"
class FalterLogger;
class FalterPlayer;
class Settings;

class MainComponent : public Component
                    , public Button::Listener
					, public FileBrowserListener
                    , public FileDragAndDropTarget
                    , public DragAndDropContainer
					, public FW::FileWatchListener
					, public Timer
{
public:
    MainComponent();
    ~MainComponent();

    void paint( Graphics & g ) override;
    void resized() override;
	void buttonClicked( Button * button ) override;

private:
    void importFile( File file );

	void procButtonClicked(); // Processes input files with the supplied lua script
	void scriptSelectButtonClicked();

	void filesDropped( const StringArray & files, int x, int y ) override;
	bool isInterestedInFileDrag( const StringArray & ) override;
	void fileDragEnter( const StringArray &, int, int ) override;
	void fileDragExit( const StringArray & ) override;

	// FileBrowserListener interface
	void selectionChanged() override;
	void fileClicked( const File &, const MouseEvent & ) override;
	void fileDoubleClicked( const File & file ) override;
	void browserRootChanged( const File & ) override;

	// FileWatchListener interface
	void handleFileAction( FW::WatchID watchid, const FW::String & dir, const FW::String & filename, FW::Action action ) override;
	void addWatchProtected();

	// Timer interface
	void timerCallback() override;

	std::unique_ptr<FalterLogger> log;
	std::unique_ptr<Settings> settings;
	std::unique_ptr<FalterPlayer> player;

	FW::FileWatcher fileWatcher;
	bool recentlyAutoProcessed;
	FW::WatchID watchID;
	
	FalterButton procButton;
	FalterButton scriptSelectButton;
	
	Label scriptLabel;

	FalterFileBrowser sampleBrowser;
	FalterClipList inClips, outClips;
	FalterThreadList threads;

	const int logHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
