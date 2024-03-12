#pragma once

#include <JuceHeader.h>

class Settings
{
public:
	Settings();
	~Settings();

	static File getAudioLoadDir();
	static void setAudioLoadDir( const File & );

	static File getAudioSaveDir();
	static void setAudioSaveDir( const File & );

	static File getScriptFile();
	static void setScriptFile( const File & );

private:
	void saveToSettingsFile();

	File audioLoadDir;
	File audioSaveDir;
	File scriptFile;

	static const String settingsFileName;
	static Settings * instance;
};