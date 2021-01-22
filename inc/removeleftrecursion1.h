#ifndef REMOVELEFTRECURSIONNOREPLACE_H_
#define REMOVELEFTRECURSIONNOREPLACE_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

void AddSymbolToProduction(Production *production, Symbol *newSymbol);
void AddProductionToRule(Rule *rule, Production *newProduction);
void RemoveLeftRecursion(Rule *ruleHead);

#endif
