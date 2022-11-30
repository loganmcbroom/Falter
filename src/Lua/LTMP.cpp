#include "LTMP.h"

#include "./FalterThread.h"

std::atomic<bool> & getThreadCanceller( lua_State * L )
	{
	auto * thread = dynamic_cast<FalterThread *>( Thread::getCurrentThread() );
    if( ! thread )
        luaL_error( L, "Thread was not Falter thread in LTMP." );
    return thread->getCanceller();
	}


