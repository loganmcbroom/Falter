#pragma once

#include <JuceHeader.h>
#include "AltarList.h"
#include "AltarClip.h"

class AltarClipList : public AltarList<AltarClip>
					, public DragAndDropTarget
	{
public:
	AltarClipList( AudioFormatManager &_formatManager, AudioTransportSource &_transportSource );
	~AltarClipList();

	int getItemHeight() override;
	void erase( AltarClip * child );
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


