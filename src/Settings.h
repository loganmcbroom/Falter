#pragma once

#include <JuceHeader.h>

class Settings
{
public:
	Settings();
	~Settings();

	static File getFileLoadDir();
	static void setFileLoadDir( const File & );
	static File defaultFileLoadDir();

	static File getScriptFile();
	static void setScriptFile( const File & );
	static File defaultScriptFile();

private:
	void saveToSettingsFile();

	File fileLoadDir;
	File scriptFile;

	static const String settingsFileName;
	static Settings * instance;
};