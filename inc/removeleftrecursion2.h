#ifndef REMOVELEFTRECURSION_H_
#define REMOVELEFTRECURSION_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

int RuleHasLeftRecursion(Rule *pRule);
void AddSymbolToProduction(Production *pProduction, Symbol *pNewSymbol);
void AddProductionToRule(Rule *pRule, Symbol *pNewProduction);
void FreeProduction(Production *pProduction);
Symbol *ReplaceProduction(const Production *pProductionTemplate);
void RemoveLeftRecursion(Rule *pRuleHead);

#endif /* REMOVELEFTRECURSION_H_ */
