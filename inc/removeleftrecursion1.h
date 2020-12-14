#ifndef REMOVELEFTRECURSIONNOREPLACE_H_
#define REMOVELEFTRECURSIONNOREPLACE_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

void AddSymbolToProduction(Production *pProduction, Symbol *pNewSymbol);
void AddProductionToRule(Rule *pRule, Production *pNewProduction);
void RemoveLeftRecursion(Rule *pRuleHead);

#endif
