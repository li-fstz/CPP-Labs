#ifndef REMOVELEFTRECURSION_H_
#define REMOVELEFTRECURSION_H_

#define POSTFIX "'"

#include "rule.h"

int RuleHasLeftRecursion(const Rule *rule);
void AddProductionToRule(Rule *rule, const Production *newProduction);
void FreeProduction(Production *production);
Production *ReplaceProduction(const Production *productionTemplate);
void RemoveLeftRecursion(Rule *ruleHead);

#endif
