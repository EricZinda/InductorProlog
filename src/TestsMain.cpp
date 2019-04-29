#include "UnitTest++/UnitTest++.h"

void LoadTests();

int main (int argc, char *argv[])
{
	// Just here to make the linker load that library since it is all statics
	LoadTests();
	return UnitTest::RunAllTests();

  	return 0;
}