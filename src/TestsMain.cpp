#include "FXPlatform/FailFast.h"
#include "UnitTest++/UnitTest++.h"
#include "UnitTest++/TestReporterStdout.h"
using namespace UnitTest;

// This class filters out what tests get run if you are isolating failures
class TestFilter
{
    public:
    bool operator()(UnitTest::Test *test) const
    {
        // Return true if the test matches the criteria for a test you want to run. Examples:
        // Just run this one test:
//              return strcmp(test->m_details.testName, "HtnGoalResolverTrueFalseTests") == 0;
        // Run just this suite of tests:
//              return strcmp(test->m_details.suiteName, "PrologCompilerTests") == 0;
        
        // Return true to run everything
        return true;
    }
};

int main (int argc, char *argv[])
{
	// Treat all FailFasts as exceptions when running tests so the process doesn't abort
	TreatFailFastAsException(true);

    TestReporterStdout reporter;
    TestRunner runner(reporter);
    return runner.RunTestsIf(Test::GetTestList(), NULL, TestFilter(), 0);
}
