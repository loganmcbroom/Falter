Notes about systems:
    For each flan data type T, there are two Lua types:
        userdata containing a shared_ptr<T>
        Lua array of the first type
    Methods return the type they were passed
    Constructing the array type and calling a method can be done manually using TVec{ x, y }:method()

    Function types are userdata wrapping shared ptrs to flan::Func types
    Passing a lua function will create a flan::Func closure containing a lua reference to the lua function which is then wrapped as usual

