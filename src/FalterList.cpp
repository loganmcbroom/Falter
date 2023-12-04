#include "FalterList.h"

#include "FalterButton.h"
#include "FalterLookAndFeel.h"

static const int CLEAR_HEIGHT = 25;
static const int ERASE_WIDTH = 25;

FalterList::FalterList()
	: Component()
	, scroll( true )
	, clearButton( std::make_unique<FalterButton>("r", &FalterLookAndFeel::getLNF().fontWebdings, int( CLEAR_HEIGHT * .67f ) ) )
	, changingItemsLock()
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

std::shared_ptr<Component> FalterList::addItem( std::shared_ptr<Component> item )
	{
	return insertItem( item, items.size() );
	}

std::shared_ptr<Component> FalterList::insertItem( std::shared_ptr<Component> item, size_t index )
	{
	auto button = std::make_unique<FalterButton>( "r", &FalterLookAndFeel::getLNF().fontWebdings, int( ERASE_WIDTH / 2.0f ) );
	button->addListener( this );
	addAndMakeVisible( button.get() );
	addAndMakeVisible( item.get() );

		{
		const ScopedWriteLock scoperLock( changingItemsLock );
		items.emplace( items.begin() + index, item, std::move( button ) );
		}

	scroll.setRangeLimits( 0, float( itemHeight ) * float( items.size() ) + CLEAR_HEIGHT, NotificationType::dontSendNotification );
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
		{
		const ScopedWriteLock scoperLock( changingItemsLock );
		items.erase( items.begin() + index );
		}
	scroll.setRangeLimits( 0, float( itemHeight ) * float( items.size() ) + CLEAR_HEIGHT, NotificationType::dontSendNotification );
	resized();
	}

void FalterList::erase( Component * item )
	{
	erase( getIndex( item ) );
	}

void FalterList::clear()
	{
	for( int i = items.size() - 1; i >= 0; --i )
		erase( i );
	}

void FalterList::swap( int a, int b )
	{
		{
		const ScopedWriteLock scoperLock( changingItemsLock );
		iter_swap( items.begin() + a, items.begin() + b );
		}
	resized();
	}

std::shared_ptr<Component> FalterList::getItem( int index )
	{
	return items[index].first; 
	}

std::shared_ptr<Component> FalterList::getItem( Component * c )
	{
	return getItem( getIndex( c ) );
	}

void FalterList::setClearButtonText( const String & s )
	{
	clearButton->setButtonText( s );
	}

// Private -----------------------------------------------------------

// Paint
void FalterList::paint( Graphics & g )
	{
	g.fillAll( FalterLookAndFeel::getLNF().dark );
	g.setColour( FalterLookAndFeel::getLNF().light.withAlpha( 0.3f ) );
	g.drawText( getName(), getLocalBounds(), Justification::centred );
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


