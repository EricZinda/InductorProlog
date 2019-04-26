//
//  PrologCompiler.cpp
//  GameLib
//
//  Created by Eric Zinda on 10/10/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//

#include "ParserDebug.h"
#include "PrologCompiler.h"
#include "HtnGoalResolver.h"
#include "HtnRuleSet.h"
#include "HtnTerm.h"
#include "HtnTermFactory.h"
using namespace Prolog;

void PrologCompiler::ClearWithNewRuleSet()
{
    m_goals.clear();
    m_compilerOwnedRuleSet = shared_ptr<HtnRuleSet>(new HtnRuleSet());
    m_state = m_compilerOwnedRuleSet.get();
}

void PrologCompiler::Clear()
{
    m_goals.clear();
    m_state->ClearAll();
}

shared_ptr<HtnTerm> PrologCompiler::CreateTermFromFunctor(HtnTermFactory *factory, shared_ptr<Symbol> functor)
{
    shared_ptr<Symbol> name = GetChild(functor, 0, -1);
    
    vector<shared_ptr<HtnTerm>> arguments;
    for(int argIndex = 1; argIndex < functor->children().size(); argIndex++)
    {
        shared_ptr<Symbol> arg = GetChild(functor, argIndex, -1);
        arguments.push_back(CreateTermFromItem(factory, arg));
    }
    
    return factory->CreateFunctor(name->ToString(), arguments);
}

shared_ptr<HtnTerm> PrologCompiler::CreateTermFromItem(HtnTermFactory *factory, shared_ptr<Symbol> symbol)
{
    switch(symbol->symbolID())
    {
        case PrologSymbolID::PrologFunctor:
            return CreateTermFromFunctor(factory, symbol);
            break;
        case PrologSymbolID::PrologVariable:
            return CreateTermFromVariable(factory, symbol);
            break;
        case PrologSymbolID::PrologAtom:
            return factory->CreateConstant(symbol->ToString());
            break;
        case PrologSymbolID::PrologList:
            return CreateTermFromList(factory, symbol);
        default:
            StaticFailFastAssert(false);
            return nullptr;
    }
}

// Lists are defined inductively: The atom [] is a list. A compound term with functor . (dot) and arity 2, whose second argument is a list, is itself a list.
// There exists special syntax for denoting lists: .(A, B) is equivalent to [A|B]. For example, the list .(1, .(2, .(3, []))) can also be written as [1 | [2 | [3 | []]]],
// or even more compactly as [1,2,3]
shared_ptr<HtnTerm> PrologCompiler::CreateTermFromList(HtnTermFactory *factory, shared_ptr<Symbol> list)
{
    vector<shared_ptr<HtnTerm>> arguments;
    for(int argIndex = 0; argIndex < list->children().size(); argIndex++)
    {
        shared_ptr<Symbol> arg = GetChild(list, argIndex, -1);
        arguments.push_back(CreateTermFromItem(factory, arg));
    }
    
    // Now create a list starting from back to front, right term of the deepest child is [] atom
    shared_ptr<HtnTerm>rightTerm = factory->CreateConstant("[]");
    for(vector<shared_ptr<HtnTerm>>::reverse_iterator iter = arguments.rbegin(); iter != arguments.rend(); iter++)
    {
        shared_ptr<HtnTerm>arg = *iter;
        rightTerm = factory->CreateFunctor(".", { arg, rightTerm });
    }
    
    return rightTerm;
}

shared_ptr<HtnTerm> PrologCompiler::CreateTermFromVariable(HtnTermFactory *factory, shared_ptr<Symbol> variable)
{
    int level1Index = 0;
    shared_ptr<Symbol> name = GetChild(variable, level1Index++, -1);
    string nameString = name->ToString();
    return factory->CreateVariable(nameString);
}

void PrologCompiler::ParseAtom(shared_ptr<Symbol> symbol)
{
    vector<shared_ptr<HtnTerm>> emptyTail;
    m_state->AddRule(m_termFactory->CreateConstant(symbol->ToString()), emptyTail);
}


void PrologCompiler::ParseRule(shared_ptr<Symbol> symbol)
{
    shared_ptr<Symbol> head = GetChild(symbol, 0, -1);
    
    vector<shared_ptr<HtnTerm>> list;
    for(int index = 1; index < symbol->children().size(); index++)
    {
        shared_ptr<Symbol> item = GetChild(symbol, index, -1);
        list.push_back(CreateTermFromItem(m_termFactory, item));
    }
    
    m_state->AddRule(CreateTermFromFunctor(m_termFactory, head), list);
}

void PrologCompiler::ParseTopLevelFunctor(shared_ptr<Symbol> symbol)
{
    // Some top level functors could be reserved words
    string name = GetChild(symbol, 0, -1)->ToString();
    
    shared_ptr<HtnTerm>term = CreateTermFromFunctor(m_termFactory, symbol);
    if(term->name() == "goals")
    {
        m_goals.insert(m_goals.end(), term->arguments().begin(), term->arguments().end());
    }
    else
    {
        // Interpret top level functors that aren't reserved words as facts
        vector<shared_ptr<HtnTerm>> emptyTail;
        m_state->AddRule(term, emptyTail);
    }
}

// The only thing at the top level are rules or functors
bool PrologCompiler::ProcessAst(shared_ptr<CompileResultType> result)
{
    FailFastAssert(m_termFactory != nullptr && m_state != nullptr);
    
//    string foo = ParserDebug::PrintTree(*result);
    
    // Top level symbol is a document, we want to iterate its children
    for(auto item : (*result)[0]->children())
    {
        //        string output = ParserDebug::PrintTree(functor, 0);
        //        TraceString1("{0}", SystemTraceType::Parsing, TraceDetail::Normal, output);
        switch(item->symbolID())
        {
            case PrologSymbolID::PrologAtom:
                ParseAtom(item);
                break;
            case PrologSymbolID::PrologFunctor:
                ParseTopLevelFunctor(item);
                break;
            case PrologSymbolID::PrologRule:
                ParseRule(item);
                break;
            default:
                FailFastAssert(false);
        }
    }
    
    return true;
}


