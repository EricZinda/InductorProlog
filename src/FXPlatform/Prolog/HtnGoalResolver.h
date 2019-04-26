//
//  HtnGoalResolver.hpp
//  GameLib
//
//  Created by Eric Zinda on 9/29/18.
//  Copyright © 2018 Eric Zinda. All rights reserved.
//

#ifndef HtnGoalResolver_hpp
#define HtnGoalResolver_hpp
#include <functional>
#include "HtnRule.h"
#include "HtnTerm.h"
enum class ResolveContinuePoint;
class ResolveNode;
class ResolveState;
class HtnRuleSet;
class HtnTerm;

typedef pair<shared_ptr<HtnTerm>, shared_ptr<HtnTerm>> UnifierItemType;
typedef vector<UnifierItemType> UnifierType;
typedef pair<shared_ptr<HtnRule>, UnifierType> RuleBindingType;

// Performs Prolog-style resolution of a set of terms against a database of rules as well as unification 
class HtnGoalResolver
{
public:
    typedef std::function<void(ResolveState *)> CustomRuleType;

    HtnGoalResolver();
    void AddCustomRule(const string &name, CustomRuleType);
    static shared_ptr<HtnTerm> ApplyUnifierToTerm(HtnTermFactory *termFactory, UnifierType unifier, shared_ptr<HtnTerm>term);
    static shared_ptr<vector<RuleBindingType>> FindAllRulesThatUnify(HtnTermFactory *termFactory, HtnRuleSet *prog, shared_ptr<HtnTerm> goal, int *uniquifier, int indentLevel, int memoryBudget, bool fullTrace, int64_t *highestMemoryUsedReturn);
    static shared_ptr<HtnTerm> FindTermEquivalence(const UnifierType &unifier, const HtnTerm &termToFind);
    static bool IsGround(UnifierType *unifier);
    bool HasCustomRule(const string &name);
    // Always check factory->outOfMemory() after calling to see if we ran out of memory during processing and the resolutions might not be complete
    shared_ptr<vector<UnifierType>> ResolveAll(HtnTermFactory *termFactory, HtnRuleSet *prog, const vector<shared_ptr<HtnTerm>> &initialGoals, int initialIndent = 0, int memoryBudget = 1000000, int64_t *highestMemoryUsedReturn = nullptr);
    // Always check factory->outOfMemory() after calling to see if we ran out of memory during processing and the resolutions might not be complete
    shared_ptr<UnifierType> ResolveNext(ResolveState *state);
    static shared_ptr<HtnTerm> SubstituteUnifiers(HtnTermFactory *factory, const UnifierType &source, shared_ptr<HtnTerm> target);
    static shared_ptr<UnifierType> SubstituteUnifiers(HtnTermFactory *factory, const UnifierType &source, const UnifierType &destination);
    static shared_ptr<vector<shared_ptr<HtnTerm>>> SubstituteUnifiers(HtnTermFactory *factory, const UnifierType &source, const vector<shared_ptr<HtnTerm>> &terms);
    static string ToString(const vector<UnifierType> *unifierList);
    static string ToString(const UnifierType &unifier);
    static shared_ptr<UnifierType> Unify(HtnTermFactory *factory, shared_ptr<HtnTerm> term1, shared_ptr<HtnTerm> term2);

private:
    static void RuleAggregate(ResolveState *state);
    static void RuleCount(ResolveState *state);
    static void RuleDistinct(ResolveState *state);
    static void RuleFirst(ResolveState *state);
    static void RuleIs(ResolveState *state);
    static void RuleNot(ResolveState *state);
    static void RulePrint(ResolveState *state);
    static void RuleSortBy(ResolveState *state);
    static void RuleTermCompare(ResolveState *state);
    static void RuleTrace(ResolveState *state);
    static void RuleUnify(ResolveState *state);
    static void SubstituteAllVariables(HtnTermFactory *factory, shared_ptr<HtnTerm> newTerm, shared_ptr<HtnTerm> existingVariable, vector<pair<shared_ptr<HtnTerm>, shared_ptr<HtnTerm>>> &stack, UnifierType &solution);

