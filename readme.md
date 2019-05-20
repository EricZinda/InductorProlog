Inductor Prolog Compiler
========================
This lightweight Prolog compiler was first used in production in an iPhone strategy game called [Exospecies](https://www.exospecies.com). Visit the [Exospecies Blog](https://www.exospecies.com/blog) for more details.  It was designed for use with the [Hierarchical Task Network Engine](https://github.com/EricZinda/InductorHtn) but can also be used standalone as a simple embedded Prolog compiler.

It is designed to be small, memory constrained, and used as an implementation detail of an app... *not* as an interactive prolog compiler.  That means it has features like:

- Small and easy to debug and extend.  I wanted something I could understand.
- Constrained feature set (see below for a non-exhaustive list of features it *doesn't* support). I only implemented what was required to implement the Heirarchical Task Network AI algorithm (see blog above for more details)
- Ability to set the memory budget and have it fail gracefully if the budget is exceeded
- Avoidance of the stack whenever possible since the stack size on some devices like the iPhone is *very* constrained.  Unfortunately, this makes some of the code harder to understand but was necessary to use in production.
- It will abort the process when it encounters logic failures (like asserting a fact twice) instead of returning a nice error message.  It was designed like this to make it easier to find bugs in production (since you'll get a stack trace and error report), but it does make the interactive usage less "friendly".

Regardless, when you build the project you will end up with an app that you can use interactively just to understand and demonstrate the features.  See the gettingstarted.md doc for more information.

Use and enjoy!

## Unsupported Prolog
The Inductor Prolog engine supports only what was required to ship the [Exospecies](www.exospecies.com) game and AI so it is not exhaustive or standards conforming. However, it does have the minimum set of features to write a very robust game AI, so it has most of the base features you'd expect from a Prolog compiler.

The following features are for sure *not* in the Inductor Prolog engine (this is not an exhaustive list):
- support for lists
- Only facts can be asserted or retracted
- Can't declare a function as dynamic like `dynamic(myRule/1)`
	- Anything can be changed in IndProlog, this declaration is not necessary
- `;` (or)
- `->` (if)
- syntax like `a == b` instead of `==(a, b)`
- `"` inside comments.  Use `"This is a quote 'inside another quote' "` instead
- Any Metaprogramming features or rules like `call`
	

## To Build
indprolog is designed to be built with [CMake](https://cmake.org) like this:

1. [Install CMake on your machine](https://cmake.org/install/)
2. Go to the root of the indprolog repository and create a build directory. 
	unix: `mkdir build`
	win: `md build`
3. Change to that directory.
	unix and win: `cd build`
4. CMake can build different types of projects using "generators".  Run `cmake -help` to get a list of generators on your system:
	unix and win: `cmake -help`
4. Pick the generator that will create the type of project you want and use the `-G` option to choose it. Here are the ones that have been tested:
	Mac make file: 			`cmake -G "Unix Makefiles" ../src`
	Mac Xcode:	 			`cmake -G "Xcode" ../src`
	Windows Visual Studio: 	`cmake -G "Visual Studio 16 2019" ../src`
5. Then actually do the build using this command which magically builds whatever you choose on the command line: 
	`cmake --build ./`
5a. OR you can manually use the build system that got created by cmake:
	unix or mac make file: 	`make`
	Mac Xcode:				Open the IndProlog.xcodeproj file in the build directory using Xcode.
	Windows Visual Studio: 	Open the .sln file in the build directory using VS.

## Running Tests
If you're using a command line generator of some sort, just run `runtests` on the commandline in your operating system of choice.


### Xcode
In Xcode, after you build you should change the scheme to `runtests` and then choose Product/Run.  The output of the test will appear in the Output window.


### Visual Studio
Set the default project to runtests and hit F5. You'll get a console window with the results

## Directory Structure
99.99% of the code for the Prolog compiler is platform agnostic (or at least should be). It has been built and tested on Windows, Mac and iOS. The platform specific code is located in the iOS and Win directories and is currently only a single function for debug logging.

- /FXPlatform: 			Contains some general purpose code for tracing, asserts, strings, etc
- /FXPlatform/Parser: 	The Inductor Parser code. A detailed description of how the Parser works is available [here](https://github.com/EricZinda/InductorParser)
- /FXPlatform/iOS: 		Code specific to iOS and Mac
- /FXPlatform/Win: 		Code specific to Windows
- /FXPlatform/Prolog: 	The Prolog compiler and runtime engine

- /UnitTest++:			The [UnitTest++](https://github.com/unittest-cpp/unittest-cpp) framework used to write unit tests 
- /Tests:				Basic smoke tests used to make sure it compiled properly

## Getting Started

Read GettingStarted.md.

License
---------
Do what you like, with no warranties! Read License.md.