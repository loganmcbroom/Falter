#include "FalterClipList.h"

#include <algorithm>
#include <cassert>

#include "FalterPlayer.h"

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

void FalterClipList::addClipFromAudio( flan::Audio a )
	{
	insertClipFromAudio( a, getNumItems() );
	}

void FalterClipList::insertClipFromAudio( flan::Audio a, size_t index )
	{
	insertItem( new FalterClip( a, player, thumbnailCache ), index );
	}

bool FalterClipList::isInterestedInDragSource( const SourceDetails & dragSourceDetails )
{
	return dragSourceDetails.description == "Clip";
}

//This function sucks but it works
void FalterClipList::itemDropped( const SourceDetails & s )
	{
	// auto parent = static_cast< FalterClipList * >( s.sourceComponent->getParentComponent() );
	// auto item = static_cast< FalterClip * >( s.sourceComponent.get() );

	// //Figure out what slot this thing should be in	
	// int slot = s.localPosition.y / getItemHeight();
	// if( parent == this && slot > getIndex( item ) ) --slot;
	// if( slot > ( getNumItems() - 1 ) ) slot = ( getNumItems() - 1 );
	// if( slot < 0 ) slot = 0;
	

	// if( parent != this )
	// 	{ 
	// 	//Copy the file out, delete the thing and recreate it in this container
	// 	File f( item->getFile().getParentDirectory().getFullPathName() + "/_altar_move.wav" );
	// 	item->getFile().copyFileTo( f );
	// 	insertClipFromFile( f, slot, item->getName(), false );
	// 	parent->erase( item );	
	// 	f.deleteFile();
	// 	}
	// else
	// 	{
	// 	//Item came from this container so we juce need to move it
	// 	swap( getIndex( item ), slot );
	// 	}
	}