    typedef map<string, CustomRuleType> CustomRulesType;
    CustomRulesType m_customRules;
};

enum class ResolveContinuePoint
{
    CustomStart,
    CustomContinue1,
    CustomContinue2,
    CustomContinue3,
    CustomContinue4,
    NextGoal,
    NextRuleThatUnifies,
    ProgramError,
    Return
};

class ResolveNode
{
public:
    typedef set<shared_ptr<HtnTerm>, HtnTermComparer> TermSetType;
    ResolveNode(shared_ptr<vector<shared_ptr<HtnTerm>>> resolventArg, shared_ptr<UnifierType> unifierArg);
    void AddToSolutions(shared_ptr<vector<UnifierType>> &solutions);
    static shared_ptr<ResolveNode> CreateInitialNode(const vector<shared_ptr<HtnTerm>> &resolventArg, const UnifierType &unifierArg);
    shared_ptr<ResolveNode> CreateChildNode(HtnTermFactory *termFactory, const vector<shared_ptr<HtnTerm>> &originalGoals, const vector<shared_ptr<HtnTerm>> &additionalResolvents, const UnifierType &additionalSubstitution);
    shared_ptr<HtnTerm> currentGoal()
    {
        return (m_resolvent == nullptr || m_resolvent->size() == 0) ? nullptr : (*m_resolvent)[0];
    }
    
    RuleBindingType currentRule()
    {
        FailFastAssert(rulesThatUnify != nullptr && currentRuleIndex < rulesThatUnify->size());
        return (*rulesThatUnify)[currentRuleIndex];
    }

    int64_t dynamicSize()
    {
        if(cachedDynamicSize == -1)
        {
            CalcDynamicSize();
        }
        
        return cachedDynamicSize;
    }
    
    void CalcDynamicSize()
    {
        int64_t rulesThatUnifySize = 0;
        if(rulesThatUnify != nullptr)
        {
            rulesThatUnifySize = sizeof(rulesThatUnify);
            for(auto rule : *rulesThatUnify)
            {
                rulesThatUnifySize += sizeof(RuleBindingType) + rule.second.size() * sizeof(UnifierItemType);
            }
        }

        int64_t previousSolutionsSize = 0;
        if(previousSolutions != nullptr)
        {
            previousSolutionsSize = sizeof(previousSolutions);
            for(auto item : *previousSolutions)
            {
                previousSolutionsSize += sizeof(UnifierType) + item.size() * sizeof(UnifierItemType);
            }
        }

        int64_t variablesToKeepSize = 0;
        if(variablesToKeep != nullptr)
        {
            variablesToKeepSize = sizeof(variablesToKeep);
            for(auto item : *variablesToKeep)
            {
                variablesToKeepSize += sizeof(HtnTerm) + item->dynamicSize();
            }
        }

        cachedDynamicSize = sizeof(ResolveNode) +
            (m_resolvent == nullptr ? 0 : sizeof(m_resolvent) + m_resolvent->size() * sizeof(shared_ptr<HtnTerm>)) +
            rulesThatUnifySize +
            (unifier == nullptr ? 0 : sizeof(unifier) + unifier->size() * sizeof(UnifierItemType)) +
            (previousSolutions == nullptr ? 0 : sizeof(previousSolutions) + unifier->size() * sizeof(UnifierItemType)) +
            previousSolutionsSize +
            variablesToKeepSize;
    }
    
    void PopStandaloneResolve(ResolveState *state);
    void PushStandaloneResolve(ResolveState *state, shared_ptr<TermSetType> additionalVariablesToKeep, vector<shared_ptr<HtnTerm>>::const_iterator firstTerm, vector<shared_ptr<HtnTerm>>::const_iterator lastTerm, ResolveContinuePoint continuePointArg);
    static shared_ptr<UnifierType> RemoveUnusedUnifiers(shared_ptr<TermSetType> variablesToKeep, const UnifierType &currentUnifiers, const vector<shared_ptr<HtnTerm>> &originalGoals, const vector<shared_ptr<HtnTerm>> &resolvent);
    int CountOfGoalsLeftToProcess()
    {
        // If the current goal is original, don't count it. We want the ones that are not "in progress"
        return originalGoalCount;
    }
    
