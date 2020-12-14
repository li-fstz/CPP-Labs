#ifndef REMOVELEFTRECURSION_H_
#define REMOVELEFTRECURSION_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

int RuleHasLeftRecursion(const Rule *pRule);
void AddSymbolToProduction(Production *pProduction, const Symbol *pNewSymbol);
void AddProductionToRule(Rule *pRule, Production *pNewProduction);
void FreeProduction(Production *pProduction);
Production *ReplaceProduction(const Production *pProductionTemplate);
void RemoveLeftRecursion(Rule *pRuleHead);

#endif
