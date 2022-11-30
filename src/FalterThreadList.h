#pragma once

#include "FalterList.h"
#include "FalterThread.h"

class FalterThreadList : public FalterList
{
public:
	FalterThreadList();
	~FalterThreadList();

	FalterThread & addThread( const String & script, const FalterThreadCallback & callback, const AudioVec & files );

private:
};