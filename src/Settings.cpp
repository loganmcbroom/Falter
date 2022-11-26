#include "Settings.h"

#include <fstream>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

const String Settings::settingsFileName = "Settings.txt";
Settings * Settings::instance = nullptr;

Settings::Settings()
	: fileLoadDir()
	{
	jassert( instance == nullptr );
	instance = this;
	
	std::ifstream file( settingsFileName.toStdString() );
	if( file )
		{
		std::string fileLoadDirS;
		std::string scriptFileS;
		std::getline( file, fileLoadDirS );
		std::getline( file, scriptFileS   );
		fileLoadDir = File( fileLoadDirS );
		scriptFile  = File( scriptFileS );
		if( ! fileLoadDir.exists() ) fileLoadDir = defaultFileLoadDir();
		if( ! scriptFile .exists() ) scriptFile  = defaultScriptFile();
		}
	else
		{
		fileLoadDir = defaultFileLoadDir();
		scriptFile  = defaultScriptFile();
		}

	saveToSettingsFile();
	}

Settings::~Settings()
	{
	instance = nullptr;
	}

File Settings::getFileLoadDir()
	{
	jassert( instance != nullptr );
	if( instance )
		return instance->fileLoadDir;
	else 
		return defaultFileLoadDir();
	}

void Settings::setFileLoadDir( const File & newDir )
	{
	jassert( instance != nullptr );
	if( instance )
		{
		instance->fileLoadDir = newDir;
		instance->saveToSettingsFile();
		}
	}

File Settings::defaultFileLoadDir()
	{
	return File::getCurrentWorkingDirectory();
	}

File Settings::getScriptFile()
	{
	jassert( instance != nullptr );
	if( instance )
		return instance->scriptFile;
	else 
		return defaultScriptFile();
	}

void Settings::setScriptFile( const File & newFile )
	{
	jassert( instance != nullptr );
	if( instance )
		{
		instance->scriptFile = newFile;
		instance->saveToSettingsFile();
		}
	}

File Settings::defaultScriptFile()
	{
	return File::getCurrentWorkingDirectory().getChildFile( "Select Script File" );
	}

void Settings::saveToSettingsFile()
	{
	File settingsAbs = File::getCurrentWorkingDirectory().getChildFile( settingsFileName );

	// Make sure Settings file exists
	if( ! settingsAbs.exists() )
		{
		Logger::writeToLog( settingsFileName + " didn't exist, creating one." );
		if( ! settingsAbs.create() )
			{
			Logger::writeToLog( settingsFileName + " couldn't be created, so settings couldn't be saved." );
			return;
			}
		}

	// Open file
	std::ofstream file( settingsFileName.toStdString(), std::ios_base::binary | std::ios_base::trunc );
	if( ! file )
		{
		Logger::writeToLog( settingsFileName + " exists, but couldn't be opened for saving." );
		return;
		}

	// Write to file
	file << fileLoadDir	.getFullPathName() << std::endl;
	file << scriptFile	.getFullPathName() << std::endl;
	}
