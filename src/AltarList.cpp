#include "AltarList.h"

#include "AltarButton.h"
#include "FalterLookAndFeel.h"

#define CLEAR_HEIGHT 25
#define ERASE_WIDTH 25

using namespace std;

template <typename T>
AltarList<T>::AltarList()
	: Component()
	, scroll( true )
	, clearButton( new AltarButton("r", &FalterLookAndFeel::getLNF().fontWebdings, int( CLEAR_HEIGHT * .67f ) ) )
	{
	addAndMakeVisible( &scroll );
	scroll.addListener( this );
	scroll.setRangeLimits( 0, 1, NotificationType::dontSendNotification );
	scroll.setCurrentRange( 0, 1, NotificationType::dontSendNotification );

	addAndMakeVisible( clearButton.get() );
	clearButton->addListener( this );
	}

template <class T>
AltarList<T>::~AltarList()
	{
	}

template<typename T>
shared_ptr<T> AltarList<T>::addItem( T * item )
	{
	return insertItem( item, items.size() );
	}

template<typename T>
shared_ptr<T> AltarList<T>::insertItem( T * item, size_t index )
	{
	if( index < 0 ) index = 0;
	AltarButton * button = new AltarButton( "r", &FalterLookAndFeel::getLNF().fontWebdings, int( ERASE_WIDTH / 2.0f ) );
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
int AltarList<T>::getIndex( T * clip )
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
void AltarList<T>::erase( int index )
	{
	items.erase( items.begin() + index );
	scroll.setRangeLimits( 0, float( getItemHeight() ) * float( items.size() ) + CLEAR_HEIGHT, NotificationType::dontSendNotification );
	resized();
	}

template<typename T>
void AltarList<T>::erase( T * item )
	{
	erase( getIndex( item ) );
	}

template<typename T>
void AltarList<T>::clear()
	{
	items.clear();
	}

template<typename T>
void AltarList<T>::swap( int a, int b )
	{
	iter_swap( items.begin() + a, items.begin() + b );
	resized();
	}

template<typename T>
shared_ptr<T> AltarList<T>::getItem( int index )
	{
	return items[index].first; 
	}

template<typename T>
shared_ptr<T> AltarList<T>::getItem( Component * c )
{
	return getItem( getIndex( static_cast< T * >( c ) ) );
}


// Private -----------------------------------------------------------

// Paint
template<typename T>
void AltarList<T>::paint( Graphics & g )
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	}

// Resized
template <class T>
void AltarList<T>::resized()
	{ 
	bool scrollVisible = float( getItemHeight() ) * float( getNumItems() ) > getHeight();
	scroll.setCurrentRange( scroll.getCurrentRangeStart(), getHeight(), NotificationType::dontSendNotification );

	positionItems( scrollVisible );

	scroll.setBounds( getWidth() - 8, CLEAR_HEIGHT, 8, getHeight() );

	clearButton->setBounds( juce::Rectangle<int>( 0, 0, getWidth(), CLEAR_HEIGHT ) );
	}

// Repositions all the items based on relevant info
template <class T>
void AltarList<T>::positionItems( bool scrollVisible )
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
void AltarList<T>::scrollBarMoved( ScrollBar *, double )
	{
	positionItems( true );
	}

// Callback for scrolling with mouse
template<typename T>
void AltarList<T>::mouseWheelMove( const MouseEvent &, const MouseWheelDetails & wheel )
	{
	scroll.setCurrentRangeStart( scroll.getCurrentRangeStart() - wheel.deltaY * getItemHeight() * 4 );
	}

//
template<typename T>
void AltarList<T>::buttonClicked( Button * b )
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
#include "AltarClip.h"
template class AltarList<AltarClip>;
#include "AltarThread.h"
template class AltarList<AltarThread>;
