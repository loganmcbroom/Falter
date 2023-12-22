Off topic:
    song idea, cut mid video sponsered ads out of youtube videos

Maybe todo:
    Could keep seperate logs for each thread
    Interpolator system with enum doesn't work with interpolate_points and also sucks
        We would like to just take pFunc1x1 and call it a day
        can't because flan::Function can't be copied, so it can't be captured by std::function, which is needed for interpolate_points
        Function periodize suffers the same fate, can't capture the function passed in
    save inputs and outputs when closing to reopen later
    clicking on clip sets transport to that point in the clip
 
Task:
    publish lls addon
        wait a bit for new framework developments
        
    We should have some type of process that takes in a bunch of inputs and texturizes as in cdp

    PV::freeze and other vector param methods can't ltmp, but why not?

    lls needs to know that [] on a vec type returns non vec of that type

    clicking save and cancelling currently makes the file name blank

    new idea for file saving
        clip save button currently unused
        red if unsaved
        green if saved
        click to save as current name
        clicking name allows rename
        dragging out of application auto saves

    There should be some kind of reasonably short limit on the size of an Audio, we can probably just throw if it tries to be too big

    qmk compile -kb sofle_choc -km loganmcbroom -e CONVERT_TO=elite_pi
