#pragma once

#include <JuceHeader.h>

class FalterFileBrowser : public FileBrowserComponent
{
public:
    FalterFileBrowser();
private:

    void paint( Graphics & g ) override;

    WildcardFileFilter filter;
};