#include "FalterThread.h"

#include <vector>
#include <sstream>
#include <fstream>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "FalterLookandFeel.h"

#include "Lua/Utility.h"
#include "Lua/Usertypes.h"

static int lua_print( lua_State * L ) 
	{
    const int nargs = lua_gettop( L );
	FalterThread * T = static_cast<FalterThread *>( Thread::getCurrentThread() );
    for( int i = 1; i <= nargs; ++i ) 
		{
        if( lua_isstring( L, i ) )	T->log( lua_tostring( L, i ) );
        else						T->log( "The script tried to print a non-string in lua." );
		}
    return 0;
	}

// Array of name/function pairs to be registered
static const struct luaL_Reg printlib [] =
	{
	{"print", lua_print},
	{NULL, NULL} /* end of array */
	};

CriticalSection FalterThread::mutex;

FalterThread::FalterThread( 
	const String & name, 
	const String & _script, 
	std::function< void( AudioVec & ) > & _callback, 
	const AudioVec & inputs )
	: Thread( name )
	, callback( _callback )
	, script( _script )
	, L( lua_open() )
	, startTime( )
	, endTime( )
	, cdpDir( )
	, workingDir( )
	, threadFinished( false )
	, threadSuccess( false )
	, allProcessesSetUp( true )
	, listener()
	{
	addListener( &listener );

	bool startupError = false;
	auto err = [&]( const char * error )
		{
		log( String("[Thread Setup] ") + String( error ) + "\n" );
		startupError = true;
		};

	auto luaerr = [&]( lua_State * L )
		{
		err( lua_tostring(L, -1) );
		};

	luaL_openlibs( L );

	// Redefine Lua's print function to our logging function
	lua_getglobal( L, "_G" );
	luaL_register( L, NULL, printlib );
	lua_pop( L, 1 );

	// Get stuff out of settings file first
	// if( luaL_loadfile( L, ( File::getCurrentWorkingDirectory().getFullPathName() + "/Settings.lua" ).toRawUTF8() ) || lua_pcall( L, 0, 0, 0 ) ) 
	// 	err( lua_tostring(L, -1) );
	// lua_getglobal( L, "cdpDir" );
	// if( ! lua_isstring( L, -1 ) )
	// 	err( lua_tostring(L, -1) );
	
	lua_settop( L, 0 ); //Clear the stack
	luaF_register_Usertypes( L );

	// Create global "f" with all the input file paths and metatable to stop bad array access
	luaF_push( L, inputs );

	lua_setglobal( L, "f" );

	// Load the supplied script
	if( luaL_loadfile( L, script.getCharPointer() ) )
		{
		luaerr( L );
		return;
		}

	repaint();

	//Boot it up!
	if( ! startupError ) 
		{
		startTime = Time::getCurrentTime(); 
		startTimer( 33 );
		startThread();
		}
	}

FalterThread::~FalterThread()
	{
	if( isThreadRunning() )
		stopThread( -1 );

	lua_close( L );
	}

void FalterThread::log( const String & s )
	{
	MessageManagerLock mml;
	Logger::writeToLog( s );
	}

std::atomic<bool> & FalterThread::getCanceller()
	{
	return listener.canceller;
	}

void FalterThread::paint( Graphics & g )
	{
	auto & lnf = FalterLookAndFeel::getLNF();
	
	// Draw background
	g.setColour( lnf.shadow );
	g.fillRect( getLocalBounds() );

	// Draw info text
	const auto bound = getLocalBounds().reduced( 6 );
	g.setFont( lnf.fontMonospace );
	g.setColour( threadFinished? threadSuccess? lnf.accent2 : lnf.accent1 :lnf.light );
	g.drawText( "Start:", bound, Justification::topLeft );
	g.drawText( getStartTimeString(), bound, Justification::topRight );
	g.drawText( "Elapsed:", bound, Justification::bottomLeft );
	g.drawText( getElapsedTimeString(), bound, Justification::bottomRight );
	}

void FalterThread::run()
	{
	auto exit = [&]()
		{
		// Success or not these need to be called
		const ScopedLock lock( mutex );
		MessageManagerLock mml;
		stopTimer();
		threadFinished = true;
		endTime = Time::getCurrentTime();
		repaint();
		};

	if( threadShouldExit() )
		{
		log( "Exiting thread early due to startup error.\n" );
		exit();
		return;
		}
	
	if( lua_gettop( L ) != 1 ) // There should only be the script
		{
		log( "Lua stack size was not 1 when starting a new thread, stack size was " + String( lua_gettop( L ) ) + ".\n" );
		exit();
		return;
		}

	log( "[PROCESSING START]" );

	// ...Make the call
	if( ! lua_pcall( L, 0, LUA_MULTRET, 0 ) ) // if call succeeds
		{
		log( String( "[PROCESSING END] Time elapsed: " ) + getElapsedTimeString() + "\n" );

		threadSuccess = true;	

		// Pull all the audios returned from lua into a container and send them to the callback
		AudioVec outputs;
		const int numLuaOutputs = lua_gettop( L );
		for( int i = 1; i <= numLuaOutputs; ++i )
			{
			AudioVec currentReturn = luaF_checkAsArray<flan::Audio>( L, i );
			outputs.insert( outputs.end(), currentReturn.begin(), currentReturn.end() );
			}
		const ScopedLock lock( mutex );
		MessageManagerLock mml;
		callback( outputs );
		}
	else
		{
		// We can't do this stuff if the failure is due to the user exiting the thread early
		if( ! threadShouldExit() ) 
			{
			log( std::string("[Lua] Error: ") + std::string( lua_tostring(L, -1) ) );
			log( String( "[PROCESSING FAILED] Time elapsed: " ) + getElapsedTimeString() + "\n" );
			}
		}

	exit();
	}

void FalterThread::timerCallback()
	{
	repaint();
	}

String FalterThread::getStartTimeString() const
	{
	return startTime.formatted( "%H:%M:%S." ) + String( startTime.getMilliseconds() );
	}

String FalterThread::getElapsedTimeString() const
	{
	const uint64_t startTimeMS = startTime.toMilliseconds();
	const uint64_t endTimeMS = threadFinished? endTime.toMilliseconds() : Time::currentTimeMillis();
	const uint64_t elapsedMS = endTimeMS - startTimeMS;
	return juce::Time( elapsedMS ).formatted( "%M:%S." ) + String( elapsedMS % 1000 );
	}
