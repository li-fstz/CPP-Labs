#ifndef REMOVELEFTRECURSION_H_
#define REMOVELEFTRECURSION_H_

#define POSTFIX "'"

#include "rule.h"
#include <stdio.h>

int RuleHasLeftRecursion(const Rule *rule);
void AddSymbolToProduction(Production *production, const Symbol *newSymbol);
void AddProductionToRule(Rule *rule, Production *newProduction);
void FreeProduction(Production *production);
Production *ReplaceProduction(const Production *productionTemplate);
void RemoveLeftRecursion(Rule *ruleHead);

#endif
