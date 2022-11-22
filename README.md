New project, no readme.
This currently is just details I need to remember to explain in the readme
add code examples of how each feature can be used

LTMP

flan types have _____Vec types in lua to allow ltmp activating when the object is the LTMP source

static methods are set up as functions on the global ctor table, e.i. Audio.synthesize 
    Static methods taking vectors of the object type are also registered on the _____Vec types, i.e. AudioVec{ a, b }:join()


