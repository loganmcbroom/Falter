#include "FalterThreadList.h"

#include <string>

#include <flan/Audio.h>

#include "FalterLogger.h"

FalterThreadList::FalterThreadList()
	{
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

