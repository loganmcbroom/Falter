Off topic:
    Graph convertToPVOC
    filter package
    like one in twenty times converting to pvoc and back breaks
    https://dsp.stackexchange.com/questions/41696/calculating-the-inverse-filter-for-the-exponential-sine-sweep-method/41700#41700
    song idea, cut mid video sponsered ads out of youtube videos
    song idea, something making fun of "top 10 x june 2022" articles
    can anything else interesting be done with the wavetable class?
    wavetable cancel points

todo:
    Setting for sample folder
    Save script to settings when selected
    a way to record in new audio
    load audio async for more responsive clip import
    file browser drag and drop
        Allow dragging from clips to browser
        Allow dragging from browser to clips
        Allow dragging from clips to other clips
    package flan dox with program
    watch script to compute on save https://github.com/apetrone/simplefilewatcher
    script and sample folder select should start in a better folder
    Clip is being given title "Temp Name"
    stop button isn't changing back when audio finishes playing
    Put thread number on thread somewhere
    seems like canceller isn't working, or more that when running a long repitch the gui thread is frozen
    hardcoded path File("C:/Users/logan/Documents/Samples") in two places
    At some point I found a window that said there was a memory leak, idk where but look into that  
    audio synthesize is making a sine wave sound saw like, try in plain c++
    Lua:
        prism, requires a prismFunc wrapper
        we need a way to check that lua functions passed in have the required signiture
        adding waveforms for audio synthesize might speed it up a good bit
            anything to keep from calling lua functions

    from cppref, can be used instead of vector concept: 
        // C++20 deduced class type placeholder, class template arguments are deduced at the call site
        template<std::array arr>
        void f();
        f<std::array<double, 8>{}>();

task:
    we shoud have a vec type for any flan type with methods
    use ctor functors if needed for
        -wavetable
        pvoc
        audio
        functions