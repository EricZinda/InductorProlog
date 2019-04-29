//
//  PrologCompiler.cpp
//  GameLib
//
//  Created by Eric Zinda on 10/10/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//

#include "ParserDebug.h"
#include "PrologQueryCompiler.h"
#include "PrologCompiler.h"
#include "HtnTerm.h"
#include "HtnTermFactory.h"
using namespace Prolog;

void PrologQueryCompiler::Clear()
{
	Initialize();
	m_result.clear();
}

// A Prolog Query is simply a TermList
bool PrologQueryCompiler::ProcessAst(shared_ptr<CompileResultType> result)
{
    FailFastAssert(m_termFactory != nullptr);
	// There should only ever be a single PrologQuery item if our parser is working
	FailFastAssert(result->size() == 1 && (*result)[0]->symbolID() == PrologSymbolID::PrologQuery);
        
    // Top level symbol is a Query, we want to iterate its children which will be functors
    for(auto item : (*result)[0]->children())
    {
        switch(item->symbolID())
        {
            case PrologSymbolID::PrologFunctor:
                m_result.push_back(PrologCompiler::CreateTermFromFunctor(m_termFactory, item));
                break;
            default:
                FailFastAssert(false);
        }
    }
    
    return true;
}


