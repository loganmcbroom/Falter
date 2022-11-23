#pragma once

#include "FalterList.h"
#include "FalterThread.h"

class FalterThreadList : public FalterList
{
public:
	FalterThreadList();
	~FalterThreadList();

	void addThread( const String & script, std::function< void( AudioVec & ) > & callback, const AudioVec & files );

private:
};