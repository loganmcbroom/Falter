#pragma once

#include <functional>

#include <flan/Audio/Audio.h>

#include <JuceHeader.h>

#include "./Types.h"

#include "FalterLogger.h"

using FalterThreadCallback = std::function< void( AudioVec &, const String & ) >;

class FalterThread : public Thread
				  , public Component
				  , public Timer
				  , public Thread::Listener
{
public:
	FalterThread( 
		int threadID,
		const File & script, 
		const FalterThreadCallback & callback,
		const AudioVec & files = AudioVec()
		);
	~FalterThread();

	void log( const String & s ); // Passes to Logger with added thread info and lock
	std::atomic<bool> & getCanceller();
	
private:
	void paint( Graphics & g ) override;
	void run() override; // Called on the new thread when startThread is called on this thread
	void timerCallback() override;

	String getStartTimeString() const;
	String getElapsedTimeString() const;

	void exitSignalSent() override;
	
	int ID;
	FalterThreadCallback callback;
	const File scriptFile;
	lua_State * L;
	juce::Time startTime;
	juce::Time endTime;
	bool threadSuccess;
	bool allProcessesSetUp;
	std::atomic<bool> canceller;

	AudioVec outputs;
};