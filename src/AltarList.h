#pragma once

#include <JuceHeader.h>

class AltarButton;

template < typename T >
class AltarList  : public Component
				 , public ScrollBar::Listener
				 , public Button::Listener
	{
public:
	AltarList();
	~AltarList();

	virtual int getItemHeight() = 0;
	
	int getNumItems() { return int( items.size() ); };
	std::shared_ptr<T> addItem( T * item );
	std::shared_ptr<T> insertItem( T * item, size_t index );
	int getIndex( T * item );
	void erase( int index );
	void erase( T * item );
	void clear();
	void swap( int a, int b );
	std::shared_ptr<T> getItem( int index );
	std::shared_ptr<T> getItem( Component * c );

protected:
	void buttonClicked( Button * b ) override;

private:
	void paint( Graphics & ) override;
	void resized() override;
	void positionItems( bool scrollVisible );
	void scrollBarMoved( ScrollBar * scrollBarThatHasMoved, double newRangeStart ) override;
	void mouseWheelMove(const MouseEvent & event, const MouseWheelDetails & wheel ) override;
	
	
	std::vector< std::pair< std::shared_ptr<T>, std::unique_ptr<AltarButton> > > items;

	ScrollBar scroll;

	std::unique_ptr<AltarButton> clearButton;
	};