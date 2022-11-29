#pragma once

#include <JuceHeader.h>

class FalterFileBrowser : public FileBrowserComponent
                        , public DragAndDropTarget
{
public:
    FalterFileBrowser();
    
private:
    void paint( Graphics & g ) override;

    // DragAndDropTarget interface
    bool isInterestedInDragSource( const SourceDetails & dragSourceDetails) override;
    void itemDropped( const SourceDetails & dragSourceDetails ) override;

    WildcardFileFilter filter;
};