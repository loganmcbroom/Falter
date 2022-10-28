#include "AltarThread.h"

#include <vector>
#include <sstream>
#include <fstream>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <flan/Audio.h>

#include "FalterLookandFeel.h"

#include "Lua/Audio.h"
#include "Lua/Function.h"

static int lua_print( lua_State * L ) 
	{
    const int nargs = lua_gettop( L );
	AltarThread * T = static_cast<AltarThread *>( Thread::getCurrentThread() );
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

CriticalSection AltarThread::mutex;

AltarThread::AltarThread( 
	const String & name, 
	const String & _script, 
	std::function< void( AudioVec & ) > & _callback, 
	const AudioVec & inputs )
	: Thread( name )
	, callback( _callback )
	, script( _script )
	, L( lua_open() )
	, startTime( )
	, cdpDir( )
	, workingDir( )
	, threadFinished( false )
	, allProcessesSetUp( true )
	//, inputs( _inputs )
	{
	bool startupError = false;
	auto err = [&]( const char * error )
		{
		log( String("[Thread Setup] ") + String( error ) );
		startupError = true;
		};

	auto luaerr = [&]( lua_State * L )
		{
		log( std::string("[Thread Setup] ") + std::string( lua_tostring(L, -1) ) );
		startupError = true;
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
	luaF_register_Audio( L ); // Register Audio class into the Lua context
	luaF_register_function_types( L ); // Register all function types into the Lua context

	// Create global "f" with all the input file paths and metatable to stop bad array access
	luaF_pushAudioVec( L, inputs );
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
		startTime = Time::getMillisecondCounter(); 
		startThread();
		}
	}

AltarThread::~AltarThread()
	{
	if( isThreadRunning() )
		stopThread( -1 );

	lua_close( L );
	}

void AltarThread::log( const String & s )
	{
	MessageManagerLock mml;
	Logger::writeToLog( "{Thread: " + getThreadName() + "} " + s );
	}

// bool AltarThread::isInput( flan::Audio * a )
// 	{
// 	// for( auto & _a : inputs )
// 	// 	if( _a.get() == a )
// 	// 		return true;
// 	return false;
// 	}

void AltarThread::paint( Graphics & g )
	{
	auto & lnf = FalterLookAndFeel::getLNF();
	g.fillAll( threadFinished? lnf.accent2 : lnf.mid );

	g.setFont( Font( 18 ) );
	g.setColour( lnf.light );
	g.drawText( getThreadName().substring( 5 ), getLocalBounds(), Justification::centred );
	}

void AltarThread::run()
	{
	auto luaerr = [&]( lua_State * L )
		{
		log( std::string("[Lua] Error: ") + std::string( lua_tostring(L, -1) ) );
		};

	if( threadShouldExit() )
		{
		log( "Exiting thread early due to startup error." );
		return;
		}
	
	if( lua_gettop( L ) != 1 ) // There should only be the script
		{
		log( "Lua stack size was > 1 when starting a new thread, stack size was " + String( lua_gettop( L ) ) + "." );
		return;
		}

	log( "[PROCESSING START]" );

	// ...Make the call
	if( lua_pcall( L, 0, LUA_MULTRET, 0 ) )
		{
		//We can't do this stuff if the failure is due to the user exiting the thread early
		if( ! threadShouldExit() ) 
			{
			luaerr( L );
			log( "[PROCESSING FAILED] Time elapsed: " 
				+ Time( Time::getMillisecondCounter() - startTime ).formatted( "%M:%S:" ) + 
				std::to_string( ( Time::getMillisecondCounter() - startTime ) % 1000 ) );
			const ScopedLock lock( mutex );
			MessageManagerLock mml;
			threadFinished = true;
			repaint();
			}
		return;
		}
	log( "[PROCESSING END] Time elapsed: "
		+ Time( Time::getMillisecondCounter() - startTime ).formatted( "%M:%S:" ) + 
		std::to_string( ( Time::getMillisecondCounter() - startTime ) % 1000 )
		+ std::string("\n\n\n") );

	const int numLuaOutputs = lua_gettop( L );

	// Pull all the audios returned from lua into a container
	AudioVec outputs;
	for( int i = 1; i <= numLuaOutputs; ++i )
		{
		if( luaF_isAudio( L, i ) )
			{
			outputs.push_back( luaF_checkAudio( L, i ) );
			}
		else if( luaF_isAudioVec( L, i ) )
			{
			auto audioVec = luaF_checkAudioVec( L, i );
			for( auto & audio : audioVec )
				outputs.push_back( audio );
			}
		}

	{ // Close it up, ship it out
	const ScopedLock lock( mutex );
	MessageManagerLock mml;
	callback( outputs );
	threadFinished = true;
	repaint();
	}
	}

// void AltarThread::setUpProcess( string & command, cdpInfo_t info )
// 	{
// 	processList.emplace_back( cdpDir + "/" + command, info );
// 	}

// vector<string> AltarThread::process()
// 	{
// 	struct Child 
// 		{
// 		ChildProcess process;
// 		bool startStatus;
// 		string output;
// 		vector<string> generatedFileNames;
// 		};
// 	{
// 	vector<Child> children( processList.size() );

// 	for( int i = 0; i < processList.size(); ++i )
// 		{
// 		//improved tokenreplace, generates file names for placeholder character ('$')
// 		size_t stringPos = 0;
// 		while( ( stringPos = processList[i].first.find( '$', stringPos ) ) != string::npos )
// 			{ //We have to add _altar to deal with wacko multi out procs
// 			size_t extensionSize = processList[i].first.find( ' ', stringPos ) - ( stringPos + 1 );
// 			string extension = processList[i].first.substr( stringPos + 1, extensionSize );
// 			string path = getFreeFilename( extension );
// 			children[i].generatedFileNames.emplace_back( path );
// 			processList[i].first.replace( stringPos, 1 + extensionSize, path );
// 			}
// 		}
	
// 	//Start running each process
// 	for( int i = 0; i < processList.size(); ++i )
// 		{
// 		children[i].startStatus = children[i].process.start( processList[i].first );
// 		}
// 	//Wait for everything to finish and escape if needed
// 	for( int i = 0; i < processList.size(); ++i )
// 		{
// 		while( children[i].process.isRunning() )
// 			{
// 			//We can't simply block because the gui thread might ask this thread to exit
// 			Thread::wait( 30 );
// 			if( threadShouldExit() ) 
// 				{
// 				for( auto &i : children )
// 					i.process.kill();
// 				goto killSwitch; //Using goto to force ChildProcess off the stack, avoids memory leak
// 				}
// 			}
// 		}
// 	vector<string> output;
// 	for( int i = 0; i < children.size(); ++i ) 
// 		{
// 		children[i].output = children[i].process.readAllProcessOutput().toStdstd::string();
// 		if( ! children[i].startStatus || children[i].process.getExitCode() != 0 ) //If either startup or processing went wrong, error out
// 			{
// 			log( "[CDP] Error running command " + get<0>(processList[i]) + "\n[CDP] " + children[i].output );
// 			}
// 		else
// 			{
// 			//This is handled outside the output lookup because we need access to the output string
// 			if( get<1>( processList[i] ).first == cmdInfo ) 
// 				log( "[CDP] " + get<0>( processList[i] ) + " gave info: " + children[i].output );
// 			//Do lookup for outfile names. Most cdp processes output a single given file but some
// 			//	output multiple with related names. We use this to handle those cases.	
// 			else outFileTypeLookup( output, children[i].generatedFileNames, processList[i].first, processList[i].second );
// 			}
// 		}
// 	return output;
// 	}
// 	killSwitch: luaL_error( L, "Escaping processes for thread close" );
// 	}

