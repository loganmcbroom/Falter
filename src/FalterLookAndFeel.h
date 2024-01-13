#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

class FalterLookAndFeel : public LookAndFeel_V4
{
public:
	static FalterLookAndFeel& getLNF();

	FalterLookAndFeel();
 
	void drawScrollbar( Graphics &, ScrollBar &, int, int, int, int, bool, int, int, bool, bool ) override;

	void drawFileBrowserRow( Graphics &, int, int, const File &, const String &, Image *, 
		const String &, const String &, bool, bool, int, DirectoryContentsDisplayComponent & ) override;

	void drawPopupMenuItem( Graphics &, const juce::Rectangle<int> &, bool, bool, bool, bool, bool, 
		const String &, const String &, const Drawable *, const Colour * ) override;

	// void drawComboBox( Graphics &, int, int, bool, int, int, int, int, ComboBox & ) override;
	Font getComboBoxFont( ComboBox & ) override;
	void drawLabel( Graphics &, Label & ) override;

	// Settings
	const int margin;
    const int unit;
    const float fontSize;

    const juce::Colour dark;
    const juce::Colour shadow;
    const juce::Colour mid;
    const juce::Colour light;
    const juce::Colour red;
    const juce::Colour green;
    const juce::Colour yellow;

    const juce::Font fontDefault;
    const juce::Font fontWingdings;
	const juce::Font fontWebdings;
	const juce::Font fontSymbol;
	//const juce::Font fontRoboto;
	const juce::Font fontMonospace;

	static FalterLookAndFeel* instance;
};
