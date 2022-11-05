#include "AltarThreadList.h"

#include <string>

#include <flan/Audio.h>

#include "AltarLogger.h"

AltarThreadList::AltarThreadList()
	{
	}

AltarThreadList::~AltarThreadList()
	{
	}

void AltarThreadList::addThread( const String & script, std::function< void ( AudioVec & ) > & callback, const AudioVec & files )
	{
	static int n = 0;
	++n;
	Time t = Time::getCurrentTime();
	addItem( new AltarThread( 
		String( n ) + " " + script,
			//+ t.formatted( "%M:%S." ) + String( t.getMilliseconds() ), 
		script, 
		callback, 
		files ) );
	}

