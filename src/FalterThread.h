#pragma once

#include <functional>

#include <flan/Audio.h>

#include <JuceHeader.h>

#include "./Types.h"

#include "FalterLogger.h"

using FalterThreadCallback = std::function< void( AudioVec &, const String & ) >;

struct FalterThreadListener : public Thread::Listener
{
	FalterThreadListener() : canceller( false ) {}
	void exitSignalSent() override { canceller = true; }
	std::atomic<bool> canceller;
};

class FalterThread : public Thread
				  , public Component
				  , public Timer
{
public:
	FalterThread( 
		int threadID,
		const String & script, 
		const FalterThreadCallback & callback,
		const AudioVec & files = AudioVec() );
	~FalterThread();

	void log( const String & s ); // Passes to Logger with added thread info and lock
	std::atomic<bool> & getCanceller();
	
private:
	void paint( Graphics & g ) override;
	void run() override; // Called on the new thread when startThread is called on this thread
	void timerCallback() override;

	String getStartTimeString() const;
	String getElapsedTimeString() const;

	int ID;
	FalterThreadCallback callback;
	const String script;
	lua_State * L;
	juce::Time startTime;
	juce::Time endTime;
	String cdpDir;
	File workingDir;
	bool threadFinished;
	bool threadSuccess;
	bool allProcessesSetUp;
	FalterThreadListener listener;

	static CriticalSection mutex;
};