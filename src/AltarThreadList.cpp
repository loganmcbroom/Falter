#include "AltarThreadList.h"

#include <string>

#include <flan/Audio.h>

#include "AltarLogger.h"

AltarThreadList::AltarThreadList()
	{
	}

AltarThreadList::~AltarThreadList()
	{
	}

void AltarThreadList::addThread( 
	const String & script, 
	std::function< void( std::vector<std::shared_ptr<flan::Audio>> & ) > & callback, 
	const std::vector<std::shared_ptr<flan::Audio>> & files )
	{
	addItem( new AltarThread( 
		File( File::getCurrentWorkingDirectory().getFullPathName() + "/" + script ).getFileNameWithoutExtension() + String( Time::currentTimeMillis() ), 
		script, 
		callback, 
		files ) );
	}

