#pragma once

#include <JuceHeader.h>
#include "FalterList.h"
#include "FalterClip.h"

class FalterClipList : public FalterList<FalterClip>
					, public DragAndDropTarget
	{
public:
	FalterClipList( AudioFormatManager &_formatManager, AudioTransportSource &_transportSource );
	~FalterClipList();

	int getItemHeight() override;
	void erase( FalterClip * child );
	void erase( unsigned int pos );
	void clear();

	void addClipFromAudio( flan::Audio );
	void insertClipFromAudio( flan::Audio file, size_t index );

private:
	bool isInterestedInDragSource( const SourceDetails & dragSourceDetails ) override;
	///void itemDragEnter	( const SourceDetails &dragSourceDetails ) override;
	///void itemDragMove	( const SourceDetails &dragSourceDetails ) override;
	///void itemDragExit	( const SourceDetails &dragSourceDetails ) override;
	void itemDropped	( const SourceDetails &dragSourceDetails ) override;
	///bool shouldDrawDragImageWhenOver() override;


	AudioTransportSource &transportSource;
	AudioFormatManager &formatManager;
	AudioThumbnailCache thumbnailCache;
	};


