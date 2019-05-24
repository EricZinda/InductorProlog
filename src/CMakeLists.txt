cmake_minimum_required(VERSION 3.14)

project(IndProlog CXX)
set(CMAKE_CXX_STANDARD 11)

add_executable(runtests TestsMain.cpp)
add_library(testLib "")

add_executable(indprolog main.cpp)
add_library(lib "")

target_include_directories(indprolog PRIVATE ./)
target_include_directories(lib PRIVATE ./)
target_include_directories(testLib PRIVATE ./)
target_include_directories(runtests PRIVATE ./)

#### Prolog

add_subdirectory(FXPlatform)
add_subdirectory(FXPlatform/Parser)
add_subdirectory(FXPlatform/Prolog)

if(WIN32)
	# Link with Kernel32.lib so we can output debug messages
	target_link_libraries(indprolog lib Kernel32.lib)
	target_link_libraries(runtests lib testLib Kernel32.lib)
	add_subdirectory(FXPlatform/Win)
else()
	target_link_libraries(indprolog lib "-framework Foundation")
	target_link_libraries(runtests testLib lib "-framework Foundation")
	add_subdirectory(FXPlatform/iOS)
endif()


#### Tests

add_subdirectory(UnitTest++)
add_subdirectory(Tests)
add_subdirectory(Tests/Prolog)

if(WIN32)
	add_subdirectory(UnitTest++/Win32)
else()
	add_subdirectory(UnitTest++/Posix)
endif()


