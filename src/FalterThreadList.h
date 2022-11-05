#pragma once

#include "FalterList.h"
#include "FalterThread.h"

class FalterThreadList : public FalterList< FalterThread >
{
public:
	FalterThreadList();
	~FalterThreadList();

	int getItemHeight() override { return 50; }

	void addThread( const String & script, std::function< void( AudioVec & ) > & callback, const AudioVec & files );

private:
};