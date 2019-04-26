//
//  PrologCompiler.hpp
//  GameLib
//
//  Created by Eric Zinda on 10/10/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//

#ifndef PrologCompiler_hpp
#define PrologCompiler_hpp

#include "FXPlatform/Parser/Compiler.h"
#include "HtnGoalResolver.h"
#include "PrologParser.h"
class HtnTerm;
class HtnTermFactory;
class HtnRuleSet;
using namespace Prolog;

class PrologCompiler : public Compiler<PrologDocument>
{
public:
    PrologCompiler(HtnTermFactory *termFactory, HtnRuleSet *state) :
        m_state(state),
        m_termFactory(termFactory)
    {
    }
    
    void Clear();
    // Helper that creates a new ruleset and sets it instead of just clearing it.
    // Useful if the RuleSet has been locked
    void ClearWithNewRuleSet();
    shared_ptr<vector<UnifierType>> SolveGoals()
    {
        HtnGoalResolver resolver;
        return SolveGoals(resolver);
    }

    shared_ptr<vector<UnifierType>> SolveGoals(HtnGoalResolver &resolver)
    {
        return resolver.ResolveAll(m_termFactory, m_state, m_goals);
    }

    shared_ptr<vector<UnifierType>> SolveGoals(HtnGoalResolver *resolver, int memoryBudget = 1000000)
    {
        return resolver->ResolveAll(m_termFactory, m_state, m_goals, 0, memoryBudget);
    }

    static shared_ptr<HtnTerm> CreateTermFromFunctor(HtnTermFactory *factory, shared_ptr<Symbol> functor);
    static shared_ptr<HtnTerm> CreateTermFromItem(HtnTermFactory *factory, shared_ptr<Symbol> symbol);
    static shared_ptr<HtnTerm> CreateTermFromList(HtnTermFactory *factory, shared_ptr<Symbol> variable);
    static shared_ptr<HtnTerm> CreateTermFromVariable(HtnTermFactory *factory, shared_ptr<Symbol> variable);
    
    vector<shared_ptr<HtnTerm>> &goals() { return m_goals; }

    ValueProperty(private, shared_ptr<HtnRuleSet>, compilerOwnedRuleSet);
    ValueProperty(private, HtnRuleSet *, state);
    ValueProperty(private, HtnTermFactory *, termFactory);
    
private:
    vector<shared_ptr<HtnTerm>> m_goals;
    void ParseAtom(shared_ptr<Symbol> symbol);
    void ParseRule(shared_ptr<Symbol> symbol);
    void ParseTopLevelFunctor(shared_ptr<Symbol> functor);
    virtual bool ProcessAst(shared_ptr<CompileResultType> ast);
};

#endif /* PrologCompiler_hpp */
