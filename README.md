Notes about systems:
    For each flan data type T, there are two Lua types:
        userdata containing a shared_ptr<T>
        Lua array of the first type
    Methods return the type they were passed
    Constructing the array type and calling a method can be done manually using TVec{ x, y }:method()

    Function types are userdata wrapping shared ptrs to flan::Func types
    Passing a lua function will create a flan::Func closure containing a lua reference to the lua function which is then wrapped as usual

todo:
    Setting for sample folder
    Save script to settings when selected
    a way to record in new audio
    load audio async for more responsive clip import?
    file browser drag and drop
        Allow dragging from clips to browser
        Allow dragging from browser to clips
        Allow dragging from clips to other clips
   
    hardcoded path File("C:/Users/logan/Documents/Samples") in two places
    use namespace Falter instead of FalterThing
    use juce without including in build tree
    make sure audio algos are moving the buffer not copying


Visual:
    Command background is wrong color
    Thread background is wrong color
    Audio thumbnail stopped loading
    Fix sample button icon
    Successful process has 2 newline in logger, failed has 0, switch both to 1
    color thread red or green based on thread succeeding
    AltarClip playhead line starts at wrong position

task:
    Func2x1
    Func2x2
    PVOC
    Graph?
    remove extra buttons, package flan dox with program
        
    