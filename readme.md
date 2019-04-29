Inductor Prolog Compiler
========================

It is used in production in an iPhone strategy game called [Exospecies](https://www.exospecies.com). Visit the [Exospecies Blog](https://www.exospecies.com/blog) for more details.

Use it and enjoy!

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
	Windows Visual Studio: 	`cmake ../src`
5. Then actually do the build using this command which magically builds whatever you choose on the command line: 
	`cmake --build ./`

5a. OR you can manually use the build system that got created by cmake:
	unix or mac make file: 	`make`
	Mac Xcode:				Open the IndProlog.xcodeproj file in the build directory. Build the project using Xcode.
	Windows Visual Studio: 	Open the .sln file in the build directory.  Build the solution from VS.

## Running Tests
If you're using a command line generator of some sort, just run `runtests` on the commandline in your operator system of choice.

### Xcode
In Xcode, after you build you should change the scheme to `runtests` and then choose Product/Run.  The output of the test will appear in the Output window.


Directory Structure
-------------------
99.99% of the code for the Prolog compiler is platform agnostic (or at least should be). It has been built and tested on Windows, Mac and iOS. The platform specific code is located in the iOS and Win directorys and is currently only a single function for debug logging.

- /FXPlatform: 			Contains some general purpose code for tracing, asserts, strings, etc
- /FXPlatform/Parser: 	The Inductor Parser code
- /FXPlatform/iOS: 		Code specific to iOS and Mac
- /FXPlatform/Win: 		Code specific to Windows
- /FXPlatform/Prolog: 	The prolog compiler and runtime engine

- /UnitTest++:			The UnitTest++ framework used to write unit tests (https://github.com/unittest-cpp/unittest-cpp)
- /Tests:				Basic smoke tests used to make sure it call compiled properly

Getting Started
-------------------
Read GettingStarted.md.

License
---------
Do what you like, with no warranties! Read License.md.