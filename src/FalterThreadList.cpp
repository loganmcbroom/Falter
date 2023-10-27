#include "FalterThreadList.h"

#include <string>

#include <flan/Audio/Audio.h>

#include "FalterLogger.h"

FalterThreadList::FalterThreadList()
	{
	startTimer( 10*1000 );
	}

FalterThreadList::~FalterThreadList()
	{
	}

FalterThread & FalterThreadList::addThread( const String & script, const FalterThreadCallback & callback, const AudioVec & files )
	{
	static int n = 0; // Used for thread names
	++n;
	Time t = Time::getCurrentTime();
	auto newThread = std::make_shared<FalterThread>( n, script, callback, files );
	addItem( newThread );
	return *newThread;
	}

void FalterThreadList::erase( int index )
	{
	auto thread_sp = std::dynamic_pointer_cast<FalterThread>( items[index].first );
	jassert( thread_sp );
	dyingThreads.push_back( thread_sp );
	thread_sp->setVisible( false );
	thread_sp->signalThreadShouldExit();
	FalterList::erase( index );
	}

void FalterThreadList::erase( Component * item ) 
	{
	FalterThreadList::erase( getIndex( item ) );
	}

void FalterThreadList::timerCallback()
	{
	std::erase_if( dyingThreads, []( std::shared_ptr<Component> c )
		{
		auto thread = std::dynamic_pointer_cast<FalterThread>( c );
		return ! thread->isThreadRunning();
		} );
	}


