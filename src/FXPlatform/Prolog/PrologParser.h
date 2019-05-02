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
    
    // Errors
    extern char PrologAtomError[];
    extern char PrologVariableError[];
    extern char PrologTermError[];
    extern char PrologFunctorError[];
    extern char PrologFunctorListError[];
    extern char PrologRuleError[];
    extern char PrologTermListError[];
    extern char PrologDocumentError[];
    extern char PrologCommentError[];
    extern char PrologQueryError[];

	extern char BeginCommentBlock[];
    extern char CrlfString[];
	extern char CapitalChar[];
	extern char EndCommentBlock[];

    class PrologSymbolID
    {
    public:
        SymbolDef(PrologAtom, CustomSymbolStart + 0);
        SymbolDef(PrologVariable, CustomSymbolStart + 1);
        SymbolDef(PrologTerm, CustomSymbolStart + 2);
        SymbolDef(PrologFunctor, CustomSymbolStart + 3);
        SymbolDef(PrologRule, CustomSymbolStart + 4);
        SymbolDef(PrologDocument, CustomSymbolStart + 5);
        SymbolDef(PrologComment, CustomSymbolStart + 6);
		SymbolDef(PrologQuery, CustomSymbolStart + 7);

        // Must be last so that other parsers can extend
        SymbolDef(PrologMaxSymbol, CustomSymbolStart + 8);
    };

    //    a comment starts with % and can have anything after it until it hits a group of newline, carriage returns in any order and in any number
	// or it is a block comment  /* comment */
    class PrologComment : public
		OrExpression<Args
		<
			AndExpression<Args
			<
				CharacterSymbol<PercentString, FlattenType::None>,
				ZeroOrMoreExpression<CharacterSetExceptSymbol<CrlfString>>,
				OneOrMoreExpression<CharacterSetSymbol<CrlfString>>
			>>,
			AndExpression<Args
			<
				LiteralExpression<BeginCommentBlock>,
				NotLiteralExpression<EndCommentBlock>,
				LiteralExpression<EndCommentBlock>
			>>
		>, FlattenType::None, PrologSymbolID::PrologComment, PrologCommentError>
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
			CharacterSymbol<ExclamationPointString, FlattenType::None>,
            AndExpression<Args
            <
				CharacterSymbol<DoubleQuoteString>,
				ZeroOrMoreExpression
				<
					CharacterSetExceptSymbol<DoubleQuoteString>
				>,
				CharacterSymbol<DoubleQuoteString>
			>>,
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
        >, FlattenType::None, PrologSymbolID::PrologAtom, PrologAtomError>
    {
    };
    
	class PrologCapitalizedAtom : public
        AndExpression<Args
        <
			CharacterSetSymbol<CapitalChar>,
            ZeroOrMoreExpression
            <
                OrExpression<Args
                <
                    CharOrNumberSymbol,
                    CharacterSymbol<Underscore, FlattenType::None>,
                    CharacterSymbol<HyphenString, FlattenType::None>
                >>
            >
        >, FlattenType::None, PrologSymbolID::PrologAtom, PrologAtomError>
    {
    };

	// Variables start with a capital letter
    // In this custom Prolog engine, a variable can also start with ? followed by any atom (I find it more readable).
    class PrologVariable : public
		OrExpression<Args<
			AndExpression<Args
			<
				CharacterSymbol<QuestionMarkString, FlattenType::Delete>,
				PrologAtom
			>>,
			PrologCapitalizedAtom
        >, FlattenType::None, PrologSymbolID::PrologVariable, PrologVariableError>
    {
    };

    // A term is a variable or functor (which could have no arguments and thus be an atom)
    class PrologTerm : public
    OrExpression<Args
    <
        PrologFunctor,
		PrologVariable
    >, FlattenType::Flatten, 0, PrologTermError>
    {
    };

	class PrologFunctorList : public
    AndExpression<Args
    <
        PrologFunctor,
        PrologOptionalWhitespace,
        ZeroOrMoreExpression
        <
            AndExpression<Args
            <
                CharacterSymbol<CommaString>,
                PrologOptionalWhitespace,
				PrologFunctor,
                PrologOptionalWhitespace
            >>
        >
    >, FlattenType::Flatten, SymbolID::andExpression, PrologFunctorListError>
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
    >, FlattenType::Flatten, SymbolID::andExpression, PrologTermListError>
    {
    };
    
    // A compound term is composed of an atom called a "functor" and a number of "arguments", which are again terms. Compound terms are ordinarily written as a functor
    // followed by a comma-separated list of argument terms, which is contained in parentheses. The number of arguments is called the term's arity. An atom can be regarded
    // as a compound term with arity zero.
    class PrologFunctor : public
    AndExpression<Args
    <
        OrExpression<Args<
            PrologAtom
            // Special case the period string since it is used for lists i.e. .()
//            CharacterSymbol<PeriodString>
        >>,
        OptionalExpression
        <
            AndExpression<Args
            <
                CharacterSymbol<LeftParenthesisString>,
                PrologOptionalWhitespace,
                OptionalExpression
                <
                    PrologTermList
                >,
                PrologOptionalWhitespace,
                CharacterSymbol<RightParenthesisString>
            >>
        >
    >, FlattenType::None, PrologSymbolID::PrologFunctor, PrologFunctorError>
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
    >, FlattenType::None, PrologSymbolID::PrologRule, PrologRuleError>
    {
    };
    
	class PrologQuery : public
    AndExpression<Args<
		PrologOptionalWhitespace,
		PrologFunctorList,
		PrologOptionalWhitespace,
		CharacterSymbol<PeriodString>,
		PrologOptionalWhitespace,
        EofSymbol
    >, FlattenType::None, PrologSymbolID::PrologQuery, PrologQueryError>
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
                    PrologFunctor
                >>,
                PrologOptionalWhitespace,
                CharacterSymbol<PeriodString>,
                PrologOptionalWhitespace
            >>
        >,
        PrologOptionalWhitespace,
        EofSymbol
    >, FlattenType::None, PrologSymbolID::PrologDocument, PrologDocumentError>
    {
    };
}
#endif /* PrologParser_hpp */
