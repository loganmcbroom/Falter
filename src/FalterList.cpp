#include "FalterList.h"

#include "FalterButton.h"
#include "FalterLookAndFeel.h"

#define CLEAR_HEIGHT 25
#define ERASE_WIDTH 25

using namespace std;

template <typename T>
FalterList<T>::FalterList()
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

template <class T>
FalterList<T>::~FalterList()
	{
	}

template<typename T>
shared_ptr<T> FalterList<T>::addItem( T * item )
	{
	return insertItem( item, items.size() );
	}

template<typename T>
shared_ptr<T> FalterList<T>::insertItem( T * item, size_t index )
	{
	if( index < 0 ) index = 0;
	FalterButton * button = new FalterButton( "r", &FalterLookAndFeel::getLNF().fontWebdings, int( ERASE_WIDTH / 2.0f ) );
	items.emplace( items.begin() + index, item, button );
	addAndMakeVisible( item );
	addAndMakeVisible( button );
	button->addListener( this );

	scroll.setRangeLimits( 0, float( getItemHeight() ) * float( items.size() ) + CLEAR_HEIGHT, 
		NotificationType::dontSendNotification );
	resized();

	clearButton->toFront( false );
	return items[index].first;
}

template<typename T>
int FalterList<T>::getIndex( T * clip )
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

template<typename T>
void FalterList<T>::erase( int index )
	{
	items.erase( items.begin() + index );
	scroll.setRangeLimits( 0, float( getItemHeight() ) * float( items.size() ) + CLEAR_HEIGHT, NotificationType::dontSendNotification );
	resized();
	}

template<typename T>
void FalterList<T>::erase( T * item )
	{
	erase( getIndex( item ) );
	}

template<typename T>
void FalterList<T>::clear()
	{
	items.clear();
	}

template<typename T>
void FalterList<T>::swap( int a, int b )
	{
	iter_swap( items.begin() + a, items.begin() + b );
	resized();
	}

template<typename T>
shared_ptr<T> FalterList<T>::getItem( int index )
	{
	return items[index].first; 
	}

template<typename T>
shared_ptr<T> FalterList<T>::getItem( Component * c )
{
	return getItem( getIndex( static_cast< T * >( c ) ) );
}


// Private -----------------------------------------------------------

// Paint
template<typename T>
void FalterList<T>::paint( Graphics & g )
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	}

// Resized
template <class T>
void FalterList<T>::resized()
	{ 
	bool scrollVisible = float( getItemHeight() ) * float( getNumItems() ) > getHeight();
	scroll.setCurrentRange( scroll.getCurrentRangeStart(), getHeight(), NotificationType::dontSendNotification );

	positionItems( scrollVisible );

	scroll.setBounds( getWidth() - 8, CLEAR_HEIGHT, 8, getHeight() );

	clearButton->setBounds( juce::Rectangle<int>( 0, 0, getWidth(), CLEAR_HEIGHT ) );
	}

// Repositions all the items based on relevant info
template <class T>
void FalterList<T>::positionItems( bool scrollVisible )
	{
	for( int i = 0; i < items.size(); ++i )
		{
		juce::Rectangle<int> rect( 
			0, 
			i * getItemHeight() - int( scroll.getCurrentRangeStart() ) + CLEAR_HEIGHT, 
			getWidth() - ( scrollVisible? 8 : 0 ), 
			getItemHeight() 
			);
		items[i].first-> setBounds( rect.withLeft ( ERASE_WIDTH ).reduced( 2 ) );
		items[i].second->setBounds( rect.withRight( ERASE_WIDTH ).reduced( 2 ) );
		}
	}

// Callback for the scroll object having changed
template<typename T>
void FalterList<T>::scrollBarMoved( ScrollBar *, double )
	{
	positionItems( true );
	}

// Callback for scrolling with mouse
template<typename T>
void FalterList<T>::mouseWheelMove( const MouseEvent &, const MouseWheelDetails & wheel )
	{
	scroll.setCurrentRangeStart( scroll.getCurrentRangeStart() - wheel.deltaY * getItemHeight() * 4 );
	}

//
template<typename T>
void FalterList<T>::buttonClicked( Button * b )
	{
	if( b == clearButton.get() )
		{
		clear();
		}
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

//Instantiate templates
#include "FalterClip.h"
template class FalterList<FalterClip>;
#include "FalterThread.h"
template class FalterList<FalterThread>;
