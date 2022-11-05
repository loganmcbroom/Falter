#pragma once

#include <JuceHeader.h>

#include "FalterButton.h"
#include "FalterFileBrowser.h"
#include "FalterClipList.h"
#include "FalterThreadList.h"
#include "FalterLookAndFeel.h"
class FalterLogger;

#include "FalterLookandFeel.h"

class MainComponent : public AudioAppComponent
                    , public ChangeListener
                    , public Button::Listener
                    , public FileDragAndDropTarget
                    , public DragAndDropContainer
					, public FileBrowserListener
{
public:
    MainComponent();
    ~MainComponent();

    void prepareToPlay( int samplesPerBlockExpected, double sampleRate ) override;
    void getNextAudioBlock( const AudioSourceChannelInfo& bufferToFill ) override;
    void releaseResources() override;
    void paint (Graphics& g) override;
    void resized() override;
	void changeListenerCallback( ChangeBroadcaster* source ) override;
	void buttonClicked( Button* button ) override;

private:
    void importFile( File file );

	void procButtonClicked(); // Processes input files with the supplied lua script
	void scriptSelectButtonClicked();

	void filesDropped( const StringArray & files, int x, int y ) override;
	bool isInterestedInFileDrag( const StringArray & ) override;
	void fileDragEnter( const StringArray &, int, int ) override;
	void fileDragExit( const StringArray & ) override;

	void selectionChanged() override {}
	void fileClicked( const File & file, const MouseEvent & e ) override {}
	void fileDoubleClicked( const File & file ) override;
	void browserRootChanged( const File & newRoot ) override {}

	FalterButton procButton;
	FalterButton scriptSelectButton;
	
	Label scriptLabel;
	
	AudioFormatManager formatManager;
	AudioTransportSource transportSource;

	FalterFileBrowser sampleBrowser;
	FalterClipList inClips, outClips;
	FalterThreadList threads;

	std::unique_ptr<FalterLogger> log; // Using ptr because logger needs lnf access which doesn't exist until after we set it in ctor
	const int logHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
