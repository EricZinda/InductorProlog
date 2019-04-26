#include <cctype>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "FXPlatform/Prolog/PrologCompiler.h"
#include "FXPlatform/Prolog/HtnRuleSet.h"
#include "FXPlatform/Prolog/HtnTermFactory.h"

int main (int argc, char *argv[])
{
	if(argc != 2)
	{
		  fprintf(stdout, 
		  	"IndProlog example from https://github.com/EricZinda/InductorProlog. \r\n"
		  	"Pass the name of a Prolog document on the command line and then execute prolog queries interactively.\r\n"
		  	"Examples: \r\n"
		  	"	indprolog rule.txt \r\n"
		  	"	? myRule(What).  << You type that\r\n"
		  	"	What = something << System returns value\r\n"
			);
	}
	else
	{
	    string targetFileAndPath = string(argv[1]);

        shared_ptr<HtnTermFactory> factory = shared_ptr<HtnTermFactory>(new HtnTermFactory());
        shared_ptr<HtnRuleSet> state = shared_ptr<HtnRuleSet>(new HtnRuleSet());
    	PrologCompiler compiler(factory.get(), state.get());
    	if(compiler.CompileDocument(targetFileAndPath))
    	{
    		fprintf(stdout, "Succesfully compiled %s\r\n\r\nType a Prolog query or hit ctrl-C to end.\r\n\r\n", targetFileAndPath.c_str());
    		fprintf(stdout, "?");

	     //    HtnGoalResolver resolver;
    		// string input;
    		// vector<shared_ptr<HtnTerm>> query;
    		// while(cin >> input)
    		// {
    		// 	resolver.ResolveAll(factory, state, query);

	    	// 	fprintf(stdout, ">> %s\r\n\r\n", input.c_str());	    		
	    	// 	fprintf(stdout, "?");
    		// }
    		return 0;
    	}
    	else
    	{
    		fprintf(stdout, "Error compiling %s, %s\r\n", targetFileAndPath.c_str(), compiler.GetErrorString().c_str());
    		return 1;
    	}
	}

  	return 0;
}