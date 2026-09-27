#pragma once

#include <JuceHeader.h>

class FalterFileBrowser : 
    public FileBrowserComponent
    //public MouseListener,
    //public DragAndDropTarget
{
public:
    FalterFileBrowser();
    
private:
    void paint( Graphics & g ) override;

    // DragAndDropTarget interface
    // bool isInterestedInDragSource( const SourceDetails & dragSourceDetails) override;
    // void itemDropped( const SourceDetails & dragSourceDetails ) override;
    //void mouseDown( const MouseEvent & event ) override;

    WildcardFileFilter filter;
};