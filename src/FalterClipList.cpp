#include "FalterClipList.h"

#include <algorithm>
#include <cassert>

#include "FalterPlayer.h"
#include "DragAndDropTypes.h"

#define MAX_CLIPS 128

//Constructor
FalterClipList::FalterClipList( FalterPlayer & _player )
	: player( _player )
	, thumbnailCache( MAX_CLIPS )
	{
	}

//Destructor
FalterClipList::~FalterClipList()
	{
	for( int i = 0; i < getNumItems(); ++i )
		{
		dynamic_pointer_cast<FalterClip>( getItem( i ) )->getThumbnail().setSource( nullptr );
		}
	}

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

void FalterClipList::addClipFromAudio( flan::Audio a, const String & name )
	{
	insertClipFromAudio( a, getNumItems(), name );
	}

void FalterClipList::insertClipFromAudio( flan::Audio a, size_t index, const String & name )
	{
	insertItem( new FalterClip( a, player, thumbnailCache, name ), index );
	}

bool FalterClipList::isInterestedInDragSource( const SourceDetails & s )
	{
	return s.description == DragAndDropTypes::AudioClip 
		|| s.description == DragAndDropTypes::FalterFile;
	}

void FalterClipList::itemDropped( const SourceDetails & s )
	{
	const int slot = std::clamp( int( s.localPosition.y / itemHeight ), 0, getNumItems() );

	if( s.description == DragAndDropTypes::AudioClip )	
		{
		auto parent = dynamic_cast<FalterClipList *>( s.sourceComponent->getParentComponent() );
		auto item = dynamic_cast<FalterClip *>( s.sourceComponent.get() );
		if( ! parent || ! item ) return;
		
		if( parent != this )
			{ 
			insertClipFromAudio( item->audio, slot, item->getName() );
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
			flan::Audio audio( file.getFullPathName().toStdString() );
			if( audio.isNull() ) continue;
			insertClipFromAudio( audio, slot, file.getFileName() );
			}
		}
	else jassertfalse;
	}


