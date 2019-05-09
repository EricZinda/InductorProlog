Getting Started
===============
Read readme.md for background on the engine and how to build, this document describes how to use it if you think it is right for you.

## Interactive Mode
Even though IndProlog is not designed to run interactively, it will do it, and this can allow you to play around to see what features it supports.

NOTE: Because it is designed as a production engine, not an interactive mode compiler, it does not try to be friendly.  In fact, if it hits a state that it considers to be a "programmer error", it will abort the process and exit. It was designed like this to make it easier to find bugs in production (since you'll get a stack trace and error report), but it does make the interactive usage less "friendly". It will give you a source file and line number when it fails. There will be a comment there that tells you what went wrong.

To run it interactively, follow the instructions in readme.md to build the project, and run the `indprolog` program that gets generated.  `indprolog` requires you to pass it a prolog program on the command line since the only thing that can be asserted are facts, so there's not much you can do to change the program interactively.

Here are a couple of things to try:

First: Load HelloWorld.pl to try some "classic" prolog programs:

~~~
1. Run indprolog <path to Examples directory>HelloWorld.pl
2. Look at the comments in the HelloWorld.pl file for examples to try.
~~~

Next: Try the interactive text based adventure game contributed by Jordan Isaacs:
~~~
1. Run indprolog <path to Examples directory>JordanAdventure.pl
2. type "start." and follow the instructions from there
~~~

## Using the engine
Look at the code in `main.cpp` for the simplest usage case:

~~~
// IndProlog uses a factory model for creating terms so it can "intern" them to save memory.  You must never
// mix terms from different HtnTermFactorys
shared_ptr<HtnTermFactory> factory = shared_ptr<HtnTermFactory>(new HtnTermFactory());

// HtnRuleSet is where the facts and rules are stored for the program.  It is the database.
shared_ptr<HtnRuleSet> state = shared_ptr<HtnRuleSet>(new HtnRuleSet());

// The PrologStandardCompiler will use the normal Prolog parsing rules where capitalized things are variables
// An alternative used by the HTN engine (see blog mentioned in readme.md for more details) is called PrologCompiler
PrologStandardCompiler compiler(factory.get(), state.get());

// There are variants of CompileDocument() that take paths, streams or simply strings like this
if(compiler.CompileDocument("is(joe, happy)."))
{
	// The PrologStandardQueryCompiler will compile prolog queries using the normal Prolog parsing rules
	PrologStandardQueryCompiler queryCompiler(factory.get());

	// HtnGoalResolver is the Prolog "engine" that resolves queries
    HtnGoalResolver resolver;

	if(queryCompiler.Compile("is(joe, X)."))
	{
		// The resolver can give one answer at a time using ResolveNext(), or just get them all using ResolveAll()
		shared_ptr<vector<UnifierType>> queryResult = resolver.ResolveAll(factory.get(), state.get(), queryCompiler.result());
	}
	else
	{
		// Query compile failure
		fprintf(stdout, "Error: %s\r\n\r\n", queryCompiler.GetErrorString().c_str());
	}
}
else
{
	// Program compile failure
	fprintf(stdout, "Error compiling %s, %s\r\n", targetFileAndPath.c_str(), compiler.GetErrorString().c_str());
}
~~~
