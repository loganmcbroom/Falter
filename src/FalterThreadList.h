#pragma once

#include "FalterList.h"
#include "FalterThread.h"

class FalterThreadList : public FalterList
					   , public Timer // Every ten seconds the dying threads are checked for completion and destroyed if complete
{
public:
	FalterThreadList();
	~FalterThreadList();

	FalterThread & addThread( const String & script, const FalterThreadCallback & callback, const AudioVec & files );

	virtual void erase( int index ) override;
	virtual void erase( Component * item ) override;

private:
	virtual void timerCallback() override;

	std::vector<std::shared_ptr<Component>> dyingThreads;
};