#pragma once

#include <JuceHeader.h>

class FalterButton;

class FalterList  : public Component
				 , public ScrollBar::Listener
				 , public Button::Listener
	{
public:
	FalterList();
	~FalterList();
	
	int getNumItems() { return int( items.size() ); };
	std::shared_ptr<Component> addItem( Component * item );
	std::shared_ptr<Component> insertItem( Component * item, size_t index );
	int getIndex( Component * item );
	void erase( int index );
	void erase( Component * item );
	void clear();
	void swap( int a, int b );
	std::shared_ptr<Component> getItem( int index );
	std::shared_ptr<Component> getItem( Component * c );

	void setClearButtonText( const String & );

	static const size_t itemHeight = 44;

protected:
	void buttonClicked( Button * b ) override;

private:
	void paint( Graphics & ) override;
	void resized() override;
	void positionItems( bool scrollVisible );
	void scrollBarMoved( ScrollBar * scrollBarThatHasMoved, double newRangeStart ) override;
	void mouseWheelMove(const MouseEvent & event, const MouseWheelDetails & wheel ) override;
	
	std::vector< std::pair< std::shared_ptr<Component>, std::unique_ptr<FalterButton> > > items;

	ScrollBar scroll;

	std::unique_ptr<FalterButton> clearButton;
	};