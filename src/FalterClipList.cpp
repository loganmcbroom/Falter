#include "FalterClipList.h"

#include <algorithm>
#include <cassert>

#include "FalterPlayer.h"
#include "DragAndDropTypes.h"
#include "AudioLoadThread.h"

#define MAX_CLIPS 1024

FalterClipList::FalterClipList( FalterPlayer & _player )
	: player( _player )
	, thumbnailCache( MAX_CLIPS )
	, threadPool()
	{
	}

FalterClipList::~FalterClipList() {}

void FalterClipList::erase( FalterClip * item )
	{
	FalterClipList::erase( getIndex( item ) );
	}

void FalterClipList::erase( unsigned int index )
	{	
	thumbnailCache.removeThumb( dynamic_pointer_cast<FalterClip>( getItem( index ) )->getThumbnail().getHashCode() );
	FalterList::erase( index );
	}

void FalterClipList::clear()
	{
	FalterList::clear();
	thumbnailCache.clear();
	}

void FalterClipList::insertClipFromAudio( 
	std::shared_ptr<flan::Audio> a, 
	int index, 
	const String & name, 
	const File & globalFile,
	flan::Audio::SndfileStrings strings )
	{
	if( !a->is_null() )
		insertItem( std::make_shared<FalterClip>( a, player, thumbnailCache, name, globalFile, strings ), index == -1 ? getNumItems() : index );
	}

bool FalterClipList::isInterestedInDragSource( const SourceDetails & s )
	{
	return s.description == DragAndDropTypes::AudioClip 
		|| s.description == DragAndDropTypes::FalterFile;
	}

void FalterClipList::itemDropped( const SourceDetails & s )
	{
	//const float scrollZoom = scroll.getCurrentRangeSize() / scroll.getMaximumRangeLimit();
	const float trueYPos = s.localPosition.y + scroll.getCurrentRange().getStart();
	const int slot = std::clamp( int( trueYPos / itemHeight ), 0, getNumItems() );

	if( s.description == DragAndDropTypes::AudioClip )	
		{
		auto parent = dynamic_cast<FalterClipList *>( s.sourceComponent->getParentComponent() );
		auto item = dynamic_cast<FalterClip *>( s.sourceComponent.get() );
		if( ! parent || ! item ) return;
		
		if( parent != this )
			{ 
			insertClipFromAudio( item->audio, slot, item->getName(), item->getGlobalFile(), item->getSndFileStrings() );
			parent->erase( item );	
			}
		else
			{
			swap( getIndex( item ), slot > getIndex( item ) ? slot - 1 : slot );
			}
		}
	else if( s.description == DragAndDropTypes::FalterFile )
		{
		auto tree = dynamic_cast<FileTreeComponent *>( s.sourceComponent.get() );
		if( ! tree ) return;
		const int n = tree->getNumSelectedFiles();
		for( int i = 0; i < n; ++i )
			{
			File file = tree->getSelectedFile( i );
			importAudioFileAsync( file );
			}
		}
	else jassertfalse;
	}

void FalterClipList::importAudioFileAsync( const File & file )
	{
	auto newJob = std::make_unique<AudioLoadThread>( file );
	newJob->addListener( this );
	threadPool.addJob( newJob.release(), true );
	}

void FalterClipList::exitSignalSent() 
	{
	auto * job = dynamic_cast<AudioLoadThread *>( ThreadPoolJob::getCurrentThreadPoolJob() );
	if( ! job )
		{
		const MessageManagerLock mml;
		if( mml.lockWasGained() )
			Logger::writeToLog( "Audio loader couldn't be accessed." );
		return;
		}

	const MessageManagerLock mml;
	if( mml.lockWasGained() )
		{
		insertClipFromAudio( std::move( job->output ), -1, job->file.getFileName(), job->file, job->strings );
		}
	}

