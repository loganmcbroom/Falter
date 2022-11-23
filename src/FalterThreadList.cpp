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

void FalterThreadList::addThread( const String & script, std::function< void ( AudioVec & ) > & callback, const AudioVec & files )
	{
	static int n = 0; // Used for thread names
	++n;
	Time t = Time::getCurrentTime();
	addItem( new FalterThread( 
		String( n ) + " " + script,
			//+ t.formatted( "%M:%S." ) + String( t.getMilliseconds() ), 
		script, 
		callback, 
		files ) );
	}

