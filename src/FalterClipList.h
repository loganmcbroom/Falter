#pragma once

#include <JuceHeader.h>
#include "FalterList.h"
#include "FalterClip.h"

class FalterPlayer;
struct AudioLoadThread;

class FalterClipList : public FalterList
					 , public DragAndDropTarget
					 , public Thread::Listener
	{
public:
	FalterClipList( FalterPlayer & player );
	~FalterClipList();

	void erase( FalterClip * child );
	void erase( unsigned int pos );
	void clear();

	void insertClipFromAudio( std::shared_ptr<flan::Audio> audio, int index = -1, const String & name = "-" );
	void insertClipFromFile( std::shared_ptr<flan::Audio> file, int index = -1, const File & source = File() );
	void importAudioFileAsync( const File & file );

private:
	// DragAndDropTarget interface
	bool isInterestedInDragSource( const SourceDetails & dragSourceDetails ) override;
	//void itemDragEnter( const SourceDetails &dragSourceDetails ) override;
	//void itemDragMove( const SourceDetails &dragSourceDetails ) override;
	//void itemDragExit( const SourceDetails &dragSourceDetails ) override;
	void itemDropped( const SourceDetails & dragSourceDetails ) override;

	void exitSignalSent() override;

	FalterPlayer & player;
	AudioThumbnailCache thumbnailCache;
	ThreadPool threadPool;
	};


