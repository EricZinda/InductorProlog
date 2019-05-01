target_sources(runtests
    PRIVATE
	HtnGoalResolverTests.cpp
	HtnRuleSetTests.cpp
	HtnTermTests.cpp
	ParserTestBase.h
    	PrologCompilerTests.cpp
)
target_include_directories(testLib PRIVATE ./)
