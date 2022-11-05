cmake_minimum_required( VERSION 3.15 )

project( Falter VERSION 0.1 LANGUAGES CXX )

set( CMAKE_EXPORT_COMPILE_COMMANDS TRUE )

add_subdirectory( JUCE )                

juce_add_gui_app( Falter
    # VERSION ...                       # Set this if the app version is different to the project version
    # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
    # ICON_SMALL ...
    # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
    COMPANY_NAME "Logan McBroom"        # Specify the name of the app's author
    PRODUCT_NAME "Falter")              # The name of the final executable, which can differ from the target name

juce_generate_juce_header( Falter )

target_sources( Falter
    PRIVATE
        src/Main.cpp
        src/MainComponent.cpp
        src/FalterLookAndFeel.cpp
        src/FalterButton.cpp
        src/FalterFileBrowser.cpp
        src/FalterLogger.cpp
        src/FalterClip.cpp
        src/FalterThread.cpp
        src/FalterList.cpp
        src/FalterClipList.cpp
        src/AltarThreadList.cpp
        src/Types.h

        src/Lua/Utility.cpp
        src/Lua/Usertypes.h
        src/Lua/Usertypes.cpp
        src/Lua/LTMP.h
        src/Lua/LTMP.cpp
        src/Lua/Audio.cpp
        src/Lua/AudioMod.cpp
        src/Lua/PVOC.cpp
        src/Lua/Function.cpp
        )

target_compile_features( Falter PRIVATE cxx_std_20 )
set_target_properties( Falter PROPERTIES CXX_EXTENSIONS OFF )
target_include_directories( Falter PRIVATE src )

target_compile_definitions( Falter
    PRIVATE
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
        JUCE_MODAL_LOOPS_PERMITTED=1 #Allows FileChooser to function
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Falter,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Falter,JUCE_VERSION>"
    )

target_link_libraries( Falter
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" )

# Flan
find_package( Flan REQUIRED ) # Flan::Flan
target_link_libraries( Falter PRIVATE Flan::Flan )

# LuaJit
find_package( LuaJit REQUIRED ) # LuaJit::LuaJit
target_link_libraries( Falter PRIVATE LuaJit::LuaJit )
