#include "FalterFileBrowser.h"

#include <utility>

#include "FalterLookandFeel.h"
#include "Settings.h"
#include "FalterClip.h"
#include "DragAndDropTypes.h"

FalterFileBrowser::FalterFileBrowser()
    : FileBrowserComponent(
		FileBrowserComponent::openMode | 
		FileBrowserComponent::canSelectFiles | 
        FileBrowserComponent::canSelectMultipleItems |
		FileBrowserComponent::canSelectDirectories | 
		FileBrowserComponent::useTreeView | 
		FileBrowserComponent::filenameBoxIsReadOnly | 
		FileBrowserComponent::warnAboutOverwriting,
		Settings::getFileLoadDir(), 
        nullptr, 
        nullptr ) 
    , filter( "*.wav;*.aiff", "", "Audio files" )
    {
    setFileFilter( &filter );
    auto * tree = dynamic_cast<FileTreeComponent *>( getDisplayComponent() );
    if( tree )
        tree->setDragAndDropDescription( DragAndDropTypes::FalterFile );
    else
        Logger::writeToLog( "Drag and Drop from file browser setup failed... somehow?" );
    }

void FalterFileBrowser::paint( Graphics & g )
    {
    g.fillAll( FalterLookAndFeel::getLNF().dark );
    }

bool FalterFileBrowser::isInterestedInDragSource( const SourceDetails & s )
    {
    return s.description == DragAndDropTypes::AudioClip;
    }

void FalterFileBrowser::itemDropped( const SourceDetails & s )
    {
    if( s.description == DragAndDropTypes::AudioClip )	
		{
		auto item = dynamic_cast<FalterClip *>( s.sourceComponent.get() );
		if( ! item ) return;
		const std::shared_ptr<flan::Audio> audio = item->getAudio();
        if( audio->is_null() ) return;

        const String filepath = getRoot().getFullPathName() + "\\" + item->getName();
        Logger::writeToLog( "Saving by dropping to browser is currently disabled." );
        //audio.save( filepath.toStdString() );
		}
	else jassertfalse;
    }


