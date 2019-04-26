//
//  PrologParser.hpp
//  GameLib
//
//  Created by Eric Zinda on 10/10/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//

#ifndef PrologParser_hpp
#define PrologParser_hpp

#include "FXPlatform/Parser/Parser.h"
using namespace FXPlat;
namespace Prolog
{
    class PrologFunctor;
    class PrologList;
    
    extern char CrlfString[];

    class PrologSymbolID
    {
    public:
        SymbolDef(PrologAtom, CustomSymbolStart + 0);
        SymbolDef(PrologVariable, CustomSymbolStart + 1);
        SymbolDef(PrologTerm, CustomSymbolStart + 2);
        SymbolDef(PrologFunctor, CustomSymbolStart + 3);
        SymbolDef(PrologRule, CustomSymbolStart + 4);
        SymbolDef(PrologDocument, CustomSymbolStart + 5);
        SymbolDef(PrologList, CustomSymbolStart + 6);
        SymbolDef(PrologComment, CustomSymbolStart + 7);
        
        // Must be last so that other parsers can extend
        SymbolDef(PrologMaxSymbol, CustomSymbolStart + 8);
    };

    //    a comment starts with % and can have anything after it until it hits a group of newline, carriage returns in any order and in any number
    class PrologComment : public
        AndExpression<Args
        <
            CharacterSymbol<PercentString, FlattenType::None>,
            ZeroOrMoreExpression<CharacterSetExceptSymbol<CrlfString>>,
            OneOrMoreExpression<CharacterSetSymbol<CrlfString>>
        >, FlattenType::None, PrologSymbolID::PrologComment>
    {
    };
    
    // HtnWhitespace can have normal whitespace or a valid comment
    class PrologOptionalWhitespace : public
    ZeroOrMoreExpression
    <
        OrExpression<Args
        <
            WhitespaceSymbol<>,
            PrologComment
        >>, FlattenType::Delete
    >
    {
    };

    // An atom is a general-purpose name with no inherent meaning. It is composed of a sequence of characters that is parsed by the Prolog reader as a single unit. Atoms are usually bare words in Prolog code,
    // written with no special syntax. However, atoms containing spaces or certain other special characters must be surrounded by single quotes. Atoms beginning with a capital letter must also be quoted, to
    // distinguish them from variables. The empty list, written [], is also an atom. Other examples of atoms include x, blue, 'Taco', and 'some atom'.
    // atom ::= (MathSymbol)+ OR Integer OR Float OR ((CharSymbol | UnderscoreSymbol | Hyphen) (CharSymbol | NumberSymbol | UnderscoreSymbol)*)
    class PrologAtom : public
        OrExpression<Args
        <
            Float<FlattenType::Flatten>,
            Integer<FlattenType::Flatten>,
            OneOrMoreExpression
            <
                MathSymbol
            >,
            AndExpression<Args
            <
                OrExpression<Args<
                    CharSymbol,
                    CharacterSymbol<Underscore, FlattenType::None>,
                    CharacterSymbol<HyphenString, FlattenType::None>
                >>,
                ZeroOrMoreExpression
                <
                    OrExpression<Args
                    <
                        CharOrNumberSymbol,
                        CharacterSymbol<Underscore, FlattenType::None>,
                        CharacterSymbol<HyphenString, FlattenType::None>
                    >>
                >
            >>
        >, FlattenType::None, PrologSymbolID::PrologAtom>
    {
    };
    
    //    a variable starts with ? followed by an atom
    class PrologVariable : public
        AndExpression<Args
        <
            CharacterSymbol<QuestionMarkString, FlattenType::Delete>,
            PrologAtom
        >, FlattenType::None, PrologSymbolID::PrologVariable>
    {
    };

    // A term is an atom, variable or functor
    class PrologTerm : public
    OrExpression<Args
    <
        PrologFunctor,
        PrologList,
        PrologAtom,
        PrologVariable
    >, FlattenType::Flatten>
    {
    };

    class PrologTermList : public
    AndExpression<Args
    <
        PrologTerm,
        PrologOptionalWhitespace,
        ZeroOrMoreExpression
        <
            AndExpression<Args
            <
                CharacterSymbol<CommaString>,
                PrologOptionalWhitespace,
                PrologTerm,
                PrologOptionalWhitespace
            >>
        >
    >>
    {
    };
    
    class PrologList : public
    AndExpression<Args
    <
        CharacterSymbol<LeftBracketString>,
        PrologOptionalWhitespace,
        OptionalExpression
        <
            PrologTermList
        >,
        PrologOptionalWhitespace,
        CharacterSymbol<RightBracketString>
    >, FlattenType::None, PrologSymbolID::PrologList>
    {
    };
    
    // A compound term is composed of an atom called a "functor" and a number of "arguments", which are again terms. Compound terms are ordinarily written as a functor
    // followed by a comma-separated list of argument terms, which is contained in parentheses. The number of arguments is called the term's arity. An atom can be regarded
    // as a compound term with arity zero.
    // Special case the period string since it is used for lists
    class PrologFunctor : public
    AndExpression<Args
    <
        OrExpression<Args<
            PrologAtom,
            CharacterSymbol<PeriodString>
        >>,
        CharacterSymbol<LeftParenthesisString>,
        PrologOptionalWhitespace,
        OptionalExpression
        <
            PrologTermList
        >,
        PrologOptionalWhitespace,
        CharacterSymbol<RightParenthesisString>
    >, FlattenType::None, PrologSymbolID::PrologFunctor>
    {
    };

    class PrologRule : public
    AndExpression<Args
    <
        PrologFunctor,
        PrologOptionalWhitespace,
        CharacterSymbol<ColonString>,
        CharacterSymbol<DashString>,
        PrologOptionalWhitespace,
        OptionalExpression
        <
            PrologTermList
        >
    >, FlattenType::None, PrologSymbolID::PrologRule>
    {
    };
    
    class PrologDocument : public
    AndExpression<Args<
        OneOrMoreExpression
        <
            AndExpression<Args<
                PrologOptionalWhitespace,
                OrExpression<Args<
                    PrologRule,
                    PrologFunctor,
                    PrologAtom
                >>,
                PrologOptionalWhitespace,
                CharacterSymbol<PeriodString>,
                PrologOptionalWhitespace
            >>
        >,
        PrologOptionalWhitespace,
        EofSymbol
    >, FlattenType::None, PrologSymbolID::PrologDocument>
    {
    };
}
#endif /* PrologParser_hpp */
