Off topic:
    song idea, cut mid video sponsered ads out of youtube videos
    song idea, something making fun of "top 10 x june 2022" articles
    juce browser reset bug: https://forum.juce.com/t/filebrowsercomponent-timercallback-resetting-browser/53902

Maybe todo:
    Audio:
        synth grains needs a grain source function
    Could keep seperate logs for each thread
    Interpolator system with enum doesn't work with interpolate_points and also sucks
        We would like to just take pFunc1x1 and call it a day
        can't because flan::Function can't be copied, so it can't be captured by std::function, which is needed for interpolate_points
        Function periodize suffers the same fate, can't capture the function passed in
    Add synthesize_grains
    how to handle ltmp on functions already outputting vectors?
 
Task:
    publish lls addon
        wait a bit for new framework developments

