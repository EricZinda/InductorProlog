//
//  AIPrologTests.cpp
//  TestLib
//
//  Created by Eric Zinda on 10/15/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//
#include "FXPlatform/FailFast.h"
#include "FXPlatform/Parser/ParserDebug.h"
#include "FXPlatform/Prolog/HtnGoalResolver.h"
#include "FXPlatform/Prolog/PrologCompiler.h"
#include "FXPlatform/Prolog/HtnRuleSet.h"
#include "FXPlatform/Prolog/HtnTermFactory.h"
#include "FXPlatform/Prolog/PrologParser.h"
#include "FXPlatform/Prolog/PrologCompiler.h"
#include "Logger.h"
#include "ParserTestBase.h"
#include <thread>
#include "UnitTest++/UnitTest++.h"
using namespace Prolog;


SUITE(PrologCompilerTests)
{
    TEST(PrologCompilerTests)
    {
//                        SetTraceFilter(SystemTraceType::Parsing, TraceDetail::Diagnostic);
        
        shared_ptr<HtnTermFactory> factory = shared_ptr<HtnTermFactory>(new HtnTermFactory());
        shared_ptr<HtnRuleSet> state = shared_ptr<HtnRuleSet>(new HtnRuleSet());
        // shared_ptr<HtnPlanner> planner = shared_ptr<HtnPlanner>(new HtnPlanner());
        shared_ptr<PrologCompiler> compiler;
        
        // Facts
        state->ClearAll();
        compiler = shared_ptr<PrologCompiler>(new PrologCompiler(factory.get(), state.get()));
        CHECK(compiler->Compile("a."));
        CHECK(state->DebugHasRule("a", ""));

        state->ClearAll();
        compiler = shared_ptr<PrologCompiler>(new PrologCompiler(factory.get(), state.get()));
        CHECK(compiler->Compile("a(b)."));
        CHECK(state->DebugHasRule("a(b)", ""));

        // Rules
        state->ClearAll(); 
        compiler = shared_ptr<PrologCompiler>(new PrologCompiler(factory.get(), state.get()));
        CHECK(compiler->Compile("a(b) :-."));
        CHECK(state->DebugHasRule("a(b)", ""));
    }
    
    TEST_FIXTURE(ParserTestBase, PrologParserTests)
    {
        int deepestFailure;
        string result;
        string errorMessage;
        shared_ptr<Symbol> rule;
        vector<shared_ptr<Symbol>> flattenedTree;
        
//        SetTraceFilter(SystemTraceType::Parsing, TraceDetail::Diagnostic);
        
        // atom
        rule = TestTryParse<PrologAtom>("constant", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("con-stant", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("con_stant", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>(">", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>(">=", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("1", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("+1", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("-1", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("1.2", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("+1.2", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologAtom>("-1.2", deepestFailure, errorMessage);
        CHECK(rule != nullptr);


        // variable
        rule = TestTryParse<PrologVariable>("?a", deepestFailure, errorMessage);
        CHECK(rule != nullptr);


        // term
        rule = TestTryParse<PrologTerm>("a", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologTerm>("?a", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologTerm>("a(b)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);


        // term list
        rule = TestTryParse<PrologTermList>("foo(a)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        // functor
        rule = TestTryParse<PrologFunctor>("a()", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("a(b)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("-(b,c)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("a(b,c)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("a(?b,?c)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("a( ?b , ?c )", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("a( d(e,f,g) , ?c )", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologFunctor>("do(Move(?unit,?from,?to),SetEnergy(?energy,-(?energy,?moveCost)))", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        // rule
        rule = TestTryParse<PrologRule>("a :- ", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologRule>("a(g) :- ", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologRule>("a( g , ef ) :- ", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
        
        rule = TestTryParse<PrologRule>("a(g) :- b(c)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologRule>("a(g) :- b(c), d(a,b,c)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologRule>("a( g, ef) :- foo(a)", deepestFailure, errorMessage);
        CHECK(rule != nullptr);


        // Document
        rule = TestTryParse<PrologDocument>("a.  ", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologDocument>("a.  b.", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologDocument>("a(g, ef) :- foo(a).", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologDocument>("north :- go(north). \r\nsouth :- go(south).", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologDocument>("a(g).  ", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
        
        rule = TestTryParse<PrologDocument>("a(g).  \r\nb(d,e(f,g)) :-.", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
        
        // Examples
        rule = TestTryParse<PrologDocument>("operator(SetEnergy(?old,?new), del(Energy(?old)), add(Energy(?new)) ).", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
        
        rule = TestTryParse<PrologDocument>("goals(findSolution(a)).", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
        
        rule = TestTryParse<PrologDocument>("method(MoveUnit(?unit, ?to), if(UnitIdle(?unit),At(?unit,?from)),   do(Move(?unit,?from,?to),SetEnergy(?energy,-(?energy,?moveCost))) ).", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
        
        // Comment
        rule = TestTryParse<PrologComment>("%\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("%foo\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("%foo\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("%foo\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("%foo\r", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("%foo\n\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("/* test */", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologComment>("/* test \r\n test2 */", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        // Whitespace
        rule = TestTryParse<PrologOptionalWhitespace>(" ", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologOptionalWhitespace>(" \r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologOptionalWhitespace>("%foo\n\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologOptionalWhitespace>("\r\n  %foo\n\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologDocument>("a(a). b(b). a(b, c). % This is a comment\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);

        rule = TestTryParse<PrologDocument>("a(%\na%\n)%\n.%\n b(%\nb%\n)%\n.%\n a(%\nb,%\n c%\n)%\n. % This is a comment\r\n", deepestFailure, errorMessage);
        CHECK(rule != nullptr);
    }
}
