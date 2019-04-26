//
//  PrologCompiler.hpp
//  GameLib
//
//  Created by Eric Zinda on 10/10/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//

#ifndef PrologQueryCompiler_hpp
#define PrologQueryCompiler_hpp
#include <vector>
#include "FXPlatform/Parser/Compiler.h"
#include "HtnGoalResolver.h"
#include "PrologParser.h"
class HtnTerm;
class HtnTermFactory;
class HtnRuleSet;
using namespace Prolog;

class PrologQueryCompiler : public Compiler<PrologQuery>
{
public:
	PrologQueryCompiler(HtnTermFactory *termFactory) :
        m_termFactory(termFactory)
    {
    }
	void Clear();

    ValueProperty(private, HtnTermFactory *, termFactory);
    
private:
    virtual bool ProcessAst(shared_ptr<CompileResultType> ast);
	ValueProperty(private, std::vector<shared_ptr<HtnTerm>>, result);
};

#endif /* PrologQueryCompiler_hpp */
