target_sources(lib
    PRIVATE
    	FailFast.h
    	FailFast.cpp
        Logger.h
    	NanoTrace.h
    	NanoTrace.cpp
    	ReflectionEnum.h
    	SystemTraceType.h
    	Utilities.h
    	Utilities.cpp
)
target_include_directories(lib PRIVATE ./)
target_include_directories(testLib PRIVATE ./)
target_include_directories(runtests PRIVATE ./)