#include "AltarClipList.h"

#include <algorithm>
#include <cassert>

#define MAX_CLIPS 128

using namespace std;

//Constructor
AltarClipList::AltarClipList( AudioFormatManager &_formatManager, AudioTransportSource &_transportSource )
	: transportSource( _transportSource )
	, formatManager( _formatManager )
	, thumbnailCache( MAX_CLIPS )
	{
	}

//Destructor
AltarClipList::~AltarClipList()
	{
	for( int i = 0; i < getNumItems(); ++i )
		{
		 getItem( i )->getThumbnail().setSource( nullptr );
		}
	}

//Just how big is each clip?
int AltarClipList::getItemHeight()
	{
	return 50;
	}

//Erase given item from the list
void AltarClipList::erase( AltarClip * item )
	{
	AltarClipList::erase( getIndex( item ) );
	}

//Erase the item at the given index from the list
void AltarClipList::erase( unsigned int index )
	{	

	thumbnailCache.removeThumb( getItem( index )->getThumbnail().getHashCode() );
	AltarList::erase( index );
	}

//Erase all items
void AltarClipList::clear()
	{
	AltarList::clear();
	thumbnailCache.clear();
	}

void AltarClipList::addClipFromAudio( std::shared_ptr<flan::Audio> a )
	{
	insertClipFromAudio( a, getNumItems() );
	}

//
void AltarClipList::insertClipFromAudio( std::shared_ptr<flan::Audio> a, size_t index )
	{
	insertItem( new AltarClip( a, formatManager, thumbnailCache, transportSource ), index );
	}

bool AltarClipList::isInterestedInDragSource( const SourceDetails & dragSourceDetails )
{
	return dragSourceDetails.description == "Clip";
}

//This function sucks but it works
void AltarClipList::itemDropped( const SourceDetails & s )
	{
	// auto parent = static_cast< AltarClipList * >( s.sourceComponent->getParentComponent() );
	// auto item = static_cast< AltarClip * >( s.sourceComponent.get() );

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


