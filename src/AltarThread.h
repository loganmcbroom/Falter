#pragma once

#include <functional>

#include <JuceHeader.h>

#include "AltarLogger.h"

#include "./Types.h"

class AltarThread : public Thread
				  , public Component
{
public:
	AltarThread( 
		const String & name, 
		const String & script, 
		std::function< void( AudioVec & ) > & callback,
		const AudioVec & files = AudioVec() );
	~AltarThread();

	void log( const String & s ); // Passes to Logger with added thread info and lock
	
private:
	void paint( Graphics & g ) override;
	void run() override; // Called on the new thread when startThread is called on this thread

	std::function<void( AudioVec & )> callback;
	const String script;
	lua_State * L;
	int64 startTime;
	String cdpDir;
	File workingDir;
	bool threadFinished;
	bool allProcessesSetUp;

	static CriticalSection mutex;
};