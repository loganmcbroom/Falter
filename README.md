# Falter
Lua-based almost-livecoding frontend for the flan audio processing library.

## Building From Source
The provided cmake script has been tested on both Windows and Mac systems, so it's pretty likely to just work. Falter relies on
flan and LuaJIT. It also relies on JUCE, but all the needed JUCE files are included. I haven't tested on Linux, but if that's 
you, ya'know, you kinda signed up for figuring out how to compile stuff right? Okay, okay, if it doesn't work email me and we
can get it working there.


## User Guide

Hello, here is my little user guide for making sounds in Falter. Falter is a gui program that handles files, threads, computation logging,
recording, and most importantly, watching a Lua script. When processing is triggered, the Lua script being watched is run over a Lua context
containing nearly all flan functions. Any audios returned from the script are collected to be saved, or processed further. Unlike some other
audio processing softwares, Falter has no qualms about throwing away data generated. You should expect to end up with a lot of outputs, and
if you want anything to persist after closing the program, make sure to save it! We'll start with an overview of the gui itself, and then 
talk about what can be accomplished on the script side.

### GUI

#### Buttons
In the top left of the gui there are five buttons.
* The skull and crossbones button triggers processing, creating a new thread, copying the input audios into it, and running the script on them.
* The circle button starts recording. Click it again to stop recording, and whatever sounds you made will be placed in the input clip list.
* The bomb/glasses button toggles a menu with some basic I/O settings.
* The temperature button has two modes: hot and cold. Cold mode does nothing, so feel free to think about it as nothing mode. In hot mode,
    the currently active script is watched for changes. When you hit ctrl-s and some changes are saved to disk, it detects this, and automatically
    retriggers processing as though you had clicked the skull.
* The folder button selects the active script. You will probably not want to just throw out your scripts every time, right? Well, just create a 
    new one and use this button to select it. The currently active script will have its name displayed next to the script select button

#### File Browser
I don't think a really have much to say about this. It's a file browser. Double clicking a folder will set it as the browser root, and this is
saved in a settings file. The up arrow goes up one folder level. You can double-click audio files to load them into inputs, or you can drag and drop
them into a clip list.

#### Clip Lists
There are two clip lists, labelled "Inputs" and "Outputs". Audio clips within these lists can be dragged and dropped to reorder them, as well as being
dragged and dropped from one to the other. This is very useful for reprocessing a previous output without having to save it to disk. The buttons on each
clip should be self-explanatory. The X at the top of each clip list clears the entire list.

#### Thread Lists
Between the clip lists is the thread list. Any time processing is triggered, a new thread is spawned to manage that processing. Information about each
processing thread is given here, such as the success/failure of the script, the time at which it was run, and the amount of time it took to run. Each
thread also has an ID that can identify its outputs in the logger. This list could probably be removed, but I think it's kind of cool.

#### Logger
At the bottom of the gui is the logger. The logger spits out data about which flan processes are being run, what thread is running them, along with
any errors that might have caused a script to fail. Should I make it a little taller? Call now to vote.

- - - -

Woah that's everything! It feels like something is missing though... the text editor! Well, I don't think I can make a better text editor that
the great ones that are aleady out there, so you'll need to have an editor open along with Falter. Falter has a Lua Language Server addon that
I would not bother using it without, so whatever editor you choose, make sure it can use LLS. If you don't have a favorite picked out already,
vscode is a nice free editor with LLS support. The LLS addon isn't currently in the main addon repository, although this may change in the
future. For now, it will need to be manually installed. 

To add the addon to vscode, first go to the extensions marketplace and install the Lua Language Server extention. Hit "ctrl+," to open user 
settings. Click the "Workspace" tab and navigate to "lua.workspace.library" (You can type this in the search bar at the top to jump directly to it). 
Click "Add Item" and add the absolute path to the addon library folder incuded in the release, e.g., 
"C:/path/to/folder/Falter_Release_0.1.0/Falter-LLS/library/". Opening an exaple script and hovering your mouse over variables should now give
a list of available methods, and hovering methods should give a description of what they do and what arguments they take. Not all methods
and arguments are given descriptions, as many are self-explanitory. If you encounter a missing description that you think is needed, feel
free to reach out to me so I can update the addon.



### Scripting

