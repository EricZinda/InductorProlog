
Getting Started
===============
Read readme.md for background on the engine and how to build it. This document describes how to use it if you think it is right for you. Also note that a simple overview of Prolog and how Prolog SLD Resolution works is available on the [Exospecies Blog](https://www.exospecies.com/blog/prolog).

## Performance
Note that the performance of this project is *HUGELY* dependent on whether you have built for retail or debug.  Debug builds have error checking which does *major* damage to performance.  Make sure you run in retail if you are evaluating the performance!

## Interactive Mode
Even though IndProlog is not designed to run interactively, it will do it, and this can allow you to play around and see what features it supports.

NOTE: Because it is designed as a production engine, not an interactive mode compiler, it does not try to be friendly.  In fact, if it hits a state that it considers to be a "programmer error", it will abort the process and exit. It was designed like this to make it easier to find bugs in production (since you'll get a stack trace and error report), but it does make the interactive usage less "friendly". It will give you a source file and line number when it fails. There will be a comment there in the source that tells you what went wrong.

To run it interactively, follow the instructions in readme.md to build the project, and run the `indprolog` program that gets generated.  `indprolog` requires you to pass it a Prolog program on the command line since the only thing that can be asserted are facts, so there's not much you can do to change the program interactively.

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
Finally: Write your own Prolog program and run it! Remember to look at the "Unsupported Prolog" section of readme.md and avoid constructs that aren't supported.

## Using the engine in an application
### Compiling Prolog Programs
If you will primarily be using the engine to load Prolog programs and queries that are written down using the Prolog Grammar, look at the code in `main.cpp` for the simplest usage case:

~~~
// IndProlog uses a factory model for creating terms so it
// can "intern" them to save memory.  You must never
// mix terms from different HtnTermFactorys
shared_ptr<HtnTermFactory> factory = 
	shared_ptr<HtnTermFactory>(new HtnTermFactory());

// HtnRuleSet is where the facts and rules are stored 
// for the program.  It is the database.
shared_ptr<HtnRuleSet> state = 
	shared_ptr<HtnRuleSet>(new HtnRuleSet());

// The PrologStandardCompiler will use the normal Prolog 
// parsing rules where capitalized things are variables
// An alternative used by the HTN engine (see blog mentioned
// in readme.md for more details) is called PrologCompiler
PrologStandardCompiler compiler(factory.get(), state.get());

// There are variants of CompileDocument() that take paths,
// streams or simply strings like this
if(compiler.CompileDocument("is(joe, happy)."))
{
	// The PrologStandardQueryCompiler will compile prolog
	// queries using the normal Prolog parsing rules
	PrologStandardQueryCompiler queryCompiler(factory.get());

	// HtnGoalResolver is the Prolog "engine" that resolves
	// queries
	HtnGoalResolver resolver;

	if(queryCompiler.Compile("is(joe, X)."))
	{
		// The resolver can give one answer at a time using 
		// ResolveNext(), or just get them all using 
		// ResolveAll()
		shared_ptr<vector<UnifierType>> queryResult = 
			resolver.ResolveAll(factory.get(), state.get(), queryCompiler.result());
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
	fprintf(stdout, "Error compiling %s, %s\r\n", 
		targetFileAndPath.c_str(), compiler.GetErrorString().c_str());
}
~~~

That example shows all the main parts of the Prolog compiler and how to use them if you are compiling Prolog Grammar as the way you get rules and queries in the engine.  If you want to do things at a lower level, read the next section.

### Using the Engine Without Compiling
You may have cases where you don't want to compile a document, but want to directly modify terms, rules, etc.  There are *lots* of examples of this in the tests included with the project, but here's a quick example that shows how it is done:

~~~
// IndProlog uses a factory model for creating terms so it can 
// "intern" them to save memory.  You must never mix terms from 
// different HtnTermFactorys
shared_ptr<HtnTermFactory> factory = shared_ptr<HtnTermFactory>(new HtnTermFactory());

// HtnRuleSet is where the facts and rules are stored 
// for the program.  It is the database.
shared_ptr<HtnRuleSet> state = shared_ptr<HtnRuleSet>(new HtnRuleSet());

// Add rule "head1(X) :- tail1(X)."
shared_ptr<HtnTerm> ruleHead = 
	factory->CreateFunctor("head1", { factory->CreateVariable("X") });
shared_ptr<HtnTerm> ruleTail = 
	factory->CreateFunctor("tail1", { factory->CreateVariable("X") });
ruleSet->AddRule(ruleHead, { ruleTail });

// Add fact "fact1(value)"
ruleHead = 
	factory->CreateFunctor("fact1", { factory->CreateConstant("value") });
ruleSet->AddRule(ruleHead, { });

// Create a query "fact1(X)"
shared_ptr<HtnTerm> query = 
	factory->CreateFunctor("fact1", { factory->CreateVariable("X") });

// HtnGoalResolver is the Prolog "engine" that resolves queries
HtnGoalResolver resolver;

// Now run the query, queryResult the solutions
shared_ptr<vector<UnifierType>> queryResult = 
	resolver.ResolveAll(factory.get(), state.get(), { query });
~~~

### HtnTermFactory
This is the class that creates Prolog terms (`HtnTerm`).  

Since it is always passed around to just about everything, it is also where a few "global" functions exist that many parts of the engine use:
- A flag that tracks if the engine runs out of memory: `HtnTermFactory::outOfMemory()`.  
- A way to set globals used by custom rules you write in C++ for the HtnGoalResolver: `HtnTermFactory::customData()`

Like most classes in the engine, you can ask it how much memory terms are are consuming  by calling the `HtnTermFactory::dynamicSize()` method.

In order to be efficient along several axes, all of the Prolog terms are "interned" (i.e. if you create the same term twice, the engine will map it to the same exact object in memory).  This does mean you need to always use the *same* `HtnTermFactory` or you will get very odd bugs.  There are FailFastAsserts in the code that will crash the process if you mix `HtnTermFactorys` to help catch the common cases.

### HtnTerm
This class implements the base construct in the Prolog language: the term.

It has a `HtnTerm::name()` and `HtnTerm::arguments()` which are the main ways you interact with it.

There are also a *ton* of helper methods for doing common things, look at the code for examples.
 
Importantly, there is no public constructor.  If you want an `HtnTerm`, you need to create one using an `HtnTermFactory`.

### HtnRuleSet
This class stores the rules of your Prolog program.  

The compiler lets you fill the database with rules by writing standard Prolog grammar, but you can just jam the rules in there directly too using `HtnRuleSet::AddRule()` as shown in the example above. Note, however, that this method can only be used before the HtnRuleSet is locked.  See next comment...

The Prolog language allows you to `assert()` and `retract()` rules to change the program dynamically which permanently changes the database in memory (Inductor Prolog only allows *facts* to be changed via those methods). The [Inductor HTN Engine](https://github.com/EricZinda/InductorHtn) uses Inductor Prolog and also expects to be able to change facts. However, it requires the ability to roll them back for the purposes of backtracking. So, there a few methods that support these different policies:

- `HtnRuleSet::CreateCopy()` creates a new `HtnRuleSet` that shares the same rules (initially) with the original. However, you can use the `HtnRuleSet::Update()` method to add or remove facts from the copy without modifying the original (it uses diff'ing for efficiency).  When called, it locks the original and the copy so that only *facts* (not rules) can be modified. This is how backtracking is implemented in the [Inductor HTN Engine](https://github.com/EricZinda/InductorHtn) . 
- `HtnRuleSet::CreateSharedRulesCopy()` creates a new `HtnRuleSet` that has a copy of all the rules and is editable.
- `HtnRuleSet::AddRule()` allows you to add rules to the database, but only works if the database hasn't been locked by a call to `HtnRuleSet::CreateCopy()` to ensure consistency.
- `HtnRuleSet::Update()` allows you to add and remove *facts* from the database.  They will only be reflected in this instance, not instances that were copied from. Used by the [Inductor HTN Engine](https://github.com/EricZinda/InductorHtn).

Just like HtnTermFactory, call `HtnRuleSet::dynamicSize()` to get the unique memory usage used by the rules (but not the terms they contain, that is done by calling HtnTermFactory).

### HtnGoalResolver
This is the Prolog Engine that does the bulk of the work. The main functions you will use are `HtnGoalResolver::ResolveNext()` and `HtnGoalResolver::ResolveAll()` which solve Prolog queries. A simple overview of Prolog and how Prolog SLD Resolution works is available on the [Exospecies Blog](https://www.exospecies.com/blog/prolog).

There are a bunch of static helper functions that can come in handy on this class as well. Most useful is the `HtnGoalResolver::Unify()` function that will do unification between two terms.

This class is where you can extend the Prolog engine to support custom rules that are written in C++ for cases where you want to integrate it with you program or have a rule that is particularly hard to write in Prolog.  You do this by calling `HtnGoalResolver::AddCustomRule()`. There are many examples of this in `HtnGoalResolver` that you can use as examples, since that is how things like `assert/1` and `retract/1` are built.  

#### Stackless Execution
Goal resolution is done in `HtnGoalResolver::ResolveNext()` and this function is written to be "stackless" (i.e. avoid recursion by implementing a custom stack) since the stack size on devices like the iPhone is small, and the natural way of writing this function uses a lot of stack space.  It was also required to allow the caller to get each solution to a query iteratively, over time. That makes it somewhat difficult to follow and makes writing custom rules more challenging.  Unfortunately I couldn't find a way around this.  Here's a brief tour of how it works:

A single instance of `ResolveState` stores all the state for the resolver. This is how you can get the first solution for a query and then later get the next one.  The entire state is in that class.  Most importantly is the `ResolveState::resolveStack` member.  This is the "simulated stack". 

Each item in `ResolveState::resolveStack` is type `ResolveNode`.  A `ResolveNode` respresents both a classic "stack frame" but also, due to the way the algorithm works, a node in the SLD resolution tree (thus the name). A key member on this class is `ResolveNode::continuePoint`.  This is how classic "call and return" stack behavior is implemented.  When the code pushes a new `ResolveNode` on `ResolveState::resolveStack`, it sets its own `ResolveNode::continuePoint` to where execution should continue when the stack is popped back to the node.  

Here's the basic structure of how the stack works in the code:
~~~
while(resolveStack->size() > 0)
{
	shared_ptr<ResolveNode> currentNode = resolveStack->back();
	switch(currentNode->continuePoint)
	{
		...
	}
}
~~~
Understanding the stackless execution will make understanding the code *much* easier.

Just like the rest, call `ResolveState::dynamicSize()` to get the unique memory usage used by the Resolver.

#### "Standalone Resolve" in a Custom Rule
Another key concept in the resolver used by some custom rules is the "Standalone Resolve". The basic idea is that some of the custom rules like `sortBy` need to treat their arguments as a standalone query which gets executed "standalone", finishes, returns control to the rule, and then the rule does something with it.

For example, `sortBy` takes the solutions that come from doing a Standalone Resolve, sorts them, and then allows the engine to continue executing normally.

There are prodigious comments in the code that will hopefully make it clear how this all works.

## Details about the Compiler and Parser
A detailed description of the Compiler and Parser part of the Prolog Engine is available [here](https://github.com/EricZinda/InductorParser).
