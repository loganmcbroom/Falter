#include "FalterFileBrowser.h"

#include "FalterLookandFeel.h"

FalterFileBrowser::FalterFileBrowser()
    : FileBrowserComponent(
		FileBrowserComponent::openMode | 
		FileBrowserComponent::canSelectFiles | 
        FileBrowserComponent::canSelectMultipleItems |
		FileBrowserComponent::canSelectDirectories | 
		FileBrowserComponent::useTreeView | 
		FileBrowserComponent::filenameBoxIsReadOnly | 
		FileBrowserComponent::warnAboutOverwriting,
		File("C:/Users/logan/Documents/Samples"), 
        nullptr, 
        nullptr ) 
    , filter( "*.wav;*.aiff", "", "Audio files" )
    {
    setFileFilter( &filter );
    }

void FalterFileBrowser::paint( Graphics & g )
    {
    g.fillAll( FalterLookAndFeel::getLNF().dark );
    //FileBrowserComponent::paint( g );
    }

// void FalterList<T>::paintButton( Graphics & g, bool, bool )
// 	{
// 	g.fillAll( dark );
// 	}

