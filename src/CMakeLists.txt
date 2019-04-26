cmake_minimum_required(VERSION 3.14)

project(IndProlog CXX)
set(CMAKE_CXX_STANDARD 11)

add_library(lib "")
target_include_directories(lib PRIVATE ./)

add_executable(indprolog main.cpp)
target_include_directories(indprolog PRIVATE ./)

add_subdirectory(FXPlatform)
if(WIN32)
	# Link with Kernel32.lib so we can output debug messages
	target_link_libraries(indprolog lib Kernel32.lib)
	add_subdirectory(FXPlatform/Win)
else()
	target_link_libraries(indprolog lib "-framework Foundation")
	add_subdirectory(FXPlatform/iOS)
endif()

add_subdirectory(FXPlatform/Parser)
add_subdirectory(FXPlatform/Prolog)


