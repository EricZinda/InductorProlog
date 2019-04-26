target_sources(lib
    PRIVATE
    	FailFast.h
    	FailFast.cpp
        Logger.h
    	NanoTrace.h
    	NanoTrace.cpp
    	ReflectionEnum.h
    	Utilities.h
    	Utilities.cpp
)
target_include_directories(lib PRIVATE ./)