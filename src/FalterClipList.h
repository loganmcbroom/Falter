#pragma once

#include <JuceHeader.h>
#include "FalterList.h"
#include "FalterClip.h"

class FalterPlayer;

class FalterClipList : public FalterList
					, public DragAndDropTarget
	{
public:
	FalterClipList( FalterPlayer & player );
	~FalterClipList();

	void erase( FalterClip * child );
	void erase( unsigned int pos );
	void clear();

	void addClipFromAudio( flan::Audio, const String & name = "-" );
	void insertClipFromAudio( flan::Audio file, size_t index, const String & name = "-" );

private:
	// DragAndDropTarget interface
	bool isInterestedInDragSource( const SourceDetails & dragSourceDetails ) override;
	//void itemDragEnter( const SourceDetails &dragSourceDetails ) override;
	//void itemDragMove( const SourceDetails &dragSourceDetails ) override;
	//void itemDragExit( const SourceDetails &dragSourceDetails ) override;
	void itemDropped( const SourceDetails &dragSourceDetails ) override;

	FalterPlayer & player;
	AudioThumbnailCache thumbnailCache;
	};


