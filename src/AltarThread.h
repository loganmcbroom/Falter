#pragma once

#include <functional>

#include <JuceHeader.h>

#include "AltarLogger.h"

#include "./Types.h"

class AltarThread : public Thread
				  , public Component
				  , public Timer
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
	void timerCallback() override;

	String getStartTimeString() const;
	String getElapsedTimeString() const;

	std::function<void( AudioVec & )> callback;
	const String script;
	lua_State * L;
	juce::Time startTime;
	juce::Time endTime;
	String cdpDir;
	File workingDir;
	bool threadFinished;
	bool threadSuccess;
	bool allProcessesSetUp;

	static CriticalSection mutex;
};