#include "Settings.h"

#include <fstream>

const String Settings::settingsFileName = "Falter_Settings.txt";
Settings * Settings::instance = nullptr;

Settings::Settings()
	: audioLoadDir()
	{
	jassert( instance == nullptr );
	instance = this;
	
	std::ifstream file( settingsFileName.toStdString() );
	if( file )
		{
		std::string audioLoadDirS;
		std::string audioSaveDirS;
		std::string scriptFileS;
		std::getline( file, audioLoadDirS );
		std::getline( file, audioSaveDirS );
		std::getline( file, scriptFileS   );
		audioLoadDir = File( audioLoadDirS );
		audioSaveDir = File( audioSaveDirS );
		scriptFile  = File( scriptFileS );
		if( ! audioLoadDir.exists() ) audioLoadDir = File::getCurrentWorkingDirectory();
		if( ! audioSaveDir.exists() ) audioSaveDir = File::getCurrentWorkingDirectory();
		if( ! scriptFile .exists() ) scriptFile  = File::getCurrentWorkingDirectory().getChildFile( "Select Script File" );
		}
	else
		{
		audioLoadDir = File::getCurrentWorkingDirectory();
		audioSaveDir = File::getCurrentWorkingDirectory();
		scriptFile  = File::getCurrentWorkingDirectory().getChildFile( "Select Script File" );
		}

	saveToSettingsFile();
	}

Settings::~Settings()
	{
	instance = nullptr;
	}

File Settings::getAudioLoadDir()
	{
	jassert( instance != nullptr );
	return instance->audioLoadDir;
	}

void Settings::setAudioLoadDir( const File & newDir )
	{
	jassert( instance != nullptr );
	instance->audioLoadDir = newDir;
	instance->saveToSettingsFile();
	}

File Settings::getAudioSaveDir()
	{
	jassert( instance != nullptr );
	return instance->audioSaveDir;
	}

void Settings::setAudioSaveDir( const File & newDir )
	{
	jassert( instance != nullptr );
	instance->audioSaveDir = newDir;
	instance->saveToSettingsFile();
	}

File Settings::getScriptFile()
	{
	jassert( instance != nullptr );
	return instance->scriptFile;
	}

void Settings::setScriptFile( const File & newFile )
	{
	jassert( instance != nullptr );
	instance->scriptFile = newFile;
	instance->saveToSettingsFile();
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
	file << audioLoadDir.getFullPathName() << std::endl;
	file << audioSaveDir.getFullPathName() << std::endl;
	file << scriptFile.getFullPathName() << std::endl;
	}