Writing about how to click a gui is easy, telling someone how to program not so much. Maybe I should first tell you, I'm not going to teach you
how to program in Lua here. It's expected that you either know a bit about coding, or are willing to google stuff while you work. The sky
is the limit on what you can do, but here are the basic things you'll need to know. The first is that whatever inputs you passed into the script 
are stored in an array called "inputs". The second is that Lua is one-indexed, so don't go trying to access inputs[0]. The inputs object is not
just a plain array of Audios by the way, it is an "AudioVec" type object, which I will explain in a minute. You can apply algorithms to the inputs
however you see fit, or you can pass in no inputs and generate your sounds entirely within the script using Audio.synthesize or PV.synthesize.
When you are done processing, return any number of Audio objects from the script, and they will appear in the output clip list. If anything goes
wrong in the script, an error will be dumped to the logger, and the thread will turn red.

#### Types
* Audio, it is audio. You can do all the normal stuff you'd expect with it, and that is that.

* PV is phase vocoder data. This is a time/frequency analysis type that allows manipulation of an audio spectrum over time. It is famous for
    being used in time and pitch stretching algorithms, but it is an incredibly powerful data format. You can get to PV data by calling
    "convert_to_PV" on an Audio, or even better, calling "convert_to_ms_PV" on a stereo Audio. The latter reduces the effect some of the "smearing"
    artifacts generated by phase vocoder algorithms have on stereo incoherence. Compare with regular PV conversion and processing to see
    what I mean. To get back, use "convert_to_Audio" or "convert_to_lr_Audio" respectively. It isn't the most user friendly, but to see
    a hint of the true power of PV data, try the "prism" function.

* Wavetable is a type for storing wavetable data and using it to synthesize Audio.
    I won't pretend I gave 100% on Wavetable support alright? This was more an expiriment for me, and it doesn't work in the way most wavetables do.
    That said, it can't be denied that flan Wavetables can produce some sounds you are unlikely to find anywhere else. You can get a wavetable
    using Wavetable.create_from_audio or Wavetable.create_from_function, and produce output using Wavetable:synthesize. It's recommended you call
    Wavetable:remove_jumps_in_place and Wavetable:add_fades_in_place before synthesizing outputs.

* Vec Types
    The are also _Vec types for Audio and PV, that is, AudioVec, and PVVec. These are simple userdata wrappers around an array of the type you 
    would expect. Having this type info allows the LTMP to work, which is explained below, along with helping LLS offer the correct autocomplete
    options. They also have some nice utility overloads. For example, given an AudioVec a, rather than calling Audio.mix( a ), we can call
    a:mix(). This is convenient when chaining processes, which is a lot of what you'll want to do in Falter.

#### LTMP
Falter would be cool without LTMP, but I take more pride in this feature than any other. I had to learn a bunch of c++ template metaprogramming
tricks to make this work so I hope you'll use it. The idea is that any time a single (non-array) parameter would be passed into a flan function,
you can instead pass an array of such arguments to invoke the LTMP. The process will then be executed for each argument in the array, and a _Vec
type will be returned instead of the normal single return. Because the object on which a method is invoked counts as an argument in lua, this _Vec
will then continue to trigger the LTMP down any further processing. You can combine the _Vec down the line using things like mix, join, and select,
or you can just let it return from the script and get all the outputs in the gui.

When more than one method argument is substituted with an array, the LTMP is "overloaded". There are two logical ways for this to be handled, as
far as I can tell. One is to run the given process for every combination of inputs in each substitution vector. The other is to process a number
of times equal to the largest size among the substitution vectors, and access the smaller vectors modulo their size. Falter uses the latter strategy,
as I think the former behaviour is easy enough to manually add as a Lua utility function.

One final issue with the LTMP is that it causes the LLS information to be imperfect. I am trying to get this sorted out, but there are some 
complex edge cases, so if you spot an incorrect type let me know.

#### Examples

It's probably best to get started by modifying some example scripts. Examples should already be included with any release package of Falter,
so find those, load them up, and dissect some sounds!



## Contributions

I would be psyched if you added to this thing! Just keep in mind that the algorithms themselves are coming from flan, so if it's a new
algorithm you want, add it there instead.



## Licence

GNU GENERAL PUBLIC LICENSE -- see the file LICENSE for details.
