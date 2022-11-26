#include "FalterLookandFeel.h"

FalterLookAndFeel& FalterLookAndFeel::getLNF()
    {
    return *FalterLookAndFeel::instance;
    }

FalterLookAndFeel* FalterLookAndFeel::instance = nullptr;

FalterLookAndFeel::FalterLookAndFeel()
    : margin        ( 6 )
    , unit          ( 40 )
    , fontSize      ( unit * ( 26.0f / 40.0f ) )
    , shadow        ( Colour::fromHSV( .58f, .08f, .09f, 1 ) )
    , dark          ( Colour::fromHSV( .58f, .08f, .14f, 1 ) )
    , mid           ( Colour::fromHSV( .58f, .09f, .25f, 1 ) )
    , light         ( Colour::fromHSV( 0, 0, 1, 1 ) )
    , accent1       ( Colour::fromHSL( .003f, .647f, .549f, 1 ) )
    , accent2       ( Colour::fromHSL( .325f, .500f, .500f, 1 ) )
    , fontDefault   ( Font::getDefaultSansSerifFontName(), fontSize, Font::plain )
    , fontWingdings ( "Wingdings", fontSize, Font::bold )
	, fontWebdings  ( "Webdings" , fontSize, Font::bold )
	, fontSymbol    ( "Symbol"	 , fontSize, Font::bold )
	, fontMonospace ( Font::getDefaultMonospacedFontName() , 12, Font::plain )
    {
    setColour( TextEditor::textColourId, light );
    setColour( TextEditor::outlineColourId, dark );
    
    setColour( TreeView::backgroundColourId, dark );

    setColour( FileBrowserComponent::currentPathBoxBackgroundColourId, dark );
    setColour( FileBrowserComponent::currentPathBoxTextColourId, light );
    setColour( FileBrowserComponent::currentPathBoxArrowColourId, light );
    setColour( FileBrowserComponent::filenameBoxBackgroundColourId, dark );
    setColour( FileBrowserComponent::filenameBoxTextColourId, light );

    }

void FalterLookAndFeel::drawScrollbar( Graphics &g, ScrollBar &, 
                    int x, int, int width, int, 
                    bool, int thumbStartPosition, 
                    int thumbSize, bool, bool )
    {
    g.setColour( mid );
    g.fillRoundedRectangle( x + 2.0f, float(thumbStartPosition), width - 4.0f, float(thumbSize), 3.0f );
    }

void FalterLookAndFeel::drawFileBrowserRow( Graphics & g,
    int width,
    int height,
    const File & file,
    const String & filename,
    Image * optionalIcon,
    const String & fileSizeDescription,
    const String & fileTimeDescription,
    bool isDirectory,
    bool isItemSelected,
    int itemIndex,
    DirectoryContentsDisplayComponent & )
    {
    const Rectangle<float> area( width, height );

    if( isItemSelected )
        {
        g.setColour( accent2 );
        g.fillRect( area );
        }
    
    g.setFont( fontMonospace );
    g.setColour( light );
    g.drawText( filename, area, Justification::left );
    }

void FalterLookAndFeel::drawPopupMenuItem( 
    Graphics & g, const Rectangle<int> & area, 
    bool, bool, bool, bool, bool, 
    const String & text, const String &, 
    const Drawable *, const Colour * ) 
    {
    const Rectangle<float> adjustedArea( 5, 0, area.getWidth(), area.getHeight() ); // Text needs to be scooted over a few pixels
    g.setFont( fontMonospace );
    g.setColour( light );
    g.drawText( text, adjustedArea, Justification::left );
    }

//  void FalterLookAndFeel::drawComboBox( Graphics & g, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox & )
//      {
//      g.setColour( light );
//      g.drawRoundedRectangle( 0, 0, width, height, 5, 1 );
//      }

Font FalterLookAndFeel::getComboBoxFont( ComboBox & ) 
    {
    return fontMonospace;
    }

void FalterLookAndFeel::drawLabel( Graphics & g, Label & l )
    {
    g.setFont( fontMonospace );

    if( File::isAbsolutePath( l.getText() ) )
        {
        g.setColour( light );
        const File path = l.getText();
        l.hideEditor( true );
        const String shortPath = path.getRelativePathFrom( path.getParentDirectory().getParentDirectory() );
        g.drawText( shortPath, Rectangle<float>( 3, 0, l.getWidth(), l.getHeight() ), Justification::left );
        return;
        }
    else
        {
        g.fillAll( dark );
        g.setColour( light );
        g.drawText( l.getText(), Rectangle<float>( margin, 2, l.getWidth(), l.getHeight() ), Justification::left );
        }
    }