    bool SetNextRule()
    {
        currentRuleIndex++;
        return rulesThatUnify != nullptr && currentRuleIndex < rulesThatUnify->size();
    }
    
    // NOTE: If you change members, remember to change dynamicSize() function too
    ResolveContinuePoint continuePoint;
    int currentRuleIndex;
    // Remembers the count of original goals which will be at the end of m_resolvent, so we can debug better
    int originalGoalCount;
    const shared_ptr<vector<shared_ptr<HtnTerm>>> &resolvent() const { return m_resolvent; };
    shared_ptr<vector<RuleBindingType>> rulesThatUnify;
    shared_ptr<UnifierType> unifier;
    
private:
    void PopResolver(ResolveState *state);
    void PushResolver(ResolveState *state);
    
    // NOTE: If you change members, remember to change dynamicSize() function too
    // The size of a node doesn't change unless it is being actively worked on, so we cache it
    int64_t cachedDynamicSize;
    bool isStandaloneResolve; // True for all child nodes of a standalone resolve
    bool pushedStandaloneResolver;
    shared_ptr<vector<UnifierType>> previousSolutions;
    shared_ptr<TermSetType> variablesToKeep;
    bool previousCollectAllSolutions;
    shared_ptr<vector<shared_ptr<HtnTerm>>> m_resolvent;
};

class ResolveState
{
public:
    ResolveState(HtnTermFactory *termFactoryArg, HtnRuleSet *progArg, const vector<shared_ptr<HtnTerm>> &initialResolventArg, int initialIndentArg, int memoryBudgetArg);
    void ClearFailures()
    {
        deepestFailure = -1;
        deepestFailureGoal = nullptr;
        deepestFailureOriginalGoalIndex = -1;
    }
    string GetStackString();
    int64_t RecordMemoryUsage(int64_t &initialTermMemory, int64_t &initialRuleSetMemory);
    void RecordFailure(shared_ptr<HtnTerm> goal, int goalsLeftToProcess);
    static shared_ptr<UnifierType> SimplifySolution(const UnifierType &solution, vector<shared_ptr<HtnTerm>> &goals);

    int64_t dynamicSize()
    {
        int64_t stackSize = sizeof(vector<shared_ptr<ResolveNode>>);
        for(int stackIndex = 0; stackIndex < ((int) resolveStack->size()) - 1; ++stackIndex)
        {
            stackSize += (*resolveStack)[stackIndex]->dynamicSize();
        }
        
        // The last one needs to be recalculated since it is being worked on and will change
        if(resolveStack->size() > 0)
        {
            resolveStack->back()->CalcDynamicSize();
            stackSize += resolveStack->back()->dynamicSize();
        }
        
        return sizeof(ResolveState) +
            deepestFailureStack.size() + highestMemoryUsedStack.size() +
            initialGoals->size() * sizeof(shared_ptr<HtnTerm>) +
            stackSize +
            (solutions == nullptr ? 0 : (sizeof(vector<UnifierType>) + solutions->size() * sizeof(UnifierItemType)));
    }

    // NOTE: If you change members, remember to change dynamicSize() function too
    bool collectAllSolutions;
    bool fullTrace;
    int64_t highestMemoryUsed;
    string highestMemoryUsedStack;
    shared_ptr<vector<shared_ptr<HtnTerm>>> initialGoals;
    int initialIndent;
    int memoryBudget;
    HtnRuleSet *prog;
    shared_ptr<vector<shared_ptr<ResolveNode>>> resolveStack;
    int64_t ruleMemoryUsed;
    shared_ptr<vector<UnifierType>> solutions;
    int64_t stackMemoryUsed;
    HtnTermFactory *termFactory;
    int64_t termMemoryUsed;
    int uniquifier;
    
    int deepestFailure;
    shared_ptr<HtnTerm> deepestFailureGoal;
    int deepestFailureOriginalGoalIndex;
    string deepestFailureStack;

};
#endif /* HtnGoalResolver_hpp */
