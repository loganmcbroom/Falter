#include "FalterList.h"

#include "FalterButton.h"
#include "FalterLookAndFeel.h"

#define CLEAR_HEIGHT 25
#define ERASE_WIDTH 25

using namespace std;

FalterList::FalterList()
	: Component()
	, scroll( true )
	, clearButton( new FalterButton("r", &FalterLookAndFeel::getLNF().fontWebdings, int( CLEAR_HEIGHT * .67f ) ) )
	{
	addAndMakeVisible( &scroll );
	scroll.addListener( this );
	scroll.setRangeLimits( 0, 1, NotificationType::dontSendNotification );
	scroll.setCurrentRange( 0, 1, NotificationType::dontSendNotification );

	addAndMakeVisible( clearButton.get() );
	clearButton->addListener( this );
	}

FalterList::~FalterList()
	{
	}

shared_ptr<Component> FalterList::addItem( Component * item )
	{
	return insertItem( item, items.size() );
	}

shared_ptr<Component> FalterList::insertItem( Component * item, size_t index )
	{
	if( index < 0 ) index = 0;
	FalterButton * button = new FalterButton( "r", &FalterLookAndFeel::getLNF().fontWebdings, int( ERASE_WIDTH / 2.0f ) );
	items.emplace( items.begin() + index, item, button );
	addAndMakeVisible( item );
	addAndMakeVisible( button );
	button->addListener( this );

	scroll.setRangeLimits( 0, float( itemHeight ) * float( items.size() ) + CLEAR_HEIGHT, 
		NotificationType::dontSendNotification );
	resized();

	clearButton->toFront( false );
	return items[index].first;
}

int FalterList::getIndex( Component * clip )
	{
	for( int i = 0; i < getNumItems(); ++i )
		{
		if( items[i].first.get() == clip )
			{
			return i;
			}
		}
	return -1;
	}

void FalterList::erase( int index )
	{
	items.erase( items.begin() + index );
	scroll.setRangeLimits( 0, float( itemHeight ) * float( items.size() ) + CLEAR_HEIGHT, NotificationType::dontSendNotification );
	resized();
	}

void FalterList::erase( Component * item )
	{
	erase( getIndex( item ) );
	}

void FalterList::clear()
	{
	items.clear();
	}

void FalterList::swap( int a, int b )
	{
	iter_swap( items.begin() + a, items.begin() + b );
	resized();
	}

shared_ptr<Component> FalterList::getItem( int index )
	{
	return items[index].first; 
	}

shared_ptr<Component> FalterList::getItem( Component * c )
{
	return getItem( getIndex( c ) );
}


// Private -----------------------------------------------------------

// Paint
void FalterList::paint( Graphics & g )
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	}

// Resized
void FalterList::resized()
	{ 
	bool scrollVisible = float( itemHeight ) * float( getNumItems() ) > getHeight();
	scroll.setCurrentRange( scroll.getCurrentRangeStart(), getHeight(), NotificationType::dontSendNotification );

	positionItems( scrollVisible );

	scroll.setBounds( getWidth() - 8, CLEAR_HEIGHT, 8, getHeight() );

	clearButton->setBounds( juce::Rectangle<int>( 0, 0, getWidth(), CLEAR_HEIGHT ) );
	}

// Repositions all the items based on relevant info
void FalterList::positionItems( bool scrollVisible )
	{
	for( int i = 0; i < items.size(); ++i )
		{
		juce::Rectangle<int> rect( 
			0, 
			i * itemHeight - int( scroll.getCurrentRangeStart() ) + CLEAR_HEIGHT, 
			getWidth() - ( scrollVisible? 8 : 0 ), 
			itemHeight 
			);
		items[i].first-> setBounds( rect.withLeft ( ERASE_WIDTH ).reduced( 2 ) );
		items[i].second->setBounds( rect.withRight( ERASE_WIDTH ).reduced( 2 ) );
		}
	}

// Callback for the scroll object having changed
void FalterList::scrollBarMoved( ScrollBar *, double )
	{
	positionItems( true );
	}

// Callback for scrolling with mouse
void FalterList::mouseWheelMove( const MouseEvent &, const MouseWheelDetails & wheel )
	{
	scroll.setCurrentRangeStart( scroll.getCurrentRangeStart() - wheel.deltaY * itemHeight * 4 );
	}

//
void FalterList::buttonClicked( Button * b )
	{
	if( b == clearButton.get() )
		clear();
	else //One of the erase buttons was pressed
		{
		for( int i = 0; i < getNumItems(); ++i )
			{
			if( items[i].second.get() == b )
				{
				erase( i );
				return;
				}
			}
		}
	}


