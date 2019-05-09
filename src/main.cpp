#include <cctype>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "FXPlatform/Prolog/PrologCompiler.h"
#include "FXPlatform/Prolog/PrologQueryCompiler.h"
#include "FXPlatform/Prolog/HtnRuleSet.h"
#include "FXPlatform/Prolog/HtnTermFactory.h"

int main (int argc, char *argv[])
{
//    SetTraceFilter(SystemTraceType::Solver, TraceDetail::Diagnostic);

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

		// IndProlog uses a factory model for creating terms so it can "intern" them to save memory.  You must never
		// mix terms from different HtnTermFactorys
        shared_ptr<HtnTermFactory> factory = shared_ptr<HtnTermFactory>(new HtnTermFactory());

        // HtnRuleSet is where the facts and rules are stored for the program.  It is the database.
        shared_ptr<HtnRuleSet> state = shared_ptr<HtnRuleSet>(new HtnRuleSet());

        // The PrologStandardCompiler will use the normal Prolog parsing rules where capitalized things are variables
		// An alternative used by the HTN engine (see blog mentioned in readme.md for more details) is called PrologCompiler
    	PrologStandardCompiler compiler(factory.get(), state.get());

    	// There are variants of CompileDocument() that take paths, streams or simply strings
    	if(compiler.CompileDocument(targetFileAndPath))
    	{
    		fprintf(stdout, "Succesfully compiled %s\r\n\r\nType a Prolog query or hit q to end.\r\n\r\n", targetFileAndPath.c_str());
    		fprintf(stdout, "?- ");

			// The PrologStandardQueryCompiler will compile prolog queries using the normal Prolog parsing rules
			PrologStandardQueryCompiler queryCompiler(factory.get());

			// HtnGoalResolver is the Prolog "engine" that resolves queries
	        HtnGoalResolver resolver;
			string input;
			while(true)
			{
                input.clear();
                string tempInput;
                std::getline(cin, tempInput);
                for(auto c : tempInput)
                {
                    // Xcode console window will pass along all backspace, arrow keys, etc instead of just the resulting text
                    // get rid of those.
                    // NOTE: Xcode still insists on autocompleting the right parenthesis after you type something like "a(b"
                    // but DOES NOT pass along the second parenthesis to cin.  So, you have to type it twice when using Xcode.
                    if(c >= 0x20 && c <= 0x7E)
                    {
                        input.push_back(c);
                    }
                }

				if (input == "q") break;
//                cout << "received: " << input << endl;
				if(queryCompiler.Compile(input))
				{
					// The resolver can give one answer at a time using ResolveNext(), or just get them all using ResolveAll()
					shared_ptr<vector<UnifierType>> queryResult = resolver.ResolveAll(factory.get(), state.get(), queryCompiler.result());
					if (queryResult == nullptr)
					{
						fprintf(stdout, ">> false\r\n\r\n");
					}
					else
					{
						fprintf(stdout, ">> %s\r\n\r\n", HtnGoalResolver::ToString(queryResult.get()).c_str());
					}
				}
				else
				{
					fprintf(stdout, "Error: %s\r\n\r\n", queryCompiler.GetErrorString().c_str());
				}

				queryCompiler.Clear();
				fprintf(stdout, "?- ");
			}
    		return 0;
    	}
    	else
    	{
    		fprintf(stdout, "Error compiling %s, %s\r\n", targetFileAndPath.c_str(), compiler.GetErrorString().c_str());
    		return 1;
    	}
	}
}
